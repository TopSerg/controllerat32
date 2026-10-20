"""Write reproducible evidence from offline BLDC_uart.exe disassembly.

Only local files are read. No executable, CAN adapter or serial port is opened.
The packet reconstruction below is for Serial/Bluetooth, never CANopen SDO.
"""
import contextlib
import hashlib
import json
from pathlib import Path

from inspect_sintech_exe import DATA, ROOT, dump


def main():
    out=ROOT/'test_logs/sintech_reference_20261020/static_analysis'
    out.mkdir(exist_ok=True)
    functions={
        'Read_CAN':(0x4D8D40,0x4D8DD1),
        'Write_CAN':(0x4DAAC0,0x4DAC3A),
        'Read_Control':(0x4D8E00,0x4D8E91),
        'serial_packet_builder':(0x475370,0x4754D8),
        'serial_packet_validator':(0x4751A0,0x47530E),
        'Serial_BLE_transport_write':(0x4DD3B0,0x4DD4B7),
        'readData_serial':(0x4DCE10,0x4DD000),
        'CAN_settings_reply_decoder':(0x4D4969,0x4D4A93),
        'Speed_Mode_status_display':(0x4D77FC,0x4D786F),
        'PDO_graph_signal_from_selected_serial_variables':(0x4D7D86,0x4D7E9D),
    }
    for name,(start,end) in functions.items():
        with (out/f'{name}.asm').open('w',encoding='utf-8') as destination:
            with contextlib.redirect_stdout(destination):
                dump(start,end)
    payload=bytes([7,0])+bytes(28)
    checksum=sum(payload).to_bytes(2,'little')
    frame=b'#'+payload+checksum+b'*'
    strings=('sdo.cfg','hand.cfg','auto.cfg','SDO Objects','canChannel','cfg/graph.js')
    report={
        'source':'software_files/BLDC_uart.exe',
        'source_sha256':hashlib.sha256(DATA).hexdigest(),
        'method':'Static PE imports, Qt metaobject method dispatch, x86 instructions; no target program executed.',
        'hardware_access':False,
        'function_addresses':{name:hex(address[0]) for name,address in functions.items()},
        'confirmed_in_examined_program':{
            'transport':'Read_CAN and Write_CAN call a 34-byte packet builder and then Serial/Bluetooth transport.',
            'framing':{'length_bytes':34,'start':0x23,'end':0x2A,'payload_bytes':30,
                       'checksum':'unsigned sum of wire bytes 1..30; uint16 little-endian at wire bytes 31..32'},
            'can_settings_read':{'transport':'Serial/Bluetooth, NOT Classic CAN',
                                 'opcode':7,'read_flag':0,'frame_hex':frame.hex(' ').upper(),
                                 'wire_tested':False},
            'can_settings_write_layout':{'opcode':7,'write_flag':1,
                'payload_byte_2':'uint16 little-endian CAN ID, user input masked with 0x7FF',
                'payload_byte_4':'uint16 little-endian bitrate selector from UI item data',
                'payload_byte_6':'uint16 little-endian CAN/Analog control selector; 1 for CAN radio button',
                'note':'Configuration write, not a recovered speed demand message; not generated or sent.'},
            'can_settings_read_response':{'opcode':7,'read_flag':0,
                'payload_byte_2':'uint16 CAN ID & 0x7FF',
                'payload_byte_4':'uint16 bitrate selector',
                'payload_byte_6':'uint16 control selector & 1'},
            'speed_mode_label':'Speed Mode is rendered from status word bit 6 at 0x004D7839; label alone does not identify a speed command.',
            'graph_data':'Examined PDO graph emitters pack UI-selected serial variables into graph events; graph names alone do not establish CAN payload mapping.'
        },
        'string_presence':{name:{'ascii':DATA.find(name.encode())>=0,
                                 'utf16le':DATA.find(name.encode('utf-16-le'))>=0} for name in strings},
        'not_confirmed':['CANopen compatibility of installed inverter firmware',
                         'SDO COB-IDs 0x630/0x5B0 and support for object 0x1000',
                         'Whether sdo.cfg and hand.cfg are used by this particular executable',
                         'CAN receive command ID and payload for speed demand',
                         'Which downloaded firmware image matches the connected unit'],
    }
    (out/'report.json').write_text(json.dumps(report,indent=2,ensure_ascii=False),encoding='utf-8')
    print(json.dumps(report,indent=2,ensure_ascii=False))


if __name__=='__main__':
    main()
