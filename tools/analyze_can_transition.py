"""Decode StandRumoteUI WS_CAN_TRACE_FILE around a PWMrun -> FreeWheel event.

Usage: py -3 tools/analyze_can_transition.py C:\\path\\to\\can_trace.csv
"""

import argparse
import csv
from datetime import datetime
from pathlib import Path


def signal(data: bytes, start_bit: int, length: int) -> int:
    """Read the Motorola bit numbering used by CANDBCUtils.c."""
    value = 0
    bit = start_bit
    for _ in range(length):
        value = (value << 1) | ((data[bit // 8] >> (bit % 8)) & 1)
        bit = bit + 15 if bit % 8 == 0 else bit - 1
    return value


def decode(frame):
    data = frame["data"]
    if len(data) < 8:
        return "short frame"
    if frame["id"] == 0x2C6:
        level = signal(data, 7, 3)
        sw = signal(data, 15, 19)
        hw = signal(data, 39, 16)
        sensor = signal(data, 55, 8)
        can = signal(data, 63, 8)
        names = []
        for bit, name in ((1, "OverCurrent"), (2, "OverVoltage"),
                          (7, "MotorOverTemp"), (9, "PowerOverTemp")):
            if sw & (1 << bit):
                names.append(name)
        if hw & 1:
            names.append("HardwareFault")
        if hw & (1 << 5):
            names.append("LowVref")
        if sensor & 2:
            names.append("CurrentOffset")
        if can & 1:
            names.append("0x300Timeout")
        if can & 2:
            names.append("0x300Counter")
        return (f"level={level} sw=0x{sw:05X} hw=0x{hw:04X} "
                f"sensor=0x{sensor:02X} can=0x{can:02X} "
                f"[{', '.join(names) if names else 'no mapped faults'}]")
    if frame["id"] == 0x300:
        id_a = signal(data, 7, 16) * 0.1 - 3200
        iq_a = signal(data, 23, 16) * 0.1 - 3200
        enable = signal(data, 39, 1)
        counter = signal(data, 51, 4)
        checksum = signal(data, 63, 8)
        return (f"Id={id_a:.1f}A Iq={iq_a:.1f}A enable={enable} "
                f"counter={counter} checksum=0x{checksum:02X}")
    if frame["id"] == 0x07D:
        gate = signal(data, 34, 2)
        id_a = signal(data, 11, 12) * 0.5 - 1023.5
        iq_a = signal(data, 31, 12) * 0.5 - 1023.5
        return f"GateDrv={gate} Id={id_a:.1f}A Iq={iq_a:.1f}A"
    if frame["id"] == 0x07A:
        speed_rad_s = signal(data, 39, 16) - 32768
        speed_rpm = speed_rad_s * 60 / (2 * 3.141592653589793)
        return f"speed={speed_rad_s}rad/s ({speed_rpm:.0f}rpm)"
    if frame["id"] == 0x046:
        state = signal(data, 47, 4)
        gear = signal(data, 22, 3)
        kl15 = signal(data, 8, 1)
        return f"requested_state={state} gear={gear} KL15={kl15}"
    return ""


def read_frames(path: Path):
    with path.open(newline="", encoding="utf-8-sig") as file:
        for row in csv.DictReader(file, delimiter=";"):
            try:
                data = bytes.fromhex(row["data"])
                frame = {
                    "epoch_us": int(row["epoch_us"]),
                    "pcan_us": int(row["pcan_us"]),
                    "direction": row["direction"],
                    "id": int(row["id"], 0),
                    "dlc": int(row["dlc"]),
                    "data": data,
                    "status": int(row["status"], 0),
                }
                if len(data) != frame["dlc"]:
                    print(f"Skipped row with DLC mismatch: {row}")
                    continue
                yield frame
            except (KeyError, TypeError, ValueError) as exc:
                print(f"Skipped malformed row: {exc}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trace", type=Path)
    parser.add_argument("--before-ms", type=int, default=300)
    parser.add_argument("--after-ms", type=int, default=300)
    args = parser.parse_args()
    frames = list(read_frames(args.trace))
    if not frames:
        parser.error("trace contains no valid frames")

    transitions = []
    last_gate = None
    for frame in frames:
        if frame["direction"] == "RX" and frame["id"] == 0x07D and len(frame["data"]) == 8:
            gate = signal(frame["data"], 34, 2)
            if last_gate == 2 and gate == 1:
                transitions.append(frame)
            last_gate = gate

    print(f"Frames: {len(frames)}, PWMrun -> FreeWheel transitions: {len(transitions)}")
    for event in transitions:
        t0 = event["epoch_us"]
        local = datetime.fromtimestamp(t0 / 1_000_000).isoformat(timespec="milliseconds")
        print(f"\nTransition at {local}, PCAN timestamp {event['pcan_us']} us")
        window = [f for f in frames
                  if t0 - args.before_ms * 1000 <= f["epoch_us"] <=
                  t0 + args.after_ms * 1000]
        last_tx = None
        last_counter = None
        for frame in window:
            if frame["id"] not in (0x046, 0x07A, 0x07D, 0x2C6, 0x300):
                continue
            details = decode(frame)
            if frame["id"] == 0x300 and frame["direction"] == "TX" and len(frame["data"]) == 8:
                counter = signal(frame["data"], 51, 4)
                if last_tx is not None:
                    gap_ms = (frame["epoch_us"] - last_tx) / 1000
                    details += f" gap={gap_ms:.1f}ms"
                    if last_counter is not None and counter != (last_counter + 1) % 16:
                        details += " COUNTER_JUMP"
                last_tx = frame["epoch_us"]
                last_counter = counter
            offset_ms = (frame["epoch_us"] - t0) / 1000
            status = f" WRITE_ERROR=0x{frame['status']:X}" if frame["status"] else ""
            print(f"{offset_ms:+8.1f}ms {frame['direction']} "
                  f"0x{frame['id']:03X} {frame['data'].hex(' ').upper()} "
                  f"{details}{status}")
    if not transitions:
        print("No transition captured. Start the trace before enabling PWM and keep it running through the stop.")


if __name__ == "__main__":
    main()
