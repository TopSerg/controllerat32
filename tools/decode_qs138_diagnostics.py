"""Reassemble the temporary QS138 0x5D0..0x5E2 control snapshots.

Usage: py -3.13 tools/decode_qs138_diagnostics.py raw_can.csv diagnostics.csv
The 19 frames of each snapshot carry one coherent 10 kHz control sample and
per-phase PWM rail counts over the preceding 200 control cycles (20 ms).
"""

import argparse
import csv
import struct
from pathlib import Path

BASE_ID = 0x5D0
LAST_ID = 0x5E2
SIGNALS = (
    "Wmechanical", "Welectrical", "IdRefReg", "IqRefReg", "Id", "Iq",
    "Ud", "Uq", "UdRef", "UqRef", "UmodActual", "UmodRef",
    "UdcFiltered", "IdcFilt", "PID_ID.Out", "PID_ID.OutPreSat",
    "PID_ID.SatErr", "PID_ID.Ui", "PID_IQ.Out", "PID_IQ.OutPreSat",
    "PID_IQ.SatErr", "PID_IQ.Ui", "ThetaElectrical", "ThetaElectrCorr",
    "motorFluxPosError", "SVPWM.Ta", "SVPWM.Tb", "SVPWM.Tc",
    "motorTorque", "SVPWM.PRD_VAL",
)


class DiagnosticAssembler:
    def __init__(self):
        self.header = None
        self.parts = {}
        self.incomplete = 0
        self.complete = 0

    def feed(self, frame):
        if frame["direction"] != "RX":
            return None
        frame_id = int(frame["id"], 0)
        if not BASE_ID <= frame_id <= LAST_ID:
            return None
        data = bytes.fromhex(frame["data"])
        if len(data) != 8:
            return None
        index = frame_id - BASE_ID
        if index == 0:
            if self.header is not None:
                self.incomplete += 1
            seq, flags, samples, tick_ms = struct.unpack("<BBHI", data)
            self.header = {
                "epoch_us": int(frame["epoch_us"]),
                "pcan_us": int(frame["pcan_us"]),
                "sequence": seq,
                "tick_ms": tick_ms,
                "pwm_samples": samples,
                "fwActive": bool(flags & 1),
                "saturationActive": bool(flags & 2),
                "modActive": bool(flags & 4),
                "idcLimit": bool(flags & 8),
                "udcLimit": bool(flags & 16),
            }
            self.parts = {}
            return None
        if self.header is None:
            return None
        self.parts[index] = data
        if len(self.parts) != 18:
            return None
        record = self.header.copy()
        values = []
        for part in range(1, 16):
            values.extend(struct.unpack("<ff", self.parts[part]))
        record.update(zip(SIGNALS, values))
        for phase, part in zip("abc", range(16, 19)):
            minimum, maximum, low, high = struct.unpack("<HHHH", self.parts[part])
            record[f"duty_{phase}_min"] = minimum / 65535.0
            record[f"duty_{phase}_max"] = maximum / 65535.0
            record[f"duty_{phase}_low_count"] = low
            record[f"duty_{phase}_high_count"] = high
            record[f"duty_{phase}_low_fraction"] = low / record["pwm_samples"] if record["pwm_samples"] else 0.0
            record[f"duty_{phase}_high_fraction"] = high / record["pwm_samples"] if record["pwm_samples"] else 0.0
        self.header = None
        self.parts = {}
        self.complete += 1
        return record

    def finish(self):
        if self.header is not None:
            self.incomplete += 1
            self.header = None
            self.parts = {}


def iter_trace(path):
    with Path(path).open(newline="", encoding="utf-8") as stream:
        yield from csv.DictReader(stream, delimiter=";")


def output_fields():
    fields = ["epoch_us", "pcan_us", "sequence", "tick_ms", "pwm_samples",
              "fwActive", "saturationActive", "modActive", "idcLimit", "udcLimit"]
    fields.extend(SIGNALS)
    for phase in "abc":
        fields.extend(f"duty_{phase}_{suffix}" for suffix in
                      ("min", "max", "low_count", "high_count",
                       "low_fraction", "high_fraction"))
    return fields


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trace", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    assembler = DiagnosticAssembler()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=output_fields())
        writer.writeheader()
        for frame in iter_trace(args.trace):
            record = assembler.feed(frame)
            if record is not None:
                writer.writerow(record)
    assembler.finish()
    print(f"snapshots={assembler.complete} incomplete={assembler.incomplete} "
          f"output={args.output}")


if __name__ == "__main__":
    main()
