"""Record one short QS138 stand run through the current StandRumoteUI server.

The server must be started with WS_CAN_TRACE_FILE for the matching raw CAN log.
Keep this client connected until the external drive has stopped: 0x300 commands
then continue at 10 ms even after a fault or an abort request.
"""

import argparse
import asyncio
import json
import math
import time
from pathlib import Path

import websockets


async def run(args):
    log_path = Path(args.log)
    log_path.parent.mkdir(parents=True, exist_ok=True)
    async with websockets.connect(args.url, max_size=8_000_000) as ws:
        log = log_path.open("w", encoding="utf-8")
        await ws.recv()  # signal catalog
        await ws.send(json.dumps({"cmd": "Init"}))
        await asyncio.sleep(0.7)
        await ws.send(json.dumps({"cmd": "SetJsonPeriod", "period_ms": 50}))
        initial_iq = 0.0 if args.arm_speed_rpm > 0 else args.iq
        for command in (
            {"cmd": "SendLimits", "M_min": -10, "M_max": 10,
             "M_grad_max": 4000, "n_max": args.n_max},
            {"cmd": "SendControl", "En_Is": True, "Kl_15": False,
             "GearCtrl": 4, "MotorCtrl": 1, "ReqState": 1},
            {"cmd": "SendTorque", "En_Is": True, "Isd": 0.0, "Isq": initial_iq},
        ):
            await ws.send(json.dumps(command))
            await asyncio.sleep(0.05)
        print(f"COMMAND_SENT Id=0 Iq={initial_iq}A target={args.iq}A "
              f"arm={args.arm_speed_rpm}rpm n_max={args.n_max}rpm", flush=True)

        start = time.monotonic()
        ready_since = None
        ready = False
        motion = False
        stopped_since = None
        last_gate = None
        last_fault = None
        peak_speed = 0.0
        peak_current = 0.0
        abort_sent = False
        samples = 0
        requested_iq = initial_iq
        ramp_at = 0.0
        released = False
        full_load_since = None
        last_wait_report = start
        while True:
            try:
                raw = await asyncio.wait_for(ws.recv(), 1.0)
            except asyncio.TimeoutError:
                if time.monotonic() - start >= args.timeout_s and not released:
                    requested_iq = 0.0
                    released = True
                    await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                              "Isd": 0.0, "Isq": 0.0}))
                    print("TIMEOUT_RELEASE_IQ_WAITING_FOR_STOP", flush=True)
                continue
            row = json.loads(raw)
            if "MCU_stGateDrv" not in row:
                if row.get("type") == "command_rejected":
                    print(f"COMMAND_REJECTED {row}", flush=True)
                continue
            row["capture_epoch_us"] = time.time_ns() // 1000
            log.write(json.dumps(row, ensure_ascii=False) + "\n")
            log.flush()
            samples += 1
            now = time.monotonic()
            gate = int(row.get("MCU_stGateDrv", 0))
            speed = float(row.get("ns", 0.0))  # corrected host reports rpm
            current = math.hypot(float(row.get("MCU_Isd", 0.0)),
                                 float(row.get("MCU_Isq", 0.0)))
            fault = (int(row.get("McuFailCode", 0)),
                     int(row.get("McuSoftwareFault", 0)),
                     int(row.get("McuHardwareFault", 0)),
                     int(row.get("McuCANFault", 0)))
            peak_speed = max(peak_speed, abs(speed))
            peak_current = max(peak_current, current)
            if gate != last_gate or fault != last_fault:
                print(f"STATE gate={gate} speed={speed:.0f}rpm "
                      f"Id={row.get('MCU_Isd', 0):.1f}A Iq={row.get('MCU_Isq', 0):.1f}A "
                      f"fault={fault} Udc={row.get('Udc', 0):.1f}V", flush=True)
                last_gate, last_fault = gate, fault
            if gate == 2 and ready_since is None:
                ready_since = now
            if gate != 2:
                ready_since = None
            if not ready and ready_since is not None and now - ready_since >= 0.5:
                ready = True
                print("READY_FOR_PEDAL", flush=True)
            if ready and abs(speed) >= 150:
                if not motion:
                    print(f"MOTION_SEEN {speed:.0f}rpm", flush=True)
                motion = True
            if (args.arm_speed_rpm > 0 and ready and not released and
                    not abort_sent and speed <= -args.arm_speed_rpm and
                    now >= ramp_at and requested_iq < args.iq):
                requested_iq = min(args.iq, requested_iq + 1.0)
                await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                          "Isd": 0.0, "Isq": requested_iq}))
                print(f"RAMP_IQ={requested_iq:.1f}A speed={speed:.0f}rpm", flush=True)
                ramp_at = now + 0.2
                if requested_iq >= args.iq:
                    full_load_since = now
            if (args.hold_s > 0 and full_load_since is not None and
                    not released and now - full_load_since >= args.hold_s):
                requested_iq = 0.0
                released = True
                await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                          "Isd": 0.0, "Isq": 0.0}))
                print(f"LOAD_STEP_DONE speed={speed:.0f}rpm", flush=True)
            if (args.arm_speed_rpm > 0 and requested_iq > 0 and
                    speed > -args.release_speed_rpm):
                requested_iq = 0.0
                released = True
                await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                          "Isd": 0.0, "Isq": 0.0}))
                print(f"RELEASE_IQ speed={speed:.0f}rpm", flush=True)
            unsafe = (current >= args.abort_current_a or
                      abs(speed) >= args.abort_speed_rpm or
                      float(row.get("Udc", 0.0)) >= args.abort_udc_v or
                      (ready and float(row.get("Udc", 0.0)) < 50.0) or
                      bool(row.get("DeratingOverVoltage", False)) or
                      fault[0] != 0 or fault[1] != 0 or fault[2] != 0 or
                      fault[3] != 0 or
                      (ready and gate != 2))
            if unsafe and not abort_sent:
                abort_sent = True
                requested_iq = 0.0
                released = True
                print(f"ABORT_COMMANDS speed={speed:.0f}rpm current={current:.1f}A "
                      f"gate={gate} fault={fault}", flush=True)
                await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                          "Isd": 0.0, "Isq": 0.0}))
                # Keep CAN streaming while the shaft coasts to rest.
            if now - start >= args.timeout_s and not released:
                requested_iq = 0.0
                released = True
                await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                          "Isd": 0.0, "Isq": 0.0}))
                print("TIMEOUT_RELEASE_IQ_WAITING_FOR_STOP", flush=True)
            if (released or args.iq == 0.0) and args.stop_current_a >= current and abs(speed) <= args.stop_speed_rpm:
                if stopped_since is None:
                    stopped_since = now
                elif now - stopped_since >= args.stop_stable_s:
                    print(f"VERIFIED_STOP speed={speed:.0f}rpm current={current:.1f}A", flush=True)
                    break
            else:
                stopped_since = None
            if released and now - last_wait_report >= 5.0:
                print(f"WAITING_FOR_STOP speed={speed:.0f}rpm current={current:.1f}A "
                      f"gate={gate} fault={fault}", flush=True)
                last_wait_report = now
        await ws.send(json.dumps({"cmd": "Stop"}))
        await asyncio.sleep(0.25)
        print(f"CAPTURE_DONE samples={samples} peak_rpm={peak_speed:.0f} "
              f"peak_phase_A={peak_current:.1f} aborted={abort_sent}", flush=True)
        log.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="ws://127.0.0.1:9000")
    parser.add_argument("--log", required=True)
    parser.add_argument("--iq", type=float, default=0.0)
    parser.add_argument("--n-max", type=float, default=1700.0)
    parser.add_argument("--timeout-s", type=int, default=110)
    parser.add_argument("--abort-current-a", type=float, default=12.0)
    parser.add_argument("--abort-speed-rpm", type=float, default=1850.0)
    parser.add_argument("--abort-udc-v", type=float, default=63.0)
    parser.add_argument("--arm-speed-rpm", type=float, default=0.0)
    parser.add_argument("--release-speed-rpm", type=float, default=600.0)
    parser.add_argument("--hold-s", type=float, default=0.0,
                        help="Release armed current after this many seconds at target")
    parser.add_argument("--stop-speed-rpm", type=float, default=25.0)
    parser.add_argument("--stop-current-a", type=float, default=2.0)
    parser.add_argument("--stop-stable-s", type=float, default=2.0)
    args = parser.parse_args()
    asyncio.run(run(args))
