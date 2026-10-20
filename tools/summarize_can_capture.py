"""Summarize current, speed, faults and command cadence in a CAN trace."""

import argparse
from collections import Counter
from bisect import bisect_left
from datetime import datetime
import math
from pathlib import Path
from statistics import median

from analyze_can_transition import read_frames, signal


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trace", type=Path)
    args = parser.parse_args()
    frames = list(read_frames(args.trace))
    if not frames:
        raise SystemExit("No frames")
    speeds = []
    currents = []
    faults = Counter()
    gates = Counter()
    commands = []
    flux = []
    for f in frames:
        data = f["data"]
        if len(data) != 8:
            continue
        if f["direction"] == "RX":
            if f["id"] == 0x07A:
                speeds.append((f["epoch_us"], signal(data, 39, 16) - 32768))
            elif f["id"] == 0x07D:
                gate = signal(data, 34, 2)
                id_a = signal(data, 11, 12) * 0.5 - 1023.5
                iq_a = signal(data, 31, 12) * 0.5 - 1023.5
                currents.append((f["epoch_us"], gate, id_a, iq_a,
                                 math.hypot(id_a, iq_a)))
                gates[gate] += 1
            elif f["id"] == 0x2C6:
                faults[(signal(data, 7, 3), signal(data, 15, 19),
                        signal(data, 39, 16), signal(data, 55, 8),
                        signal(data, 63, 8))] += 1
            elif f["id"] == 0x082:
                status = signal(data, 55, 8)
                error = signal(data, 7, 16) * 0.0001
                if error > math.pi:
                    error -= 2 * math.pi
                correction = signal(data, 23, 16) * 0.0001
                if correction > math.pi:
                    correction -= 2 * math.pi
                electrical_speed = signal(data, 39, 16) * 0.1 - 3276.8
                flux.append((f["epoch_us"], status, error, correction,
                             electrical_speed))
        elif f["direction"] == "TX" and f["id"] == 0x300:
            commands.append((f["epoch_us"], signal(data, 7, 16) * 0.1 - 3200,
                             signal(data, 23, 16) * 0.1 - 3200,
                             signal(data, 39, 1), signal(data, 51, 4),
                             f["status"]))
    gaps = [(b[0] - a[0]) / 1000 for a, b in zip(commands, commands[1:])]
    jumps = sum(b[4] != (a[4] + 1) % 16 for a, b in zip(commands, commands[1:]))
    high = sorted(currents, key=lambda row: row[4], reverse=True)[:10]
    speed_times = [t for t, _ in speeds]
    def nearby_speed(timestamp):
        i = bisect_left(speed_times, timestamp)
        i = min(max(i, 0), len(speeds) - 1)
        return speeds[i][1]
    print("frames", len(frames), "duration_s", round((frames[-1]["epoch_us"] -
          frames[0]["epoch_us"]) / 1e6, 2))
    print("gate_counts", dict(gates))
    print("speed_rad_s", min(x[1] for x in speeds), max(x[1] for x in speeds))
    print("speed_rpm", round(min(x[1] for x in speeds) * 30 / math.pi),
          round(max(x[1] for x in speeds) * 30 / math.pi))
    print("max_abs_Id_A", max(abs(x[2]) for x in currents))
    print("max_abs_Iq_A", max(abs(x[3]) for x in currents))
    print("max_dq_magnitude_A", max(x[4] for x in currents))
    print("top_currents", [(datetime.fromtimestamp(t / 1e6).isoformat(timespec="milliseconds"),
                            gate, nearby_speed(t), id_a, iq_a, round(mag, 2))
                           for t, gate, id_a, iq_a, mag in high])
    for low, high_speed in ((0, 10), (10, 20), (20, 30), (30, 40), (40, 60), (60, 100)):
        subset = [x for x in currents if x[1] == 2 and
                  low <= abs(nearby_speed(x[0])) < high_speed]
        if subset:
            mags = sorted(x[4] for x in subset)
            print(f"current_by_speed_{low}_{high_speed}_rad_s", len(mags),
                  "median", round(median(mags), 2), "p95",
                  round(mags[int(0.95 * (len(mags) - 1))], 2),
                  "max", round(mags[-1], 2))
    print("faults_level_sw_hw_sensor_can", dict(faults))
    valid_flux = [x for x in flux if x[1] & 1]
    print("flux_status_counts", dict(Counter(x[1] for x in flux)),
          "valid_count", len(valid_flux))
    if valid_flux:
        print("valid_flux_error_rad_min_median_max",
              round(min(x[2] for x in valid_flux), 4),
              round(median(x[2] for x in valid_flux), 4),
              round(max(x[2] for x in valid_flux), 4),
              "correction_median", round(median(x[3] for x in valid_flux), 4),
              "electrical_speed_abs_median",
              round(median(abs(x[4]) for x in valid_flux), 1))
        print("last_valid_flux_samples", [
            (round((x[0] - valid_flux[0][0]) / 1000), round(x[2], 4),
             round(x[3], 4), round(x[4], 1)) for x in valid_flux[-12:]
        ])
    print("commands", len(commands), "nonzero_commands",
          sum(abs(x[1]) > 0.01 or abs(x[2]) > 0.01 for x in commands),
          "disabled", sum(x[3] == 0 for x in commands),
          "write_errors", sum(x[5] != 0 for x in commands),
          "counter_jumps", jumps)
    print("command_gap_ms_median_max", round(median(gaps), 2), round(max(gaps), 2))


if __name__ == "__main__":
    main()
