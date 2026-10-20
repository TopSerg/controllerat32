"""Record SINtech CAN (hardware ACK) and Gen1.1 RAM through running SWD.

No application CAN writes and no target reset, halt, resume, or RAM writes.
Addresses below were resolved from build/debug/AT32_Base.elf; executable
flash and the initialized data image are checked against AT32_Base.bin.
"""
import argparse
import csv
import ctypes as ct
from datetime import datetime, timezone
import hashlib
import json
import math
from pathlib import Path
import struct
import sys
import time

sys.path.insert(0, str(Path(__file__).resolve().parent))
from capture_sintech_passive import (PCANBasic, PCAN_USBBUS1, PCAN_LISTEN_ONLY,
    PCAN_PARAMETER_OFF, PCAN_ERROR_OK, PCAN_ERROR_QRCVEMPTY, PCAN_BAUD_1M)

ROOT = Path(__file__).resolve().parents[1]


class RunningTarget:
    def __init__(self):
        self.dll = ct.CDLL(r"C:\Program Files\SEGGER\JLink_V824\JLink_x64.dll")
        signatures = {
            'JLINKARM_EMU_SelectByUSBSN': (ct.c_int, [ct.c_uint]),
            'JLINKARM_Open': (ct.c_char_p, []),
            'JLINKARM_Close': (None, []),
            'JLINKARM_TIF_Select': (ct.c_int, [ct.c_int]),
            'JLINKARM_SetSpeed': (None, [ct.c_uint]),
            'JLINKARM_ExecCommand': (ct.c_int, [ct.c_char_p, ct.c_char_p, ct.c_int]),
            'JLINKARM_Connect': (ct.c_int, []),
            'JLINKARM_IsHalted': (ct.c_int, []),
            'JLINKARM_ReadMemEx': (ct.c_int, [ct.c_uint, ct.c_uint, ct.c_void_p, ct.c_uint]),
        }
        for name, (restype, argtypes) in signatures.items():
            fn = getattr(self.dll, name)
            fn.restype, fn.argtypes = restype, argtypes
        self.opened = False

    def read(self, address, length):
        buffer = (ct.c_ubyte * length)()
        count = self.dll.JLINKARM_ReadMemEx(address, length, buffer, 4)
        if count != length:
            raise RuntimeError(f'SWD read {address:#x}: {count}/{length} bytes')
        return bytes(buffer)

    def open(self):
        if self.dll.JLINKARM_EMU_SelectByUSBSN(601024422) < 0:
            raise RuntimeError('J-Link probe not available')
        error = self.dll.JLINKARM_Open()
        if error:
            raise RuntimeError(error.decode(errors='replace'))
        self.opened = True
        try:
            if self.dll.JLINKARM_TIF_Select(1) != 0:
                raise RuntimeError('Cannot select SWD')
            self.dll.JLINKARM_SetSpeed(1000)
            for command in (b'SetBatchMode = 1', b'Device = Cortex-M4'):
                error_buffer = ct.create_string_buffer(512)
                result = self.dll.JLINKARM_ExecCommand(command, error_buffer, len(error_buffer))
                if result < 0 or error_buffer.value:
                    raise RuntimeError(f'{command!r}: {error_buffer.value!r}, {result}')
            if self.dll.JLINKARM_Connect() < 0:
                raise RuntimeError('Cannot connect to Cortex-M4')
            self.require_running()
            firmware = (ROOT / 'build/debug/AT32_Base.bin').read_bytes()
            for offset, length in ((0, 0x167B0), (0x167B0, 0x55C), (0x20000, 0x27D8)):
                if self.read(0x08004000 + offset, length) != firmware[offset:offset+length]:
                    raise RuntimeError('Target firmware differs from ELF companion binary; refusing RAM interpretation')
            self.require_running()
            return hashlib.sha256(firmware).hexdigest()
        except BaseException:
            self.close()
            raise

    def require_running(self):
        if self.dll.JLINKARM_IsHalted() != 0:
            raise RuntimeError('Target is halted or unavailable; capture stopped without changing CPU state')

    def sample(self):
        start = time.monotonic_ns()
        data = self.read(0x20000D98, 0xDC)
        tick = struct.unpack('<I', self.read(0x200097E0, 4))[0]
        end = time.monotonic_ns()
        floats = {name: struct.unpack_from('<f', data, address - 0x20000D98)[0]
                  for name, address in [('udc_v', 0x20000D98), ('w_rad_s', 0x20000DDC),
                                        ('resolver_sin', 0x20000E6C), ('resolver_cos', 0x20000E70)]}
        if not all(math.isfinite(value) for value in floats.values()):
            raise RuntimeError('Non-finite Gen1.1 sample')
        return {'utc_time': datetime.now(timezone.utc).isoformat(), 'host_ns': (start + end)//2,
                'read_ms': (end-start)/1e6, 'target_tick_ms': tick,
                **floats, 'rpm': floats['w_rad_s'] * 30 / math.pi}

    def close(self):
        if self.opened:
            self.dll.JLINKARM_Close()
            self.opened = False


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--seconds', type=float, default=40)
    parser.add_argument('--label', default='pedal')
    args = parser.parse_args()
    if args.seconds <= 0:
        parser.error('--seconds must be positive')
    out = ROOT / 'test_logs' / f'sintech_gen11_{datetime.now():%Y%m%d_%H%M%S}_{args.label}'
    out.mkdir(parents=True, exist_ok=False)
    target = RunningTarget()
    pcan = PCANBasic()
    initialized = False
    samples = []
    counts = {}
    summary = {'duration_requested_s': args.seconds, 'path': str(out), 'application_can_writes': 0,
               'target_control_commands': 0, 'jlink_interface': 'SWD', 'can_bitrate': 1000000}
    try:
        summary['firmware_binary_sha256'] = target.open()
        for result in (pcan.SetValue(PCAN_USBBUS1, PCAN_LISTEN_ONLY, PCAN_PARAMETER_OFF),):
            if result != PCAN_ERROR_OK:
                raise RuntimeError(f'Cannot configure ACK: {result}')
        result = pcan.Initialize(PCAN_USBBUS1, PCAN_BAUD_1M)
        if result != PCAN_ERROR_OK:
            raise RuntimeError(f'Cannot initialize PCAN: {result}')
        initialized = True
        result, mode = pcan.GetValue(PCAN_USBBUS1, PCAN_LISTEN_ONLY)
        if result != PCAN_ERROR_OK or mode != PCAN_PARAMETER_OFF:
            raise RuntimeError('ACK mode verification failed')
        target.require_running()
        print(f'READY: simultaneous capture {args.seconds:g}s; Gen1.1 running; CAN hardware ACK enabled; {out}', flush=True)
        with (out/'sintech.csv').open('w', newline='', encoding='utf-8') as cf, (out/'gen11.csv').open('w', newline='', encoding='utf-8') as gf:
            cw = csv.writer(cf)
            cw.writerow(['utc_time','host_ns','pcan_us','can_id','msgtype','dlc','data_hex'])
            gw = None
            start = time.monotonic()
            next_sample = start
            next_report = start + 5
            while time.monotonic() - start < args.seconds:
                for _ in range(200):
                    result, msg, stamp = pcan.Read(PCAN_USBBUS1)
                    if result == PCAN_ERROR_QRCVEMPTY:
                        break
                    if result != PCAN_ERROR_OK:
                        raise RuntimeError(f'CAN read error {result:#x}')
                    host_ns = time.monotonic_ns()
                    pcan_us = (stamp.millis_overflow * 0x100000000 + stamp.millis)*1000 + stamp.micros
                    frame_id = f'0x{msg.ID:08X}' if msg.MSGTYPE & 2 else f'0x{msg.ID:03X}'
                    counts[frame_id] = counts.get(frame_id, 0) + 1
                    cw.writerow([datetime.now(timezone.utc).isoformat(),host_ns,pcan_us,frame_id,
                                 msg.MSGTYPE,msg.LEN,bytes(msg.DATA[:msg.LEN]).hex(' ').upper()])
                now = time.monotonic()
                if now >= next_sample:
                    row = target.sample()
                    samples.append(row)
                    if gw is None:
                        gw = csv.DictWriter(gf, fieldnames=list(row))
                        gw.writeheader()
                    gw.writerow(row)
                    next_sample = max(next_sample + .02, time.monotonic())
                if now >= next_report:
                    target.require_running()
                    cf.flush()
                    gf.flush()
                    print(f'T={now-start:.1f}s samples={len(samples)} rpm={samples[-1]["rpm"]:.1f} CAN={sum(counts.values())}', flush=True)
                    next_report += 5
                time.sleep(.001)
        target.require_running()
        summary.update(status='complete', can_counts=counts, gen11_samples=len(samples),
                       can_final_status=int(pcan.GetStatus(PCAN_USBBUS1)),
                       rpm_range=[min(x['rpm'] for x in samples), max(x['rpm'] for x in samples)],
                       resolver_sin_range=[min(x['resolver_sin'] for x in samples),max(x['resolver_sin'] for x in samples)],
                       resolver_cos_range=[min(x['resolver_cos'] for x in samples),max(x['resolver_cos'] for x in samples)])
        print(json.dumps(summary, ensure_ascii=False), flush=True)
    except BaseException as exc:
        summary.update(status='failed', error=str(exc), can_counts=counts, gen11_samples=len(samples))
        raise
    finally:
        if initialized:
            pcan.Uninitialize(PCAN_USBBUS1)
        target.close()
        (out/'summary.json').write_text(json.dumps(summary, indent=2, ensure_ascii=False), encoding='utf-8')


if __name__ == '__main__':
    main()
