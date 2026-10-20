"""Extract CANopen-like OD/PDO tables from the two supplied C28x images.

Offline only. No device is opened, firmware executed, or CAN message sent.
Defaults in downloaded files are not evidence of settings in a running unit.
"""
import hashlib
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REFERENCE = ROOT / 'test_logs/sintech_reference_20261020'


def analyze(path):
    data = path.read_bytes()
    start = data.index(bytes.fromhex('00 10 00 00 86 00 04 00'))
    entries = {}
    offset = start
    previous = 0
    while offset + 12 <= len(data):
        index, sub, attribute, length, pointer = struct.unpack_from('<HHHHI', data, offset)
        if not previous < index <= 0xFFFF or attribute > 0xFF:
            break
        entries[index] = dict(index=f'0x{index:04X}', max_subindex=sub,
                              attribute=attribute, logical_length=length,
                              pointer=pointer, file_offset=offset)
        previous = index
        offset += 12

    # Locate the descriptor array independently of its flash address. All
    # pointers use C28x 16-bit address units; file offsets use 8-bit bytes.
    descriptor = struct.pack('<IHHIHH', 0xA092, 0x0D, 1, 0xA094, 0x8D, 4)
    assert data.count(descriptor) == 1
    descriptor_offset = data.index(descriptor)
    flash_base = entries[0x1A00]['pointer'] - descriptor_offset // 2
    assert flash_base == 0x3E4000

    # Consistent initialization block: A008 points at the device-name string,
    # A030/A032/A034 hold 2, 0x600, 0x580, followed by all PDO defaults.
    name_offset = data.index('PowerController'.encode('utf-16-le'))
    init_offset = name_offset - 2 * 8

    def initial_scalar(pointer, length):
        if length not in (1, 2, 4) or not 0xA000 <= pointer < 0xA300:
            return None
        physical_length = 2 if length in (1, 2) else 4
        location = init_offset + 2 * (pointer - 0xA000)
        return dict(value=int.from_bytes(data[location:location + physical_length], 'little'),
                    file_offset=location)

    for entry in entries.values():
        if entry['attribute'] == 0 and entry['pointer'] >= flash_base:
            record_offset = 2 * (entry['pointer'] - flash_base)
            assert 0 <= record_offset < len(data)
            records = []
            for subindex in range(entry['max_subindex'] + 1):
                location = record_offset + subindex * 8
                pointer, attribute, length = struct.unpack_from('<IHH', data, location)
                assert attribute <= 0xFF and length <= 256
                records.append(dict(subindex=subindex, pointer=pointer,
                                    attribute=attribute, logical_length=length,
                                    descriptor_file_offset=location,
                                    initial_scalar=initial_scalar(pointer, length)))
            entry['records'] = records

    def value(index, subindex):
        return entries[index]['records'][subindex]['initial_scalar']['value']

    assert [value(0x1200, sub) for sub in range(3)] == [2, 0x600, 0x580]
    pdos = []
    for comm, mapping, direction in [(0x1400, 0x1600, 'RX')] + [
        (0x1800 + n, 0x1A00 + n, 'TX') for n in (0, 1, 2, 3, 11, 12)
    ]:
        fields = []
        for sub in range(1, value(mapping, 0) + 1):
            raw = value(mapping, sub)
            index, subindex, bits = raw >> 16, (raw >> 8) & 255, raw & 255
            assert index in entries and 0 < bits <= 64
            fields.append(dict(index=f'0x{index:04X}', subindex=subindex, bits=bits,
                               mapping_value=f'0x{raw:08X}'))
        assert sum(field['bits'] for field in fields) <= 64
        pdos.append(dict(direction=direction, communication_index=f'0x{comm:04X}',
                         mapping_index=f'0x{mapping:04X}',
                         default_cob_id=f'0x{value(comm, 1):08X}',
                         transmission_type=value(comm, 2), fields=fields,
                         event_timer_ms=value(comm, 5) if direction == 'TX' else None))

    return dict(source=str(path.relative_to(ROOT)), sha256=hashlib.sha256(data).hexdigest(),
                method='Static C28x word-addressed tables; CO_OD_entry_t / CO_OD_entryRecord_t compatible layouts.',
                layout_reference='https://github.com/CANopenNode/CANopenNode/blob/v1.3/stack/CO_SDO.h',
                hardware_access=False, dictionary_file_offset=start,
                flash_word_base=flash_base, candidate_A000_initialization_file_offset=init_offset,
                object_count=len(entries), objects=list(entries.values()), pdos=pdos,
                limitations=[
                    'Downloaded firmware identity has not been matched to the connected inverter.',
                    'Initialization values can be replaced by saved settings or startup code.',
                    'Base COB-IDs do not by themselves prove runtime node-ID addition.',
                    'Indices and widths do not establish signedness, engineering units, or control semantics.',
                    'No SDO implementation or command handler has been executed or fully traced.'
                ])


def main():
    results = [analyze(path) for path in sorted(REFERENCE.rglob('SinTechController*.bin'))]
    assert len(results) == 2
    assert results[0]['pdos'] == results[1]['pdos']
    destination = REFERENCE / 'static_analysis/firmware_objects.json'
    destination.write_text(json.dumps(results, indent=2), encoding='utf-8')
    for result in results:
        print(result['source'], 'objects:', result['object_count'])
        for pdo in result['pdos']:
            print(pdo['direction'], pdo['default_cob_id'], pdo['fields'])
    print(destination)


if __name__ == '__main__':
    main()
