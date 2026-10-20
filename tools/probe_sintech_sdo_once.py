"""One fixed SDO upload request, after a SINtech telemetry preflight.

Default prints the plan without opening hardware. --send-read authorizes one
PCAN Write call: standard 0x630, DLC 8, 40 00 10 00 00 00 00 00.
No retries, NMT, RPDO, configuration writes, resets, or node scanning.
"""
import argparse
import csv
from datetime import datetime, timezone
import json
from pathlib import Path
import time

from capture_sintech_passive import (
    PCANBasic, PCAN_USBBUS1, PCAN_LISTEN_ONLY, PCAN_PARAMETER_OFF,
    PCAN_ERROR_OK, PCAN_ERROR_QRCVEMPTY, PCAN_BAUD_1M,
    PCAN_MESSAGE_EXTENDED, PCAN_MESSAGE_RTR, PCAN_MESSAGE_ERRFRAME,
)
from PCANBasic import TPCANMsg, PCAN_MESSAGE_STANDARD

ROOT = Path(__file__).resolve().parents[1]
REQUEST = bytes.fromhex('40 00 10 00 00 00 00 00')
REQUIRED_IDS = {0x1B0, 0x2B0, 0x3B0, 0x4B0}


def now():
    return datetime.now(timezone.utc).isoformat()


def probe():
    destination = ROOT / 'test_logs' / ('sintech_sdo_' + datetime.now().strftime('%Y%m%d_%H%M%S'))
    destination.mkdir(parents=True, exist_ok=False)
    result = dict(start_utc=now(), bitrate=1000000, assumed_node_id='0x30',
                  request_id='0x630', request_data=REQUEST.hex(' ').upper(),
                  application_write_calls=0, reply=None, receive_statuses=[],
                  output_dir=str(destination))
    api = PCANBasic()
    channel = PCAN_USBBUS1
    opened = False
    seen = set()
    rx_counts = {}
    t0 = time.monotonic()
    try:
        status = api.SetValue(channel, PCAN_LISTEN_ONLY, PCAN_PARAMETER_OFF)
        if status != PCAN_ERROR_OK:
            raise RuntimeError(f'Cannot enable ACK mode: {int(status):#x}')
        status = api.Initialize(channel, PCAN_BAUD_1M)
        if status != PCAN_ERROR_OK:
            raise RuntimeError(f'PCAN initialize failed: {int(status):#x}')
        opened = True
        status, mode = api.GetValue(channel, PCAN_LISTEN_ONLY)
        if status != PCAN_ERROR_OK or mode != PCAN_PARAMETER_OFF:
            raise RuntimeError('Cannot verify normal CAN mode')
        with (destination / 'traffic.csv').open('w', newline='', encoding='utf-8') as stream:
            writer = csv.writer(stream)
            writer.writerow(['utc_time', 'host_seconds', 'phase', 'direction', 'can_id',
                             'extended', 'rtr', 'error_frame', 'dlc', 'data_hex', 'pcan_us'])

            def receive_for(seconds, phase):
                end = time.monotonic() + seconds
                while time.monotonic() < end:
                    code, msg, timestamp = api.Read(channel)
                    if code == PCAN_ERROR_QRCVEMPTY:
                        time.sleep(0.001)
                        continue
                    if code != PCAN_ERROR_OK:
                        result['receive_statuses'].append(hex(int(code)))
                        raise RuntimeError(f'PCAN receive status: {int(code):#x}')
                    extended = bool(msg.MSGTYPE & PCAN_MESSAGE_EXTENDED.value)
                    rtr = bool(msg.MSGTYPE & PCAN_MESSAGE_RTR.value)
                    error = bool(msg.MSGTYPE & PCAN_MESSAGE_ERRFRAME.value)
                    raw = bytes(msg.DATA[:msg.LEN]) if not rtr else b''
                    ident = f'0x{msg.ID:08X}' if extended else f'0x{msg.ID:03X}'
                    micros = (timestamp.millis_overflow * 0x100000000 + timestamp.millis) * 1000 + timestamp.micros
                    row = [now(), time.monotonic()-t0, phase, 'RX', ident,
                           int(extended), int(rtr), int(error), msg.LEN, raw.hex(' ').upper(), micros]
                    writer.writerow(row)
                    rx_counts[ident] = rx_counts.get(ident, 0) + 1
                    if not extended and not rtr and not error and msg.LEN == 8:
                        seen.add(msg.ID)
                        if phase == 'before' and msg.ID == 0x5B0:
                            raise RuntimeError('Unexpected SDO response during preflight; no request sent')
                        if (phase == 'after' and msg.ID == 0x5B0 and raw[1:4] == REQUEST[1:4]
                                and (raw[0] == 0x80 or raw[0] & 0xE0 == 0x40)):
                            result['reply'] = dict(utc_time=row[0], data_hex=row[9],
                                                 command=hex(raw[0]), abort=raw[0] == 0x80)
                            if raw[0] == 0x80:
                                result['reply']['abort_code'] = f'0x{int.from_bytes(raw[4:8], "little"):08X}'
                            elif raw[0] & 3 == 3:
                                length = 4 - ((raw[0] >> 2) & 3)
                                value = raw[4:4+length]
                                result['reply'].update(length=length, value_hex=value.hex(' ').upper(),
                                                       value_unsigned=int.from_bytes(value, 'little'))

            print('PCAN 1 Mbps, ACK enabled. Recording 2 s preflight.', flush=True)
            receive_for(2, 'before')
            result['preflight_ids'] = sorted(hex(ident) for ident in seen)
            if not REQUIRED_IDS <= seen:
                raise RuntimeError(f'Expected SINtech telemetry missing: {sorted(hex(i) for i in REQUIRED_IDS-seen)}')
            status = api.GetStatus(channel)
            result['preflight_bus_status'] = hex(int(status))
            if status != PCAN_ERROR_OK:
                raise RuntimeError(f'Bus not healthy before request: {int(status):#x}')
            msg = TPCANMsg()
            msg.ID, msg.MSGTYPE, msg.LEN = 0x630, PCAN_MESSAGE_STANDARD.value, 8
            for index, byte in enumerate(REQUEST):
                msg.DATA[index] = byte
            stream.flush()
            result['application_write_calls'] = 1
            result['write_attempt_utc'] = now()
            (destination / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
            code = api.Write(channel, msg)  # The only application CAN transmission call.
            result['write_status'] = hex(int(code))
            if code != PCAN_ERROR_OK:
                raise RuntimeError(f'Single Write failed, no retry: {int(code):#x}')
            writer.writerow([result['write_attempt_utc'], time.monotonic()-t0, 'request',
                             'TX_QUEUED', '0x630', 0, 0, 0, 8, REQUEST.hex(' ').upper(), ''])
            print('Queued exactly one SDO upload request. Recording 3 s response window.', flush=True)
            receive_for(3, 'after')
            result['final_bus_status'] = hex(int(api.GetStatus(channel)))
            result['outcome'] = 'reply_received' if result['reply'] else 'no_matching_reply'
    except Exception as exc:
        result['outcome'] = 'error'
        result['error'] = str(exc)
    finally:
        if opened:
            result['uninitialize_status'] = hex(int(api.Uninitialize(channel)))
        result['rx_counts'] = rx_counts
        result['end_utc'] = now()
        (destination / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    print(json.dumps(result, indent=2), flush=True)
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--send-read', action='store_true')
    options = parser.parse_args()
    if options.send_read:
        report = probe()
        raise SystemExit(1 if report['outcome'] == 'error' else 0)
    print('DRY RUN: standard 0x630 DLC 8 DATA ' + REQUEST.hex(' ').upper())
    print('No hardware opened. Use --send-read for the single fixed request.')
