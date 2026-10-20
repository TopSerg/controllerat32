"""Preserve static SDO evidence and an unexecuted read plan.

This program reads local firmware/disassembly only. It has no CAN, serial,
debugger or target-execution dependencies. No generated frame is transmitted.
"""
import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REF = ROOT / 'test_logs/sintech_reference_20261020'
OUT = REF / 'static_analysis'

FUNCTIONS = {
    'SDO_process': (0x3E674C, 0x3E6D9D, 0x3E4860),
    'SDO_init': (0x3E95B0, 0x3E968F, 0x3E7712),
    'SDO_receive': (0x3EA744, 0x3EA7C4, 0x3E88A6),
    'SDO_initTransfer': (0x3EAC1C, 0x3EAC94, 0x3E8D7E),
    'SDO_readOD': (0x3EB481, 0x3EB4D5, 0x3E95E3),
    'OD_find': (0x3EBC20, 0x3EBC60, 0x3E9D82),
    'OD_configure': (0x3EBEAD, 0x3EBEE3, 0x3EA00F),
    'RPDO_communication_callback': (0x3EA7C4, 0x3EA842, 0x3E8926),
    'RPDO_mapping_callback': (0x3EB5CF, 0x3EB621, 0x3E9731),
}


def excerpt(lines, start, end):
    return [line for line in lines if re.match(r'^[0-9a-f]{8}\s', line)
            and start <= int(line[:8], 16) < end]


def normalized(lines):
    result = []
    for line in lines:
        match = re.match(r'^[0-9a-f]{8}\s+[0-9a-f]{4}\s+(.+)$', line)
        if match:
            result.append(re.sub(r'0x3[ef][0-9a-f]{4}', '<FLASH>', match[1]).strip())
    return result


def read_frame(index, subindex=0):
    return bytes([0x40, index & 255, index >> 8, subindex, 0, 0, 0, 0]).hex(' ').upper()


def main():
    listings = [(OUT / f'c28x/SinTechController_v{n}.dis').read_text().splitlines()
                for n in (1, 2)]
    tables = json.loads((OUT / 'firmware_objects.json').read_text())
    evidence_dir = OUT / 'sdo_evidence'
    evidence_dir.mkdir(exist_ok=True)
    comparisons = {}
    for name, (start, end, second_start) in FUNCTIONS.items():
        parts = [excerpt(listings[0], start, end),
                 excerpt(listings[1], second_start, second_start + end - start)]
        instructions = [normalized(part) for part in parts]
        assert instructions[0] == instructions[1], name
        for version, part in enumerate(parts, 1):
            (evidence_dir / f'{name}_v{version}.asm').write_text('\n'.join(part) + '\n', encoding='utf-8')
        comparisons[name] = dict(v1_word_address=hex(start), v2_word_address=hex(second_start),
                                 word_length=end-start, normalized_instructions_equal=True,
                                 instruction_count=len(instructions[0]),
                                 normalized_sha256=hashlib.sha256('\n'.join(instructions[0]).encode()).hexdigest())

    points = [
        ('server_initialization', 0x3E7AA6, 0x3E7ACA,
         'Adds node ID to 0x580 and 0x600, passes the 99-entry OD and index 0x1200 to SDO_init.'),
        ('command_and_object_decode', 0x3E6781, 0x3E67CF,
         'Extracts client command from RX byte 0 >> 5; combines RX bytes 1,2 into index, byte 3 is subindex. Calls initTransfer; upload calls readOD.'),
        ('upload_permissions_and_copy', 0x3EB481, 0x3EB4D5,
         'Tests readable attribute bit 2, copies OD data to transfer buffer, optionally invokes the registered per-object callback.'),
        ('response_header_and_length', 0x3E6A7D, 0x3E6AD7,
         'Echoes object index/subindex. Length >4 uses segmented upload; shorter values use expedited upload.'),
        ('expedited_response_command', 0x3E6BB6, 0x3E6BC9,
         'For length 1, copies a low byte; command is 0x43 OR (0x10 - (length << 2)); length 4 produces 0x43.'),
        ('response_send', 0x3E6D8B, 0x3E6D93,
         'Sends the assembled response through the configured CAN TX buffer.'),
        ('rpdo_communication_read_callback', 0x3EA826, 0x3EA842,
         'Read path modifies the transfer buffer to report the effective COB-ID and disabled bit, rather than reconfiguring RPDO reception.'),
        ('rpdo_mapping_read_callback', 0x3EB616, 0x3EB621,
         'For nonzero subindex returns success; subindex zero may set the count in the response buffer to zero if RPDO mapping is invalid.'),
    ]
    evidence = []
    for name, start, end, finding in points:
        filename = evidence_dir / f'{name}.asm'
        filename.write_text('\n'.join(excerpt(listings[0], start, end)) + '\n', encoding='utf-8')
        evidence.append(dict(name=name, word_start=hex(start), word_end_exclusive=hex(end),
                             finding=finding, file=str(filename.relative_to(ROOT))))

    planned = [(0x1000, 0, 'First single request: device type'),
               (0x100A, 0, 'Software-version object; retain raw bytes before decoding strings')]
    planned += [(0x1018, sub, 'Identity') for sub in range(5)]
    planned += [(0x1400, sub, 'Actual RPDO1 communication settings') for sub in range(3)]
    planned += [(0x1600, sub, 'Actual RPDO1 mapping') for sub in range(5)]
    requests = []
    for index, subindex, purpose in planned:
        for table in tables:
            entry = next(obj for obj in table['objects'] if obj['index'] == f'0x{index:04X}')
            assert subindex <= entry['max_subindex']
            record = entry if entry['attribute'] else entry['records'][subindex]
            assert record['attribute'] & 4  # readable in both downloaded dictionaries
        requests.append(dict(index=f'0x{index:04X}', subindex=subindex,
                             request_data=read_frame(index, subindex), purpose=purpose))

    report = dict(
        method='Official TI dis2000 19.6.0.STS disassembly; manual control-flow inspection with preserved evidence.',
        hardware_access=False, application_can_transmissions=0,
        source_images=[dict(path=table['source'], sha256=table['sha256']) for table in tables],
        function_comparison=comparisons, evidence=evidence,
        read_plan=dict(executed=False, node_id_hypothesis='0x30',
                       request_cob_id_if_node_matches='0x630', response_cob_id_if_node_matches='0x5B0',
                       format='Classic CAN, standard 11-bit ID, DLC 8', requests=requests,
                       sequence='One request first; inspect the matching reply or timeout before any follow-up. No NMT, RPDO, configuration writes, node scan or periodic replay.'),
        conclusions=[
            'Both downloaded images implement CANopen-style SDO upload (read), not merely OD-looking constant tables.',
            'Examined read/control-flow functions match across both images after normalization of flash addresses.',
            'The examined upload path does not call the OD write function or the RPDO control-data receive path.',
            '0x1000, 0x100A, 0x1018, 0x1400 and 0x1600 are present and readable in both downloaded dictionaries.'
        ],
        limitations=[
            'No read request has been sent to the inverter; installed firmware and node ID remain unverified.',
            'Static analysis is not a runtime safety guarantee for an unidentified installed image.',
            'The previously agreed first-probe hardware torque-exclusion condition remains unresolved.',
            'Both file images contain software-version text 3.00; version text alone cannot distinguish them.',
            'C28x stores characters in 16-bit address units. Do not assume SDO version bytes are the literal ASCII string 3.00.',
            'This comparison covers SDO and selected callbacks, not the whole firmware or speed-command semantics.'
        ])
    destination = OUT / 'sdo_analysis.json'
    destination.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print('Matching SDO/OD functions:', len(comparisons))
    print('Read requests prepared (not sent):', len(requests))
    print(destination)


if __name__ == '__main__':
    main()
