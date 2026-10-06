#!/usr/bin/env python3
"""Decode the QS138 10 kHz over-current flight-recorder JTAG dump to CSV.

Example GDB dump after g_qs138FlightRecorderFrozen becomes 1:

    dump binary memory ocfr.bin \
        &g_qs138FlightRecorder \
        ((char *)&g_qs138FlightRecorder + sizeof(g_qs138FlightRecorder))

Then read the metadata in GDB and run, for example:

    python tools/decode_qs138_flight_recorder.py ocfr.bin \
        --write-index 73 --trigger-index 56 --valid 256 \
        -o ocfr.csv
"""

from __future__ import annotations

import argparse
import csv
import struct
from pathlib import Path

CAPACITY = 256
SAMPLE_FORMAT = struct.Struct("<I4H17f5B3x")
SAMPLE_SIZE = SAMPLE_FORMAT.size

FIELDNAMES = [
    "sequence",
    "rawIa", "rawIb", "rawIc", "rawUdc",
    "Ia", "Ib", "Ic",
    "Id", "Iq", "IdRef", "IqRef",
    "Ud", "Uq", "UmodRef", "Udc",
    "thetaElectrical", "wElectrical", "wMechanical",
    "pwmTa", "pwmTb", "pwmTc",
    "modActive", "overCurrentInstant", "overCurrentLatchedBefore",
    "globalErrorBefore", "hardwareFaultInput",
]


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser()
    p.add_argument("input", type=Path, help="binary dump of g_qs138FlightRecorder")
    p.add_argument("-o", "--output", type=Path, default=Path("qs138_flight_recorder.csv"))
    p.add_argument("--write-index", type=int, required=True,
                   help="g_qs138FlightRecorderWriteIndex")
    p.add_argument("--trigger-index", type=int, required=True,
                   help="g_qs138FlightRecorderTriggerIndex (65535 if no trigger)")
    p.add_argument("--valid", type=int, required=True,
                   help="g_qs138FlightRecorderValidSamples")
    p.add_argument("--sample-period-us", type=float, default=100.0)
    return p.parse_args()


def ordered_ring_indices(valid: int, write_index: int) -> list[int]:
    if not 0 <= valid <= CAPACITY:
        raise ValueError(f"valid must be 0..{CAPACITY}")
    if not 0 <= write_index < CAPACITY:
        raise ValueError(f"write-index must be 0..{CAPACITY - 1}")

    if valid < CAPACITY:
        return list(range(valid))

    return [(write_index + i) % CAPACITY for i in range(CAPACITY)]


def main() -> None:
    args = parse_args()
    data = args.input.read_bytes()
    expected = CAPACITY * SAMPLE_SIZE
    if len(data) != expected:
        raise SystemExit(
            f"Unexpected dump size: {len(data)} bytes; expected {expected} "
            f"({CAPACITY} x {SAMPLE_SIZE})."
        )

    samples = []
    for i in range(CAPACITY):
        values = SAMPLE_FORMAT.unpack_from(data, i * SAMPLE_SIZE)
        samples.append(dict(zip(FIELDNAMES, values)))

    indices = ordered_ring_indices(args.valid, args.write_index)
    trigger_position = None
    if 0 <= args.trigger_index < CAPACITY and args.trigger_index in indices:
        trigger_position = indices.index(args.trigger_index)

    rows = []
    for position, ring_index in enumerate(indices):
        row = {
            "ringIndex": ring_index,
            "relativeSample": "" if trigger_position is None else position - trigger_position,
            "relativeTime_us": "" if trigger_position is None else
                (position - trigger_position) * args.sample_period_us,
        }
        row.update(samples[ring_index])
        rows.append(row)

    columns = ["ringIndex", "relativeSample", "relativeTime_us", *FIELDNAMES]
    with args.output.open("w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=columns)
        writer.writeheader()
        writer.writerows(rows)

    print(f"Decoded {len(rows)} valid samples to {args.output}")
    print(f"Sample size: {SAMPLE_SIZE} bytes, sample period: {args.sample_period_us:g} us")
    if trigger_position is None:
        print("Trigger is not present in the valid data.")
    else:
        print(
            f"Trigger: ring index {args.trigger_index}, ordered row {trigger_position}, "
            f"sequence {samples[args.trigger_index]['sequence']}"
        )


if __name__ == "__main__":
    main()
