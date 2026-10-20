"""Flash the built Gen1.1 firmware to bootloader device 240 over PCAN 1 Mbit/s.

The script waits for the bootloader broadcast so it can be started before a
physical inverter reset. It uses the same protocol as nvert_loader's GUI.
"""

import argparse
import hashlib
from pathlib import Path
import sys
import time
import zlib

LOADER_DIR = Path(r"C:\Project\nvert_loader\pc_to_can_loader")
sys.path.insert(0, str(LOADER_DIR))

from bootloader import NvertFirmwareUpdater  # noqa: E402
from interface_impl.can import CanIO, PCAN_BAUD_1M, PCAN_ERROR_OK  # noqa: E402


class BoundedCanIO(CanIO):
    def WaitACK(self, can_id, timeout=None):
        result = super().WaitACK(can_id, timeout if timeout is not None else 5)
        if result is None:
            raise TimeoutError(f"No bootloader acknowledgement on 0x{can_id:03X}")
        return result

    def WriteMessage(self, can_id, data):
        result = super().WriteMessage(can_id, data)
        if result != PCAN_ERROR_OK:
            raise RuntimeError(f"PCAN write 0x{can_id:03X} failed: {result}")
        return result

    def write_chunk(self, chunk):
        result = super().write_chunk(chunk)
        if result != 0:
            raise RuntimeError(f"Firmware chunk rejected: status {result}")
        return result


class BoundedUpdater(NvertFirmwareUpdater):
    def await_mcu_command_request(self):
        self.io.Reset()
        deadline = time.monotonic() + 8
        while time.monotonic() < deadline:
            remaining = deadline - time.monotonic()
            if self.io.WaitACK(0xFE, min(1, remaining)) == self.mcu_id:
                print(f"BOOT_DEVICE={self.mcu_id}", flush=True)
                return
        raise TimeoutError("Bootloader request disappeared before command")


def validate_binary(firmware):
    for data_end, crc_offset in ((0x1FFF0, 0x1FFF0), (0x23FF0, 0x23FF0)):
        data_start = 0 if data_end == 0x1FFF0 else 0x20000
        expected = int.from_bytes(firmware[crc_offset:crc_offset + 4], "little")
        actual = zlib.crc32(firmware[data_start:data_end])
        if actual != expected:
            raise ValueError(f"Binary CRC mismatch at offset 0x{crc_offset:X}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", type=Path,
                        default=Path(r"C:\Project\controllerat32\build\debug\AT32_Base.bin"))
    parser.add_argument("--wait-s", type=int, default=180)
    args = parser.parse_args()
    firmware = args.binary.read_bytes()
    validate_binary(firmware)
    print(f"BINARY={args.binary} BYTES={len(firmware)} "
          f"SHA256={hashlib.sha256(firmware).hexdigest()}", flush=True)

    io = BoundedCanIO(PCAN_BAUD_1M, boot_id=0xFF, mcu_id=240,
                      public_boot_id=0xFE)
    updater = BoundedUpdater(io)
    print("WAITING_FOR_BOOT_DEVICE_240", flush=True)
    deadline = time.monotonic() + args.wait_s
    while time.monotonic() < deadline:
        found = updater.scan_device()
        if found:
            if found != {240}:
                raise RuntimeError(f"Unexpected bootloader device IDs: {sorted(found)}")
            break
    else:
        raise TimeoutError("Device 240 did not appear in boot mode")

    padded_size = len(firmware) + (8 - len(firmware) % 8)
    progress = 0
    for progress in updater.load_firmware(firmware):
        if progress == padded_size:
            print("TRANSFER_COMPLETE", flush=True)
        elif progress % 0x4000 == 0:
            print(f"TRANSFERRED={progress}/{padded_size}", flush=True)
    if progress != padded_size:
        raise RuntimeError(f"Transfer stopped at {progress}/{padded_size}")
    print("VERIFYING_FIRMWARE_CRC_AND_STARTING", flush=True)
    try:
        started = updater.start_firmware()
    except TimeoutError:
        # This bootloader can jump to the application before PCAN receives
        # its final 0xFF acknowledgement. Confirm periodic application CAN.
        started = False
        deadline = time.monotonic() + 3
        while time.monotonic() < deadline:
            received = io.m_objPCANBasic.Read(io.PcanHandle)
            if received[0] == PCAN_ERROR_OK and received[1].ID in (0x07A, 0x07D):
                started = True
                print("APPLICATION_CAN_CONFIRMED", flush=True)
                break
    if started is not True:
        raise RuntimeError("Bootloader refused to start the firmware")
    print("FIRMWARE_STARTED", flush=True)


if __name__ == "__main__":
    main()
