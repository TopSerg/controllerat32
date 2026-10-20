"""One guarded external-spin capture across the QS138 problem speed.

Start MarathonWS with WS_CAN_TRACE_FILE before running this program. It keeps
the current command alive until the external shaft has stopped, including
after a detected fault. The high-speed 2 A request lasts about 1.5 seconds.
"""

import argparse
import asyncio
import json
import math
import time
from pathlib import Path

import websockets


async def send_torque(ws, iq):
    await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                              "Isd": 0.0, "Isq": iq}))


async def main(args):
    log_path = Path(args.log)
    log_path.parent.mkdir(parents=True, exist_ok=True)
    async with websockets.connect(args.url, max_size=8_000_000) as ws:
        await ws.recv()  # signal catalog
        await ws.send(json.dumps({"cmd": "Init"}))
        await asyncio.sleep(0.7)
        await ws.send(json.dumps({"cmd": "SetJsonPeriod", "period_ms": 50}))
        await ws.send(json.dumps({"cmd": "SendLimits", "M_min": -10,
                                  "M_max": 10, "M_grad_max": 4000,
                                  "n_max": args.n_max}))
        await asyncio.sleep(0.05)
        await ws.send(json.dumps({"cmd": "SendControl", "En_Is": True,
                                  "Kl_15": False, "GearCtrl": 4,
                                  "MotorCtrl": (0 if args.pwm_enable_speed_rpm > 0 else 1),
                                  "ReqState": 1}))
        await asyncio.sleep(0.05)
        await send_torque(ws, 0.0)
        print(f"ZERO_CURRENT_READY n_max={args.n_max}rpm "
              f"arm={args.arm_speed_rpm}rpm", flush=True)

        started = time.monotonic()
        gate_ready_since = None
        ready = False
        seen_motion = False
        rest_since = None
        last_gate = None
        last_fault = None
        state = "zero"
        state_since = None
        pwm_enable_sent = args.pwm_enable_speed_rpm <= 0
        zeroed = False
        peak_speed = 0.0
        peak_current = 0.0
        minimum_udc = float("inf")
        maximum_udc = 0.0
        with log_path.open("w", encoding="utf-8") as log:
            while True:
                try:
                    message = json.loads(await asyncio.wait_for(ws.recv(), 1.0))
                except asyncio.TimeoutError:
                    if time.monotonic() - started >= args.timeout_s:
                        print("TELEMETRY_TIMEOUT", flush=True)
                        await send_torque(ws, 0.0)
                        break
                    continue
                if "MCU_stGateDrv" not in message:
                    if message.get("type") == "command_rejected":
                        print(f"COMMAND_REJECTED {message}", flush=True)
                    continue
                message["capture_epoch_us"] = time.time_ns() // 1000
                log.write(json.dumps(message, ensure_ascii=False) + "\n")
                log.flush()
                now = time.monotonic()
                gate = int(message["MCU_stGateDrv"])
                speed = float(message.get("ns", 0.0))
                current = math.hypot(float(message.get("MCU_Isd", 0.0)),
                                     float(message.get("MCU_Isq", 0.0)))
                udc = float(message.get("Udc", 0.0))
                fault = (int(message.get("McuFailCode", 0)),
                         int(message.get("McuSoftwareFault", 0)),
                         int(message.get("McuHardwareFault", 0)),
                         int(message.get("McuCANFault", 0)))
                peak_speed = max(peak_speed, abs(speed))
                peak_current = max(peak_current, current)
                if udc > 20:
                    minimum_udc = min(minimum_udc, udc)
                    maximum_udc = max(maximum_udc, udc)
                if gate != last_gate or fault != last_fault:
                    print(f"STATE gate={gate} rpm={speed:.0f} "
                          f"Id={message.get('MCU_Isd', 0):.1f} "
                          f"Iq={message.get('MCU_Isq', 0):.1f} "
                          f"Udc={udc:.0f} fault={fault}", flush=True)
                    last_gate, last_fault = gate, fault
                if (not pwm_enable_sent and speed <= -args.pwm_enable_speed_rpm
                        and 55.0 <= udc < args.abort_udc_v and not any(fault)):
                    await ws.send(json.dumps({"cmd": "SendControl", "En_Is": True,
                                              "Kl_15": False, "GearCtrl": 4,
                                              "MotorCtrl": 1, "ReqState": 1}))
                    pwm_enable_sent = True
                    print(f"PWM_ENABLE_REQUESTED rpm={speed:.0f}", flush=True)
                if gate == 2:
                    if gate_ready_since is None:
                        gate_ready_since = now
                    if not ready and now - gate_ready_since >= 0.5:
                        ready = True
                        print("READY_FOR_PEDAL", flush=True)
                else:
                    gate_ready_since = None
                if ready and abs(speed) >= 150:
                    if not seen_motion:
                        print(f"MOTION_SEEN rpm={speed:.0f}", flush=True)
                    seen_motion = True

                unsafe = (current >= args.abort_current_a or
                          abs(speed) >= args.abort_speed_rpm or
                          udc >= args.abort_udc_v or
                          (pwm_enable_sent and udc < 55.0) or
                          bool(message.get("DeratingOverVoltage", False)) or
                          fault[0] != 0 or fault[1] != 0 or fault[2] != 0 or
                          (ready and gate != 2))
                if unsafe and state != "abort":
                    state = "abort"
                    zeroed = True
                    await send_torque(ws, 0.0)
                    print(f"ABORT_ZERO rpm={speed:.0f} current={current:.1f} "
                          f"Udc={udc:.0f} gate={gate} fault={fault}", flush=True)

                if state == "zero" and ready and speed <= -args.arm_speed_rpm:
                    state = "one_amp"
                    state_since = now
                    await send_torque(ws, 1.0)
                    print(f"ARM_IQ=1A rpm={speed:.0f}", flush=True)
                elif state == "one_amp" and now - state_since >= 0.25:
                    state = "two_amp"
                    state_since = now
                    await send_torque(ws, 2.0)
                    print(f"ARM_IQ=2A rpm={speed:.0f}", flush=True)
                elif state == "two_amp" and now - state_since >= 1.5:
                    state = "done"
                    zeroed = True
                    await send_torque(ws, 0.0)
                    print(f"LOAD_STEP_DONE rpm={speed:.0f}", flush=True)

                if seen_motion and abs(speed) < 50:
                    if rest_since is None:
                        rest_since = now
                    elif now - rest_since >= 0.8:
                        print("SHAFT_STOPPED", flush=True)
                        break
                else:
                    rest_since = None
                if now - started >= args.timeout_s and not seen_motion:
                    print("NO_MOTION_TIMEOUT", flush=True)
                    break
                if now - started >= args.timeout_s and not zeroed:
                    zeroed = True
                    state = "abort"
                    await send_torque(ws, 0.0)
                    print("TIMEOUT_ZERO_WAITING_FOR_REST", flush=True)

            await ws.send(json.dumps({"cmd": "Stop"}))
            await asyncio.sleep(0.25)
            print(f"CAPTURE_DONE peak_rpm={peak_speed:.0f} "
                  f"peak_current_A={peak_current:.1f} "
                  f"Udc_range={minimum_udc:.0f}..{maximum_udc:.0f}V "
                  f"phase={state}", flush=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="ws://127.0.0.1:9000")
    parser.add_argument("--log", required=True)
    parser.add_argument("--n-max", type=float, default=2400.0)
    parser.add_argument("--arm-speed-rpm", type=float, default=1950.0)
    parser.add_argument("--pwm-enable-speed-rpm", type=float, default=0.0,
                        help="experimental late PWM enable; default enables PWM at rest")
    parser.add_argument("--abort-speed-rpm", type=float, default=2450.0)
    parser.add_argument("--abort-current-a", type=float, default=8.0)
    parser.add_argument("--abort-udc-v", type=float, default=63.0)
    parser.add_argument("--timeout-s", type=float, default=180.0)
    asyncio.run(main(parser.parse_args()))
