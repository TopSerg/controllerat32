"""Print received CAN identifiers on PCAN-USBBUS1 at 1 Mbit/s."""

import collections
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(r"C:\Project\nvert_loader\pc_to_can_loader")))
from PCANBasic import PCANBasic, PCAN_BAUD_1M, PCAN_ERROR_OK, PCAN_USBBUS1  # noqa: E402

bus = PCANBasic()
status = bus.Initialize(PCAN_USBBUS1, PCAN_BAUD_1M)
print(f"PCAN init status={int(status)}", flush=True)
if status != PCAN_ERROR_OK:
    raise SystemExit(1)
counts = collections.Counter()
examples = {}
until = time.monotonic() + 5.0
while time.monotonic() < until:
    result = bus.Read(PCAN_USBBUS1)
    if result[0] == PCAN_ERROR_OK:
        frame = result[1]
        counts[frame.ID] += 1
        examples[frame.ID] = bytes(frame.DATA[:frame.LEN]).hex(" ")
for can_id, count in counts.most_common():
    print(f"0x{can_id:03X} count={count} last={examples[can_id]}")
bus.Uninitialize(PCAN_USBBUS1)
