"""Run the existing StandRumoteUI zero-current test over WebSocket.

The server must already be running with WS_CAN_TRACE_FILE set. This program
sends the same commands as Start, Gear D, Id=Iq=0, Send in normal CAN mode.
It watches telemetry, then stops after a sustained PWMrun -> FreeWheel event.
"""

import argparse
import asyncio
import json
import math
import time

import websockets


async def main(url: str, timeout_s: int, abort_current_a: float,
               abort_speed_rad_s: float, target_valid_samples: int,
               wait_for_rest: bool, correction_rad: float | None,
               stop_after_motion: bool) -> None:
    async with websockets.connect(url, max_size=8_000_000) as ws:
        await ws.recv()  # signal catalog
        await ws.send(json.dumps({"cmd": "Init"}))
        await asyncio.sleep(0.5)
        await ws.send(json.dumps({"cmd": "SetJsonPeriod", "period_ms": 50}))
        if correction_rad is not None:
            await ws.send(json.dumps({"cmd": "SetResolverCorrection",
                                      "correction_rad": correction_rad}))
            await asyncio.sleep(0.25)
        await ws.send(json.dumps({"cmd": "SendLimits", "M_min": -10,
                                  "M_max": 10, "M_grad_max": 4000,
                                  "n_max": 1000}))
        await ws.send(json.dumps({"cmd": "SendControl", "En_Is": True,
                                  "Kl_15": False, "GearCtrl": 4,
                                  "MotorCtrl": 1, "ReqState": 1}))
        await ws.send(json.dumps({"cmd": "SendTorque", "En_Is": True,
                                  "Isd": 0.0, "Isq": 0.0}))
        print("ZERO_CURRENT_COMMAND_SENT", flush=True)

        start = time.monotonic()
        gate_two_since = None
        last_gate = None
        fall_since = None
        reported_ready = False
        valid_samples = []
        capture_complete = False
        rest_since = None
        correction_ack = False
        motion_seen = False
        while time.monotonic() - start < timeout_s:
            try:
                raw = await asyncio.wait_for(ws.recv(), timeout=1.0)
            except asyncio.TimeoutError:
                continue
            message = json.loads(raw)
            if "MCU_stGateDrv" not in message:
                continue
            now = time.monotonic()
            gate = int(message["MCU_stGateDrv"])
            speed = float(message.get("ns", 0))  # firmware sends rad/s
            current_d = float(message.get("MCU_Isd", 0))
            current_q = float(message.get("MCU_Isq", 0))
            current_magnitude = math.hypot(current_d, current_q)
            can_fault = int(message.get("McuCANFault", 0))
            if correction_rad is not None and not correction_ack:
                applied = float(message.get("ResolverAppliedCorrection", 0))
                if (int(message.get("ResolverCalibrationStatus", 0)) & 0x02 and
                        abs(applied - correction_rad) < 0.02):
                    print(f"CORRECTION_ACK={applied:.4f}rad", flush=True)
                    correction_ack = True
            if gate != last_gate:
                print(f"GATE={gate} speed={speed:.0f}rad/s "
                      f"({speed * 30 / math.pi:.0f}rpm) "
                      f"CANFault=0x{can_fault:02X}", flush=True)
                last_gate = gate
            if reported_ready and gate == 2 and (
                current_magnitude >= abort_current_a or
                abs(speed) >= abort_speed_rad_s
            ):
                print(f"ABORT_LIMIT speed={speed:.1f}rad/s "
                      f"Id={current_d:.1f}A Iq={current_q:.1f}A "
                      f"magnitude={current_magnitude:.1f}A", flush=True)
                break
            status = int(message.get("ResolverCalibrationStatus", 0))
            if gate == 2 and status & 0x01:
                valid_samples.append((speed,
                                      float(message.get("ResolverElectricalSpeed", 0)),
                                      float(message.get("ResolverFluxPositionError", 0)),
                                      current_magnitude))
                if len(valid_samples) == 1:
                    print("FLUX_SPEED_VALID", flush=True)
                if target_valid_samples and len(valid_samples) >= target_valid_samples \
                        and not capture_complete:
                    print(f"VALID_SAMPLES={len(valid_samples)} "
                          f"speed={speed:.1f}rad/s "
                          f"flux_error={valid_samples[-1][2]:.3f}rad "
                          f"current={current_magnitude:.1f}A", flush=True)
                    capture_complete = True
                    if not wait_for_rest:
                        break
            if stop_after_motion and reported_ready and gate == 2 \
                    and abs(speed) >= 18 and not motion_seen:
                motion_seen = True
                print(f"MOTION_SEEN speed={speed:.1f}rad/s", flush=True)
            if stop_after_motion and motion_seen:
                capture_complete = True
            if capture_complete and wait_for_rest and abs(speed) < 8:
                if rest_since is None:
                    rest_since = now
                elif now - rest_since >= 0.5:
                    print("SHAFT_SPEED_NEAR_ZERO", flush=True)
                    break
            else:
                rest_since = None
            if gate == 2:
                if gate_two_since is None:
                    gate_two_since = now
                fall_since = None
                if not reported_ready and now - gate_two_since >= 0.3:
                    print("READY_FOR_EXTERNAL_ROTATION", flush=True)
                    reported_ready = True
            elif reported_ready and gate == 1:
                if fall_since is None:
                    fall_since = now
                if now - fall_since >= 0.3:
                    print(f"SUSTAINED_FREEWHEEL speed={speed:.0f}rad/s "
                          f"({speed * 30 / math.pi:.0f}rpm)", flush=True)
                    await asyncio.sleep(1.0)
                    break
            else:
                gate_two_since = None
        else:
            print("CAPTURE_TIMEOUT", flush=True)

        if valid_samples:
            errors = sorted(x[2] for x in valid_samples)
            currents = sorted(x[3] for x in valid_samples)
            print(f"FLUX_VALID_COUNT={len(valid_samples)} "
                  f"median_error={errors[len(errors) // 2]:.4f}rad "
                  f"median_current={currents[len(currents) // 2]:.2f}A", flush=True)

        await ws.send(json.dumps({"cmd": "Stop"}))
        await asyncio.sleep(0.2)
        print("STOP_SENT", flush=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="ws://127.0.0.1:9000")
    parser.add_argument("--timeout-s", type=int, default=240)
    parser.add_argument("--abort-current-a", type=float, default=8.0)
    parser.add_argument("--abort-speed-rad-s", type=float, default=25.0)
    parser.add_argument("--target-valid-samples", type=int, default=0)
    parser.add_argument("--wait-for-rest", action="store_true")
    parser.add_argument("--correction-rad", type=float)
    parser.add_argument("--stop-after-motion", action="store_true")
    args = parser.parse_args()
    asyncio.run(main(args.url, args.timeout_s, args.abort_current_a,
                     args.abort_speed_rad_s, args.target_valid_samples,
                     args.wait_for_rest, args.correction_rad,
                     args.stop_after_motion))
