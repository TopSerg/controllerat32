"""Flash the verified QS138 Stage B2 image through the Nvert PCAN bootloader."""

import argparse
import hashlib
import sys
import time
from pathlib import Path


LOADER = Path(r"C:\Project\nvert_loader\pc_to_can_loader")
sys.path.insert(0, str(LOADER))

from PCANBasic import PCAN_BAUD_1M, PCAN_ERROR_OK  # noqa: E402
from bootloader import NvertFirmwareUpdater  # noqa: E402
from interface_impl.can import CanIO  # noqa: E402


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path)
    parser.add_argument("--expected-sha256", required=True)
    parser.add_argument("--scan-seconds", type=float, default=180.0)
    args = parser.parse_args()
    firmware = args.binary.read_bytes()
    digest = hashlib.sha256(firmware).hexdigest()
    if digest.lower() != args.expected_sha256.lower():
        raise RuntimeError(f"Binary changed: {digest}")
    print(f"BINARY={args.binary.resolve()} BYTES={len(firmware)} SHA256={digest}", flush=True)

    io = CanIO(PCAN_BAUD_1M, 0xFF, 240, 0xFE)
    loader = NvertFirmwareUpdater(io)
    print("WAITING_FOR_BOOT_DEVICE_240", flush=True)
    until = time.monotonic() + args.scan_seconds
    while time.monotonic() < until:
        if 240 in loader.scan_device():
            print("BOOT_DEVICE=240", flush=True)
            break
    else:
        raise RuntimeError("Device 240 did not appear in boot mode")

    for count in loader.load_firmware(firmware):
        if count % 16384 == 0 or count >= len(firmware):
            print(f"TRANSFERRED={count}/{len(firmware)}", flush=True)
    print("TRANSFER_COMPLETE", flush=True)
    print("VERIFYING_FIRMWARE_CRC_AND_STARTING", flush=True)
    if loader.start_firmware() is not True:
        raise RuntimeError("Bootloader did not confirm firmware start")
    print("FIRMWARE_STARTED", flush=True)

    until = time.monotonic() + 10.0
    while time.monotonic() < until:
        result = io.m_objPCANBasic.Read(io.PcanHandle)
        if result[0] == PCAN_ERROR_OK and result[1].ID == 0x2C6:
            print("APPLICATION_CAN_0x2C6_CONFIRMED", flush=True)
            return
    raise RuntimeError("Application CAN 0x2C6 not observed after firmware start")


if __name__ == "__main__":
    main()
