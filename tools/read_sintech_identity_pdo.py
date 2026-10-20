"""Read a fixed, reviewed set of SINtech SDO identity/RPDO objects.

Default is dry run. --read opens Classic CAN at 1 Mbps and sends only SDO
upload-initiate requests (0x40), sequentially on 0x630. No retries or writes.
Every requested index/subindex is checked against both downloaded OD tables.
"""
import argparse
import csv
from datetime import datetime, timezone
import json
from pathlib import Path
import time

from probe_sintech_sdo_once import (
    PCANBasic, PCAN_USBBUS1, PCAN_LISTEN_ONLY, PCAN_PARAMETER_OFF,
    PCAN_ERROR_OK, PCAN_ERROR_QRCVEMPTY, PCAN_BAUD_1M,
    PCAN_MESSAGE_EXTENDED, PCAN_MESSAGE_RTR, PCAN_MESSAGE_ERRFRAME,
    TPCANMsg, PCAN_MESSAGE_STANDARD, REQUIRED_IDS, ROOT,
)

ALLOWED = {(0x100A, 0)} | {(0x1018, sub) for sub in range(5)} | {
    (0x1400, sub) for sub in range(3)} | {(0x1600, sub) for sub in range(9)}
CONTROL_SETTINGS = [(0x6411, 8), (0x6410, 5), (0x6410, 6)]
ALLOWED |= set(CONTROL_SETTINGS)


def request_data(index, subindex):
    if (index, subindex) not in ALLOWED:
        raise ValueError('Object is not in the reviewed read whitelist')
    return bytes([0x40, index & 255, index >> 8, subindex, 0, 0, 0, 0])


def check_dictionary():
    tables = json.loads((ROOT / 'test_logs/sintech_reference_20261020/static_analysis/firmware_objects.json').read_text())
    for table in tables:
        objects = {int(obj['index'], 16): obj for obj in table['objects']}
        for index, sub in ALLOWED:
            entry = objects[index]
            if sub > entry['max_subindex']:
                raise ValueError('Subindex missing from downloaded OD')
            record = entry if entry['attribute'] else entry['records'][sub]
            if not record['attribute'] & 4:
                raise ValueError('Object not readable in downloaded OD')


def utc():
    return datetime.now(timezone.utc).isoformat()


def capture(control_settings_only=False):
    check_dictionary()
    directory = ROOT / 'test_logs' / ('sintech_identity_pdo_' + datetime.now().strftime('%Y%m%d_%H%M%S'))
    directory.mkdir(parents=True, exist_ok=False)
    result = dict(start_utc=utc(), bitrate=1000000, request_id='0x630', response_id='0x5B0',
                  application_write_calls=0, transactions=[], rx_counts={}, output_dir=str(directory))
    result['scope'] = 'control_settings_only' if control_settings_only else 'identity_and_rpdo'
    api, channel = PCANBasic(), PCAN_USBBUS1
    opened = False
    t0 = time.monotonic()
    seen = set()

    def save():
        (directory / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')

    try:
        code = api.SetValue(channel, PCAN_LISTEN_ONLY, PCAN_PARAMETER_OFF)
        if code != PCAN_ERROR_OK:
            raise RuntimeError(f'Cannot enable ACK: {int(code):#x}')
        code = api.Initialize(channel, PCAN_BAUD_1M)
        if code != PCAN_ERROR_OK:
            raise RuntimeError(f'Initialize failed: {int(code):#x}')
        opened = True
        code, mode = api.GetValue(channel, PCAN_LISTEN_ONLY)
        if code != PCAN_ERROR_OK or mode != PCAN_PARAMETER_OFF:
            raise RuntimeError('Normal CAN mode not verified')
        with (directory / 'traffic.csv').open('w', newline='', encoding='utf-8') as stream:
            writer = csv.writer(stream)
            writer.writerow(['utc_time', 'host_seconds', 'phase', 'direction', 'can_id',
                             'extended', 'rtr', 'error_frame', 'dlc', 'data_hex', 'pcan_us'])

            def receive(seconds, phase, expected=None):
                deadline = time.monotonic() + seconds
                while time.monotonic() < deadline:
                    status, msg, timestamp = api.Read(channel)
                    if status == PCAN_ERROR_QRCVEMPTY:
                        time.sleep(0.001)
                        continue
                    if status != PCAN_ERROR_OK:
                        raise RuntimeError(f'CAN receive status {int(status):#x}')
                    extended = bool(msg.MSGTYPE & PCAN_MESSAGE_EXTENDED.value)
                    rtr = bool(msg.MSGTYPE & PCAN_MESSAGE_RTR.value)
                    error = bool(msg.MSGTYPE & PCAN_MESSAGE_ERRFRAME.value)
                    raw = bytes(msg.DATA[:msg.LEN]) if not rtr else b''
                    ident = f'0x{msg.ID:08X}' if extended else f'0x{msg.ID:03X}'
                    micros = (timestamp.millis_overflow * 0x100000000 + timestamp.millis) * 1000 + timestamp.micros
                    writer.writerow([utc(), time.monotonic()-t0, phase, 'RX', ident,
                                     int(extended), int(rtr), int(error), msg.LEN, raw.hex(' ').upper(), micros])
                    result['rx_counts'][ident] = result['rx_counts'].get(ident, 0) + 1
                    if error:
                        raise RuntimeError('CAN error frame observed')
                    if extended or rtr:
                        continue
                    if msg.LEN == 8:
                        seen.add(msg.ID)
                    if msg.ID == 0x5B0:
                        if expected is None:
                            raise RuntimeError('Unsolicited SDO reply; stopping before next request')
                        if msg.LEN != 8 or raw[1:4] != expected:
                            raise RuntimeError('SDO reply object/DLC mismatch')
                        return raw
                if expected is not None:
                    raise TimeoutError('No matching SDO response within 1 second; no retry')

            def read(index, subindex):
                raw = request_data(index, subindex)
                receive(0.05, 'between')
                status = api.GetStatus(channel)
                if status != PCAN_ERROR_OK:
                    raise RuntimeError(f'CAN bus not healthy: {int(status):#x}')
                transaction = dict(index=f'0x{index:04X}', subindex=subindex,
                                   request_data=raw.hex(' ').upper(), attempt_utc=utc())
                result['transactions'].append(transaction)
                result['application_write_calls'] += 1
                save()
                stream.flush()
                message = TPCANMsg()
                message.ID, message.MSGTYPE, message.LEN = 0x630, PCAN_MESSAGE_STANDARD.value, 8
                for n, byte in enumerate(raw):
                    message.DATA[n] = byte
                status = api.Write(channel, message)
                transaction['write_status'] = hex(int(status))
                if status != PCAN_ERROR_OK:
                    raise RuntimeError(f'SDO Write failed: {int(status):#x}; no retry')
                writer.writerow([utc(), time.monotonic()-t0, 'request', 'TX_QUEUED',
                                 '0x630', 0, 0, 0, 8, raw.hex(' ').upper(), ''])
                reply = receive(1, 'response', raw[1:4])
                transaction['reply_data'] = reply.hex(' ').upper()
                if reply[0] == 0x80:
                    transaction['abort_code'] = f'0x{int.from_bytes(reply[4:8], "little"):08X}'
                    save()
                    raise RuntimeError(f'SDO abort at {index:#x}:{subindex}: {transaction["abort_code"]}')
                if reply[0] not in (0x43, 0x47, 0x4B, 0x4F):
                    save()
                    raise RuntimeError(f'Unexpected or segmented response {reply[0]:#x}; stopping')
                length = 4 - ((reply[0] >> 2) & 3)
                value = reply[4:4+length]
                transaction.update(length=length, value_hex=value.hex(' ').upper(),
                                   value_unsigned=int.from_bytes(value, 'little'))
                save()
                print(f'{index:04X}:{subindex:02X} = {transaction["value_hex"]} '
                      f'(u{8*length}={transaction["value_unsigned"]})', flush=True)
                return transaction['value_unsigned']

            print('Recording SINtech telemetry preflight (2 s).', flush=True)
            receive(2, 'before')
            result['preflight_ids'] = sorted(hex(ident) for ident in seen)
            if not REQUIRED_IDS <= seen:
                raise RuntimeError('Required SINtech telemetry not found; no request sent')
            if control_settings_only:
                for index, sub in CONTROL_SETTINGS:
                    read(index, sub)
            else:
                read(0x100A, 0)
                identity_subs = read(0x1018, 0)
                if not 0 <= identity_subs <= 4:
                    raise RuntimeError('Unexpected identity object subindex count')
                for sub in range(1, identity_subs+1):
                    read(0x1018, sub)
                communication_subs = read(0x1400, 0)
                if communication_subs != 2:
                    raise RuntimeError('Unexpected RPDO communication record size')
                for sub in (1, 2):
                    read(0x1400, sub)
                mapping_count = read(0x1600, 0)
                if not 0 <= mapping_count <= 8:
                    raise RuntimeError('Unexpected RPDO mapping count')
                # Four descriptor slots exist in both supplied images even if the
                # read callback reports zero currently active mappings.
                for sub in range(1, max(4, mapping_count)+1):
                    read(0x1600, sub)
            receive(1, 'after')
            result['final_bus_status'] = hex(int(api.GetStatus(channel)))
            result['outcome'] = 'completed'
    except Exception as exc:
        result['outcome'] = 'error'
        result['error'] = str(exc)
    finally:
        if opened:
            result['uninitialize_status'] = hex(int(api.Uninitialize(channel)))
        result['end_utc'] = utc()
        save()
    print('Outcome:', result['outcome'], flush=True)
    if 'error' in result:
        print(result['error'], flush=True)
    print(directory, flush=True)
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--read', action='store_true')
    parser.add_argument('--control-settings-only', action='store_true',
                        help='Read only source selector, pole pairs and electrical speed limit')
    args = parser.parse_args()
    check_dictionary()
    if args.read:
        report = capture(args.control_settings_only)
        raise SystemExit(0 if report['outcome'] == 'completed' else 1)
    print('DRY RUN: reviewed read-only objects, 0x630 -> 0x5B0, 1 Mbps. No hardware opened.')
    for index, sub in (CONTROL_SETTINGS if args.control_settings_only else sorted(ALLOWED)):
        print(f'{index:04X}:{sub:02X}: {request_data(index, sub).hex(" ").upper()}')
