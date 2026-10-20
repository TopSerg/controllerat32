"""Export a stand capture JSONL file as a flat CSV for analysis."""

import argparse
import csv
import json
from pathlib import Path


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("input", type=Path)
parser.add_argument("output", type=Path)
args = parser.parse_args()

rows = [json.loads(line) for line in args.input.read_text(encoding="utf-8").splitlines()]
if not rows:
    raise SystemExit("Capture is empty")
fields = sorted(set().union(*(row.keys() for row in rows)))
if "capture_epoch_us" in fields:
    fields.remove("capture_epoch_us")
    fields.insert(0, "capture_epoch_us")

with args.output.open("w", encoding="utf-8", newline="") as stream:
    writer = csv.DictWriter(stream, fieldnames=fields)
    writer.writeheader()
    for row in rows:
        writer.writerow({
            field: json.dumps(value, ensure_ascii=False) if isinstance(value, (dict, list)) else value
            for field, value in row.items()
        })

print(f"rows={len(rows)} output={args.output}")
