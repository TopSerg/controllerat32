"""Capture raw Classic CAN traffic from PCAN-USB.

Default is listen-only. --allow-ack permits hardware CAN acknowledgements.
No application CAN write, remote request, reset, or configuration message is issued.
"""

import argparse
import csv
from datetime import datetime, timezone
from pathlib import Path
import sys
import time

sys.path.insert(0, r"C:\Project\nvert_loader\pc_to_can_loader")
from PCANBasic import (  # noqa: E402
    PCANBasic,
    PCAN_BAUD_1M,
    PCAN_BAUD_250K,
    PCAN_BAUD_500K,
    PCAN_ERROR_OK,
    PCAN_ERROR_QRCVEMPTY,
    PCAN_LISTEN_ONLY,
    PCAN_MESSAGE_ERRFRAME,
    PCAN_MESSAGE_EXTENDED,
    PCAN_MESSAGE_RTR,
    PCAN_PARAMETER_OFF,
    PCAN_PARAMETER_ON,
    PCAN_USBBUS1,
)

RATES = {250000: PCAN_BAUD_250K, 500000: PCAN_BAUD_500K, 1000000: PCAN_BAUD_1M}


def capture(rate: int, seconds: float, path: Path, allow_ack: bool) -> None:
    pcan = PCANBasic()
    channel = PCAN_USBBUS1
    expected = PCAN_PARAMETER_OFF if allow_ack else PCAN_PARAMETER_ON
    mode = "ack" if allow_ack else "listen-only"
    set_status = pcan.SetValue(channel, PCAN_LISTEN_ONLY, expected)
    if set_status != PCAN_ERROR_OK:
        raise RuntimeError(f"Cannot set {mode} before initialization: 0x{int(set_status):X}")
    pre_status, pre_value = pcan.GetValue(channel, PCAN_LISTEN_ONLY)
    if pre_status != PCAN_ERROR_OK or pre_value != expected:
        raise RuntimeError(f"Cannot verify pre-initialization {mode}: 0x{int(pre_status):X}, {pre_value}")

    init_status = pcan.Initialize(channel, RATES[rate])
    if init_status != PCAN_ERROR_OK:
        raise RuntimeError(f"Initialize {rate} failed: 0x{int(init_status):X}")

    try:
        status, value = pcan.GetValue(channel, PCAN_LISTEN_ONLY)
        if status != PCAN_ERROR_OK or value != expected:
            raise RuntimeError(f"{mode} verification failed: 0x{int(status):X}, {value}")
        print(f"{rate}: verified {mode}; capturing {seconds:g} s", flush=True)
        path.parent.mkdir(parents=True, exist_ok=True)
        count = 0
        ids = set()
        errors = set()
        with path.open("w", newline="", encoding="utf-8") as output:
            writer = csv.writer(output)
            writer.writerow(("utc_time", "bitrate", "mode", "can_id", "extended", "rtr", "error_frame", "dlc", "data_hex", "pcan_us"))
            deadline = time.monotonic() + seconds
            while time.monotonic() < deadline:
                result, msg, timestamp = pcan.Read(channel)
                if result == PCAN_ERROR_QRCVEMPTY:
                    time.sleep(0.002)
                    continue
                if result != PCAN_ERROR_OK:
                    errors.add(int(result))
                    time.sleep(0.01)
                    continue
                is_extended = bool(msg.MSGTYPE & PCAN_MESSAGE_EXTENDED.value)
                is_rtr = bool(msg.MSGTYPE & PCAN_MESSAGE_RTR.value)
                is_error = bool(msg.MSGTYPE & PCAN_MESSAGE_ERRFRAME.value)
                frame_id = f"0x{msg.ID:08X}" if is_extended else f"0x{msg.ID:03X}"
                data = bytes(msg.DATA[:msg.LEN]) if not is_rtr else b""
                device_us = (timestamp.millis_overflow * 0x100000000 + timestamp.millis) * 1000 + timestamp.micros
                writer.writerow((datetime.now(timezone.utc).isoformat(), rate, mode, frame_id,
                                 int(is_extended), int(is_rtr), int(is_error), msg.LEN,
                                 data.hex(" ").upper(), device_us))
                count += 1
                ids.add(frame_id)
        print(f"{rate}: frames={count}, ids={sorted(ids)}, read_statuses={[hex(x) for x in sorted(errors)]}, file={path}", flush=True)
        print(f"{rate}: final bus status=0x{int(pcan.GetStatus(channel)):X}", flush=True)
    finally:
        pcan.Uninitialize(channel)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rates", nargs="+", type=int, choices=RATES, default=list(RATES))
    parser.add_argument("--seconds", type=float, default=8.0)
    parser.add_argument("--allow-ack", action="store_true", help="Enable normal CAN mode for hardware ACK; no application frames are sent")
    parser.add_argument("--output-dir", type=Path)
    args = parser.parse_args()
    if args.seconds <= 0:
        parser.error("--seconds must be positive")
    output_dir = args.output_dir or Path("test_logs/sintech_ack" if args.allow_ack else "test_logs/sintech_passive")
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    failed = False
    for rate in args.rates:
        try:
            capture(rate, args.seconds, output_dir / f"{stamp}_{rate}.csv", args.allow_ack)
        except Exception as exc:
            print(f"{rate}: {exc}", file=sys.stderr, flush=True)
            failed = True
    if failed:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
