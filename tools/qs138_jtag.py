"""Read QS138 commissioning status or dump a frozen flight recorder via J-Link.

The target is never halted or reset. Addresses are resolved from the matching
ELF, so use the same --elf that produced the flashed binary.
"""

import argparse
import json
import re
import shutil
import struct
import subprocess
import tempfile
from pathlib import Path


JLINK = Path(r"C:\Program Files\SEGGER\JLink_V824\JLink.exe")
FIELDS = {
    "resolverReady": ("g_resolverSignalsReady", "u8"),
    "offsetsFrozen": ("g_qs138CurrentOffsetsFrozen", "u8"),
    "latePwmBlocked": ("g_qs138LatePwmEnableBlocked", "u8"),
    "frozenIaOffset": ("g_qs138FrozenIaOffset", "f32"),
    "frozenIbOffset": ("g_qs138FrozenIbOffset", "f32"),
    "frozenIcOffset": ("g_qs138FrozenIcOffset", "f32"),
    "backEmfFfEnable": ("g_qs138BackEmfFeedForwardEnable", "u8"),
    "backEmfFfGain": ("g_qs138BackEmfFeedForwardGain", "f32"),
    "backEmfFfRawV": ("g_qs138BackEmfFeedForwardRawV", "f32"),
    "backEmfFfCommandV": ("g_qs138BackEmfFeedForwardCommandV", "f32"),
    "backEmfFfLimited": ("g_qs138BackEmfFeedForwardLimited", "u8"),
    "vqPlay": ("VqPlay", "f32"),
    "rpmRadPerSec": ("Control.Wmechanical", "f32"),
    "wElectrical": ("Control.Welectrical", "f32"),
    "udcFiltered": ("Control.UdcFiltered", "f32"),
    "Ia": ("inSignals.Ia", "f32"),
    "Ib": ("inSignals.Ib", "f32"),
    "Ic": ("inSignals.Ic", "f32"),
    "Id": ("Control.Id", "f32"),
    "Iq": ("Control.Iq", "f32"),
    "IdRef": ("Control.IdRefReg", "f32"),
    "IqRef": ("Control.IqRefReg", "f32"),
    "Ud": ("Control.Ud", "f32"),
    "Uq": ("Control.Uq", "f32"),
    "UmodRef": ("Control.UmodRef", "f32"),
    "thetaElectrical": ("Control.ThetaElectrical", "f32"),
    "fluxPositionError": ("Control.motorFluxPosError", "f32"),
    "modActive": ("Control.stat.mod_Active", "u8"),
    "globalError": ("Control.errors.GlobalError", "u8"),
    "overCurrent": ("Control.errors.OverCurrent", "u8"),
    "hardwareFault": ("Control.errors.HardWareFault", "u8"),
    "tripAmps": ("TripLevels.OverCurrent_level", "u16"),
    "writeIndex": ("g_qs138FlightRecorderWriteIndex", "u16"),
    "triggerIndex": ("g_qs138FlightRecorderTriggerIndex", "u16"),
    "validSamples": ("g_qs138FlightRecorderValidSamples", "u16"),
    "triggered": ("g_qs138FlightRecorderTriggered", "u8"),
    "frozen": ("g_qs138FlightRecorderFrozen", "u8"),
    "armed": ("g_qs138FlightRecorderArmed", "u8"),
    "sequence": ("g_qs138FlightRecorderSequence", "u32"),
    "recorder": ("g_qs138FlightRecorder", "address"),
}


def resolve_addresses(elf):
    gdb = shutil.which("arm-none-eabi-gdb")
    if not gdb:
        raise RuntimeError("arm-none-eabi-gdb was not found")
    command = [gdb, "-q", "-batch", str(elf)]
    for expression, _ in FIELDS.values():
        command += ["-ex", f"p/x (unsigned long)&{expression}"]
    result = subprocess.run(command, capture_output=True, text=True, check=True)
    values = re.findall(r"\$\d+ = 0x([0-9a-fA-F]+)", result.stdout)
    if len(values) != len(FIELDS):
        raise RuntimeError(f"Could not resolve all JTAG symbols: {result.stdout}\n{result.stderr}")
    return dict(zip(FIELDS, (int(value, 16) for value in values)))


def jlink_run(lines):
    with tempfile.TemporaryDirectory() as folder:
        commands = Path(folder) / "commands.jlink"
        commands.write_text("\n".join([
            "device Cortex-M4", "if SWD", "speed 1000", "connect",
            *lines, "exit", "",
        ]), encoding="ascii")
        result = subprocess.run([str(JLINK), "-NoGui", "1", "-CommandFile",
                                 str(commands)], capture_output=True, text=True,
                                check=True, timeout=30)
    if "Cannot connect" in result.stdout or "Error while reading" in result.stdout:
        raise RuntimeError(result.stdout)
    return result.stdout


def read_status(addresses):
    commands = []
    for name, (_, kind) in FIELDS.items():
        if kind == "address":
            continue
        command = {"u8": "mem8", "u16": "mem16", "u32": "mem32", "f32": "mem32"}[kind]
        commands.append(f"{command} 0x{addresses[name]:08X},1")
    output = jlink_run(commands)
    result = {}
    for name, (_, kind) in FIELDS.items():
        if kind == "address":
            continue
        address = addresses[name]
        match = re.search(rf"(?m)^{address:08X} = ([0-9A-Fa-f]+)", output)
        if not match:
            raise RuntimeError(f"J-Link did not read {name} at 0x{address:08X}\n{output}")
        value = int(match.group(1), 16)
        if kind == "f32":
            value = struct.unpack("<f", value.to_bytes(4, "little"))[0]
        result[name] = value
    result["rpm"] = result["rpmRadPerSec"] * 60.0 / (2.0 * 3.141592653589793)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("status", "dump"))
    parser.add_argument("--elf", type=Path,
                        default=Path("build/debug/AT32_Base.elf"))
    parser.add_argument("--output", type=Path,
                        help="raw .bin path for the frozen recorder")
    args = parser.parse_args()
    addresses = resolve_addresses(args.elf)
    status = read_status(addresses)
    print(json.dumps(status, indent=2))
    if args.action == "dump":
        if args.output is None:
            parser.error("dump requires --output")
        if status["frozen"] != 1 or status["triggered"] != 1:
            raise RuntimeError("Recorder is not frozen on an over-current event")
        args.output.parent.mkdir(parents=True, exist_ok=True)
        output = jlink_run([f"SaveBin {args.output.resolve()}, "
                            f"0x{addresses['recorder']:08X}, 0x6000"])
        if not args.output.exists() or args.output.stat().st_size != 24576:
            raise RuntimeError(f"J-Link dump failed: {output}")
        metadata = args.output.with_suffix(".json")
        metadata.write_text(json.dumps(status, indent=2), encoding="utf-8")
        print(f"SAVED {args.output.resolve()} bytes=24576 metadata={metadata.resolve()}")


if __name__ == "__main__":
    main()
