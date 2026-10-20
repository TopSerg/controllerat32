"""Run short, logged braking-current steps after pedal speed has stabilized.

The right drive is operated by the bench operator. This client holds the last
step until the operator releases the pedal and speed falls below the release
threshold. Every exit path requests zero current and disables CAN control.
"""

import argparse
import asyncio
from collections import deque
import json
import math
import time
from pathlib import Path

import websockets


async def capture(args):
    steps = [float(part) for part in args.steps.split(",")]
    if not steps or any(not math.isfinite(x) or x <= 0 or x > 10 for x in steps):
        raise ValueError("steps must contain positive finite currents <= 10 A")
    if steps != sorted(set(steps)):
        raise ValueError("steps must be strictly increasing")
    path = Path(args.log)
    path.parent.mkdir(parents=True, exist_ok=True)
    async with websockets.connect(args.url, max_size=8_000_000) as ws:
        await ws.recv()  # signal catalog
        await ws.send(json.dumps({"cmd": "Init"}))
        await asyncio.sleep(0.7)
        await ws.send(json.dumps({"cmd": "SetJsonPeriod", "period_ms": 50}))
        for command in (
            {"cmd": "SendLimits", "M_min": -10, "M_max": 10,
             "M_grad_max": 4000, "n_max": args.n_max},
            {"cmd": "SendControl", "En_Is": True, "Kl_15": False,
             "GearCtrl": 4, "MotorCtrl": 1, "ReqState": 1},
            {"cmd": "SendTorque", "En_Is": True, "Isd": 0.0, "Isq": 0.0},
        ):
            await ws.send(json.dumps(command))
            await asyncio.sleep(0.05)

        def print_state(row):
            print(f"STATE gate={row.get('MCU_stGateDrv')} "
                  f"rpm={float(row.get('ns', 0)):.0f} "
                  f"Iq={float(row.get('MCU_Isq', 0)):.1f}A "
                  f"Udc={float(row.get('Udc', 0)):.1f}V "
                  f"faults=({row.get('McuFailCode')},"
                  f"{row.get('McuSoftwareFault')},"
                  f"{row.get('McuHardwareFault')},"
                  f"{row.get('McuCANFault')})", flush=True)

        async def set_iq(current):
            await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                      "Isd": 0.0, "Isq": current}))

        start = time.monotonic()
        last_telemetry = start
        ready_since = None
        speed_window = deque()
        stop_since = None
        step_index = -1
        step_since = None
        profile_done_since = None
        released = False
        aborted = False
        verified = False
        samples = 0
        peak_speed = 0.0
        peak_current = 0.0
        report_at = start
        last_state = None
        ready_announced = False
        print(f"ARMED_ZERO steps={steps} speed_window="
              f"{args.arm_min_rpm}-{args.arm_max_rpm}rpm", flush=True)
        with path.open("w", encoding="utf-8") as log:
            try:
                while True:
                    try:
                        row = json.loads(await asyncio.wait_for(ws.recv(), 1.0))
                    except asyncio.TimeoutError:
                        if time.monotonic() - last_telemetry > 3.0:
                            aborted = True
                            print("ABORT_NO_TELEMETRY", flush=True)
                            break
                        continue
                    if "MCU_stGateDrv" not in row:
                        if row.get("type") == "command_rejected":
                            aborted = True
                            print(f"ABORT_COMMAND_REJECTED {row}", flush=True)
                            break
                        continue
                    now = time.monotonic()
                    last_telemetry = now
                    row["capture_epoch_us"] = time.time_ns() // 1000
                    log.write(json.dumps(row, ensure_ascii=False) + "\n")
                    log.flush()
                    samples += 1
                    speed = float(row.get("ns", 0.0))
                    udc = float(row.get("Udc", 0.0))
                    current = math.hypot(float(row.get("MCU_Isd", 0.0)),
                                         float(row.get("MCU_Isq", 0.0)))
                    gate = int(row.get("MCU_stGateDrv", 0))
                    faults = tuple(int(row.get(key, 0)) for key in
                                   ("McuFailCode", "McuSoftwareFault",
                                    "McuHardwareFault", "McuCANFault",
                                    "McuSensorFault"))
                    peak_speed = max(peak_speed, abs(speed))
                    peak_current = max(peak_current, current)
                    state = (gate, faults)
                    if state != last_state:
                        print_state(row)
                        last_state = state
                    if gate == 2 and ready_since is None:
                        ready_since = now
                    elif gate != 2:
                        ready_since = None
                    ready = ready_since is not None and now - ready_since >= 0.5
                    if ready and not ready_announced:
                        print("READY_FOR_PEDAL", flush=True)
                        ready_announced = True
                    unsafe = (current >= args.abort_current_a or
                              abs(speed) >= args.abort_speed_rpm or
                              udc >= args.abort_udc_v or
                              (ready and udc < 50.0) or
                              any(faults) or
                              bool(row.get("DeratingOverVoltage", False)) or
                              bool(row.get("HostOverspeed", False)) or
                              (step_index >= 0 and gate != 2))
                    if unsafe and not released:
                        aborted = True
                        released = True
                        await set_iq(0.0)
                        print(f"ABORT_ZERO rpm={speed:.0f} current={current:.1f}A "
                              f"Udc={udc:.1f}V faults={faults}", flush=True)
                    if not released and step_index < 0 and ready:
                        in_window = -args.arm_max_rpm <= speed <= -args.arm_min_rpm
                        if in_window:
                            speed_window.append((now, speed))
                            while speed_window and now - speed_window[0][0] > args.speed_stable_s:
                                speed_window.popleft()
                            speeds = [value for _, value in speed_window]
                            long_enough = (speed_window and
                                           now - speed_window[0][0] >=
                                           args.speed_stable_s - 0.1)
                            if (long_enough and
                                    max(speeds) - min(speeds) <=
                                    args.stable_span_rpm):
                                step_index = 0
                                step_since = now
                                await set_iq(steps[0])
                                print(f"STEP_IQ={steps[0]:.1f}A rpm={speed:.0f} "
                                      f"stable_span={max(speeds)-min(speeds):.0f}rpm",
                                      flush=True)
                        else:
                            speed_window.clear()
                    if not released and step_index >= 0:
                        if speed > -args.release_speed_rpm:
                            released = True
                            await set_iq(0.0)
                            print(f"RELEASE_ZERO rpm={speed:.0f}", flush=True)
                        elif (profile_done_since is None and
                              now - step_since >= args.step_hold_s):
                            if step_index + 1 < len(steps):
                                step_index += 1
                                step_since = now
                                await set_iq(steps[step_index])
                                print(f"STEP_IQ={steps[step_index]:.1f}A "
                                      f"rpm={speed:.0f}", flush=True)
                            else:
                                profile_done_since = now
                                print("PROFILE_DONE_RELEASE_PEDAL", flush=True)
                        elif (profile_done_since is not None and
                              now - profile_done_since >= args.max_final_hold_s):
                            released = True
                            await set_iq(0.0)
                            print("FINAL_HOLD_TIMEOUT_ZERO", flush=True)
                    if not released and now - start >= args.max_total_s:
                        released = True
                        await set_iq(0.0)
                        print("TOTAL_TIMEOUT_ZERO", flush=True)
                    if released and abs(speed) <= args.stop_speed_rpm and current <= args.stop_current_a:
                        if stop_since is None:
                            stop_since = now
                        elif now - stop_since >= args.stop_stable_s:
                            verified = True
                            print(f"VERIFIED_STOP rpm={speed:.0f} current={current:.1f}A",
                                  flush=True)
                            break
                    else:
                        stop_since = None
                    if released and now - report_at >= 5.0:
                        print(f"WAITING_FOR_STOP rpm={speed:.0f} "
                              f"current={current:.1f}A", flush=True)
                        report_at = now
            finally:
                try:
                    await set_iq(0.0)
                    await asyncio.sleep(0.1)
                    await ws.send(json.dumps({"cmd": "Stop"}))
                except Exception as exc:
                    print(f"ZERO_OR_STOP_SEND_FAILED {exc}", flush=True)
        print(f"CAPTURE_DONE samples={samples} peak_rpm={peak_speed:.0f} "
              f"peak_phase_A={peak_current:.1f} aborted={aborted} "
              f"verified_stop={verified}", flush=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="ws://127.0.0.1:9000")
    parser.add_argument("--log", required=True)
    parser.add_argument("--steps", default="1,3,5")
    parser.add_argument("--step-hold-s", type=float, default=3.0)
    parser.add_argument("--arm-min-rpm", type=float, default=500.0)
    parser.add_argument("--arm-max-rpm", type=float, default=700.0)
    parser.add_argument("--speed-stable-s", type=float, default=2.5)
    parser.add_argument("--stable-span-rpm", type=float, default=60.0)
    parser.add_argument("--release-speed-rpm", type=float, default=350.0)
    parser.add_argument("--n-max", type=float, default=800.0)
    parser.add_argument("--abort-current-a", type=float, default=8.0)
    parser.add_argument("--abort-speed-rpm", type=float, default=850.0)
    parser.add_argument("--abort-udc-v", type=float, default=62.5)
    parser.add_argument("--max-final-hold-s", type=float, default=12.0)
    parser.add_argument("--max-total-s", type=float, default=90.0)
    parser.add_argument("--stop-speed-rpm", type=float, default=25.0)
    parser.add_argument("--stop-current-a", type=float, default=2.0)
    parser.add_argument("--stop-stable-s", type=float, default=2.0)
    args = parser.parse_args()
    asyncio.run(capture(args))
