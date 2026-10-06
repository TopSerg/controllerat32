"""Interactive, guarded StandRumoteUI client for the QS138 staged test.

Start with the shaft stopped. Commands on stdin: start, zero, iq=0.5,
iq=1.0, iq=2.0, status, stop, stop_after_dump. On a fault this process
holds zero-current CAN commands and deliberately does not clear the fault.
"""

import argparse
import asyncio
import json
import math
import sys
import time
from pathlib import Path

import websockets


def control(motor):
    return {"cmd": "SendControl", "En_Is": True, "Kl_15": False,
            "GearCtrl": 4, "MotorCtrl": motor, "ReqState": 1}


def torque(iq):
    return {"cmd": "SendTorque", "En_Is": True, "Isd": 0.0, "Isq": iq}


async def input_worker(queue):
    while True:
        line = await asyncio.to_thread(sys.stdin.readline)
        if not line:
            await asyncio.sleep(1.0)
            continue
        await queue.put(line.strip().lower())


async def main(args):
    log_path = Path(args.log)
    log_path.parent.mkdir(parents=True, exist_ok=True)
    commands = asyncio.Queue()
    reader = asyncio.create_task(input_worker(commands))
    phase = "ready"
    requested_iq = 0.0
    return_to_zero_at = None
    last_gate = None
    last_fault = None
    latest = {}
    gate_ready_since = None
    try:
        async with websockets.connect(args.url, max_size=8_000_000) as ws:
            await ws.recv()
            for command in (
                {"cmd": "Init"},
                {"cmd": "SetJsonPeriod", "period_ms": 20},
                {"cmd": "SendLimits", "M_min": -10, "M_max": 10,
                 "M_grad_max": 4000, "n_max": 2050},
                control(0), torque(0.0),
            ):
                await ws.send(json.dumps(command))
                await asyncio.sleep(0.05)
            print("READY_FOR_JTAG_STATIONARY_CHECK; MotorCtrl=0 Id=Iq=0", flush=True)
            with log_path.open("w", encoding="utf-8") as log:
                while True:
                    try:
                        message = json.loads(await asyncio.wait_for(ws.recv(), 0.1))
                    except asyncio.TimeoutError:
                        message = None
                    now = time.monotonic()
                    if message is not None and "MCU_stGateDrv" in message:
                        message["capture_epoch_us"] = time.time_ns() // 1000
                        log.write(json.dumps(message, ensure_ascii=False) + "\n")
                        log.flush()
                        latest = message
                        speed = float(message.get("ns", 0.0))
                        udc = float(message.get("Udc", 0.0))
                        id_meas = float(message.get("MCU_Isd", 0.0))
                        iq_meas = float(message.get("MCU_Isq", 0.0))
                        gate = int(message["MCU_stGateDrv"])
                        fault = (int(message.get("McuFailCode", 0)),
                                 int(message.get("McuSoftwareFault", 0)),
                                 int(message.get("McuHardwareFault", 0)),
                                 int(message.get("McuCANFault", 0)))
                        if gate != last_gate or fault != last_fault:
                            print(f"STATE phase={phase} gate={gate} rpm={speed:.0f} "
                                  f"Id={id_meas:.1f} Iq={iq_meas:.1f} "
                                  f"Udc={udc:.1f} fault={fault}", flush=True)
                            last_gate, last_fault = gate, fault
                        if phase in ("starting", "active"):
                            if gate == 2:
                                if gate_ready_since is None:
                                    gate_ready_since = now
                                if phase == "starting" and now - gate_ready_since >= 0.5:
                                    phase = "active"
                                    print("PWM_ACTIVE_STABLE; check JTAG/CAN before pedal", flush=True)
                            else:
                                gate_ready_since = None
                        fault_detected = any(fault) or (phase == "active" and gate != 2)
                        safety_limit = (
                            max(abs(id_meas), abs(iq_meas)) >= 5.0 or
                            udc >= 63.0 or udc < 55.0 or abs(speed) >= 2025.0
                        )
                        if phase in ("starting", "active") and (fault_detected or safety_limit):
                            phase = "fault_hold"
                            requested_iq = 0.0
                            return_to_zero_at = None
                            await ws.send(json.dumps(torque(0.0)))
                            if fault_detected:
                                print("FAULT_HOLD_ZERO: stop right drive; do not reset "
                                      "or clear fault before JTAG dump", flush=True)
                            else:
                                # Stage B2 uses a deliberately conservative 5 A
                                # d/q guard. Repeating a zero-current request does
                                # not remove the cause, so turn CONTROL off.
                                await ws.send(json.dumps(control(0)))
                                phase = "safety_stop"
                                print("SAFETY_STOP_PWM_OFF: stop right drive; "
                                      "inspect JTAG recorder before reset", flush=True)
                    elif message is not None and message.get("type") == "command_rejected":
                        print(f"COMMAND_REJECTED {message}", flush=True)

                    if return_to_zero_at is not None and now >= return_to_zero_at:
                        requested_iq = 0.0
                        return_to_zero_at = None
                        await ws.send(json.dumps(torque(0.0)))
                        print("CURRENT_STEP_DONE Iq=0", flush=True)

                    while not commands.empty():
                        command = await commands.get()
                        if command == "status":
                            print(f"STATUS phase={phase} request_Iq={requested_iq} "
                                  f"rpm={latest.get('ns')} gate={latest.get('MCU_stGateDrv')} "
                                  f"Udc={latest.get('Udc')} "
                                  f"fault={latest.get('McuSoftwareFault')}", flush=True)
                        elif command == "start" and phase == "ready":
                            if (abs(float(latest.get("ns", 1e9))) < 20.0 and
                                55.0 <= float(latest.get("Udc", 0.0)) < 63.0 and
                                not any(int(latest.get(k, 0)) for k in
                                        ("McuFailCode", "McuSoftwareFault",
                                         "McuHardwareFault", "McuCANFault"))):
                                await ws.send(json.dumps(torque(0.0)))
                                await ws.send(json.dumps(control(1)))
                                phase = "starting"
                                print("PWM_START_REQUESTED at rest Id=Iq=0", flush=True)
                            else:
                                print("START_REJECTED: shaft, Udc or fault not ready", flush=True)
                        elif command == "zero" and phase in ("starting", "active"):
                            requested_iq = 0.0
                            return_to_zero_at = None
                            await ws.send(json.dumps(torque(0.0)))
                            print("CURRENT_ZERO_REQUESTED", flush=True)
                        elif command in ("iq=0.5", "iq=1.0", "iq=2.0") and phase == "active":
                            speed = abs(float(latest.get("ns", 0.0)) )
                            if 500.0 <= speed <= 1000.0 and return_to_zero_at is None:
                                requested_iq = float(command.split("=")[1])
                                await ws.send(json.dumps(torque(requested_iq)))
                                return_to_zero_at = now + 2.0
                                print(f"CURRENT_STEP Iq={requested_iq}A for 2s", flush=True)
                            else:
                                print("CURRENT_STEP_REJECTED: speed must be 500..1000 "
                                      "rpm and prior step complete", flush=True)
                        elif command in ("stop", "stop_after_dump"):
                            if phase == "fault_hold" and command != "stop_after_dump":
                                print("STOP_REJECTED: dump frozen flight recorder first", flush=True)
                            elif abs(float(latest.get("ns", 1e9))) >= 50.0:
                                print("STOP_REJECTED: wait until shaft stops", flush=True)
                            else:
                                await ws.send(json.dumps({"cmd": "Stop"}))
                                await asyncio.sleep(0.3)
                                print("STOP_SENT", flush=True)
                                return
                        else:
                            print(f"COMMAND_IGNORED {command} phase={phase}", flush=True)
    finally:
        reader.cancel()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="ws://127.0.0.1:9000")
    parser.add_argument("--log", required=True)
    asyncio.run(main(parser.parse_args()))
