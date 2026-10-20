"""Preserve the recovered CAN demand path and its remaining limitations.

Only local images, listings and already recorded SDO results are read.
Generated pseudocode is a review artifact, not a controller implementation.
"""
import hashlib
import json
import math
import re
import struct
from pathlib import Path

from inspect_sintech_c28x import ROOT, REF, OUT, COPIES, instructions

RAM_RANGES = {
    'CAN_source_demands': (0xD069,0xD0A3),
    'speed_mode_selection': (0xC946,0xC95E),
    'speed_reference_limits_slew_and_PI': (0xC95E,0xC9FB),
    'torque_mode': (0xC9FB,0xCA30),
    'PI_algorithm': (0xE228,0xE291),
    'run_request_gate': (0xCE7D,0xCE8B),
    'PWM_enable_gate': (0xC281,0xC2FD),
    'angle_speed_observer': (0xD952,0xDA96),
    'feedback_to_control': (0xC366,0xC37D),
    'feedback_to_rpm': (0xD0D0,0xD104),
    'float_division': (0x9738,0x9751),
}


def main():
    destination = REF/'static_analysis/control_evidence'
    destination.mkdir(exist_ok=True)
    decoded_instructions = {v:list(instructions(v,True)) for v in (1,2)}
    comparisons = {}
    for name,(start,end) in RAM_RANGES.items():
        versions = []
        for version in (1,2):
            part = [(pc,ins,line) for pc,ins,line in decoded_instructions[version] if start<=pc<end]
            (destination/f'{name}_v{version}.asm').write_text(
                '\n'.join(line for _,_,line in part)+'\n',encoding='utf-8')
            versions.append([re.sub(r'0x3[ef][0-9a-f]{4}','<FLASH>',ins) for _,ins,_ in part])
        assert versions[0] == versions[1], name
        comparisons[name] = dict(run_word_start=hex(start), run_word_end_exclusive=hex(end),
                                 versions_match_after_flash_address_normalization=True,
                                 instructions=len(versions[0]),
                                 normalized_sha256=hashlib.sha256('\n'.join(versions[0]).encode()).hexdigest())

    # These additional flash snippets connect the RAM variables to OD entries,
    # the actual little-endian PDO unpacker, and initialization parameters.
    extra = [('RPDO_unpack_and_state_gate',0x3EB978,0x3EB9BF),
             ('pole_pairs_to_control',0x3E6E03,0x3E6E0B),
             ('speed_limit_setup',0x3E6E25,0x3E6E2D),
             ('copy_calls',0x3E7DA6,0x3E7DBC)]
    flash = list(instructions(1,False))
    for name,start,end in extra:
        (destination/f'{name}_v1.asm').write_text('\n'.join(
            line for pc,_,line in flash if start<=pc<end)+'\n',encoding='utf-8')

    # Diagnostic descriptors: native 32-bit pointer, 16-bit type, 21 wide chars.
    image = next(REF.rglob('SinTechController_v1.bin')).read_bytes()
    labels = []
    for offset in range(0x179E4,0x18104,48):
        pointer,kind = struct.unpack_from('<IH',image,offset)
        label = image[offset+6:offset+48].decode('utf-16-le').split('\0')[0]
        labels.append(dict(file_offset=offset, pointer_word_address=hex(pointer), type_code=kind,label=label))
    assert next(x for x in labels if x['label']=='ElectroSpeed')['pointer_word_address']=='0xb208'
    assert next(x for x in labels if x['label']=='Fz')['pointer_word_address']=='0xb1ce'
    assert next(x for x in labels if x['label']=='Mz')['pointer_word_address']=='0xb1c6'

    live = ROOT/'test_logs/sintech_identity_pdo_20261020_165513/summary.json'
    session = json.loads(live.read_text())
    assert session['outcome']=='completed' and session['application_write_calls']==3
    values = {(int(x['index'],16),x['subindex']):x['value_unsigned'] for x in session['transactions']}
    source,poles,speed_limit = [values[key] for key in [(0x6411,8),(0x6410,5),(0x6410,6)]]
    assert poles>0
    tables = json.loads((REF/'static_analysis/firmware_objects.json').read_text())
    for fw in tables:
        od={int(x['index'],16):x for x in fw['objects']}
        assert od[0x6411]['records'][8]['pointer']==0xA309
        assert od[0x6410]['records'][5]['pointer']==0xA2F2
        assert od[0x6410]['records'][6]['pointer']==0xA2F3
        assert [od[index]['pointer'] for index in [0x6040,0x6074,0x606B,0x6062]]==[
            0xAFE0,0xAFE9,0xAFE7,0xAFE5]

    pseudocode = '''/* Reconstructed from both supplied SINtech images; NOT executable drive code.
 * RAM addresses below are C28x 16-bit word addresses. No CAN TX is implemented.
 */
if (OD_6411_sub8 == 1) {
    internal_controlword = OD_6040;              /* raw 16-bit copy */
    mode = (internal_controlword >> 3) & 7;
    if (mode == 1) {
        speed_request_Fz = (float)(int16_t)OD_606B;
        speed_PI_max_output = (float)(int16_t)OD_6074;
        speed_PI_min_output = -speed_PI_max_output;
    }
    if (mode == 0)
        torque_request_Mz = (float)(int16_t)OD_6074;
    /* OD_6062 is not consumed in this examined bridge. */
}

/* Speed mode applies configured limits and ramps before PI. The feedback
 * B208 is the electrical angular speed also used by the angle observer. */
speed_reference = limit_and_slew(speed_request_Fz, configured_limits_and_ramps);
speed_PI.reference = speed_reference;
speed_PI.feedback = electrical_angular_speed;
torque_request_Mz = speed_PI.output;

/* The run request is only one gate; ready/state/fault/PWM logic also applies. */
status_bit2 = (status_bit1 && (internal_controlword & 1));

/* Existing telemetry conversion in the image (constant is ~9.55). */
mechanical_rpm = electrical_angular_speed / configured_pole_pairs * 9.55f;
'''
    (destination/'reconstructed_control_path.c').write_text(pseudocode,encoding='utf-8')
    report = dict(
        method='Static TI C28x disassembly including RAM copies; compared both supplied images. Three reviewed SDO uploads read current settings.',
        source_images=[dict(path=fw['source'],sha256=fw['sha256']) for fw in tables],
        ram_copies={str(v):[dict(load=hex(a),run=hex(b),words=n) for a,b,n in items] for v,items in COPIES.items()},
        compared_paths=comparisons, diagnostic_labels=labels,
        live_settings=dict(log=str(live.relative_to(ROOT)),source_selector=source,
                           source_interpretation='Analog' if source==0 else 'CAN' if source==1 else 'unknown',
                           configured_pole_pairs=poles,electrical_speed_limit_raw=speed_limit,
                           bus_status=session['final_bus_status'],can_requests=3,
                           configuration_writes=0,control_frames=0),
        rpdo=dict(can_id='0x230',runtime_mapping_log='test_logs/sintech_identity_pdo_20261020_164505/summary.json',
                  packing='Four 16-bit little-endian values, confirmed by the native RPDO unpacker.',
                  fields=[
                      dict(byte_offset=0,index='0x6040',type='uint16',meaning='Copied control word; bits 3..5 select mode, bit 0 contributes run request.'),
                      dict(byte_offset=2,index='0x6074',type='int16',meaning='Mode 0: Mz demand; mode 1: speed PI positive limit and its negative counterpart.',units='Mz/controller-output units; Nm calibration not verified.'),
                      dict(byte_offset=4,index='0x606B',type='int16',meaning='Mode 1: electrical angular-speed demand, converted to float without a multiplier.',units_from_code='electrical rad/s; not yet validated by commanded rotation.'),
                      dict(byte_offset=6,index='0x6062',type='unknown',meaning='Mapped to RAM but not consumed in the examined CAN demand bridge.')]),
        controlword=dict(mode_mask='0x0038',mode_shift=3,speed_mode_value=1,torque_mode_value=0,
                         run_request_bit=0,reset_like_bit=1,
                         limitation='Bit definitions do not constitute a validated drive start/stop sequence; further state/fault/input gates apply.'),
        speed_conversion=dict(basis='Angle observer wraps at 2*pi; its speed is the PI feedback. Existing firmware converts this feedback to RPM by dividing by configured pole pairs and multiplying by 9.55.',
                              exact_formula='rpm = raw * 60 / (2*pi*pole_pairs)',
                              rpm_per_raw_unit_from_config=60/(2*math.pi*poles),
                              firmware_approximate_formula='rpm = raw * 9.55 / pole_pairs',
                              firmware_rpm_per_unit=9.55/poles,
                              configured_limit_rpm_from_formula=speed_limit*60/(2*math.pi*poles),
                              validation='Derived from code and read configuration; physical scale and direction require controlled experimental confirmation.'),
        timeout_assessment='The examined RPDO processor only copies new data while valid and NMT Operational (5). It retains the mapped values without a local RX-age timeout. The CAN demand bridge reads the retained values each cycle. No complete application stop-on-CAN-loss guarantee has been established.',
        remaining=['Exact installed binary identity remains unverified, although live mappings and parameters match the recovered layout.',
                   'Torque engineering scale and acceptable magnitude remain unverified.',
                   'Physical speed scale/direction require validation using the independent Gen1.1 reference.',
                   'Complete start/stop sequence, discrete-input interactions and lost-CAN behavior remain unverified.',
                   'No mode switch, torque request, speed request or motor-start test has been performed.'])
    path=REF/'static_analysis/control_analysis.json'
    path.write_text(json.dumps(report,indent=2),encoding='utf-8')
    print('Compared matching RAM paths:',len(comparisons))
    print('Live control source:',source,'pole pairs:',poles,'raw speed limit:',speed_limit)
    print('Derived rpm/raw unit:',report['speed_conversion']['rpm_per_raw_unit_from_config'])
    print(path)


if __name__=='__main__':
    main()
