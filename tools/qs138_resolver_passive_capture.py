"""Log resolver telemetry during externally driven shaft rotation.

Only Init, SetJsonPeriod and Stop are sent to MarathonWS. This script never
enables Gen1.1 current control or sends a nonzero current command.
"""

import argparse
import asyncio
import json
import time
from pathlib import Path

import websockets


async def capture(args):
    output = Path(args.log)
    output.parent.mkdir(parents=True, exist_ok=True)
    sample_count = 0
    sine = []
    cosine = []
    speed = []
    gates = set()
    t0 = time.monotonic()
    async with websockets.connect(args.url, max_size=8_000_000) as ws:
        await ws.recv()  # signal catalog
        await ws.send(json.dumps({"cmd": "Init"}))
        await asyncio.sleep(0.5)
        await ws.send(json.dumps({"cmd": "SetJsonPeriod", "period_ms": 50}))
        print("PASSIVE_CAPTURE_READY; Gen1.1 current control disabled", flush=True)
        with output.open("w", encoding="utf-8") as log:
            try:
                while time.monotonic() - t0 < args.duration_s:
                    try:
                        row = json.loads(await asyncio.wait_for(ws.recv(), 1.0))
                    except asyncio.TimeoutError:
                        continue
                    if "MCU_stGateDrv" not in row:
                        continue
                    row["capture_epoch_us"] = time.time_ns() // 1000
                    log.write(json.dumps(row, ensure_ascii=False) + "\n")
                    log.flush()
                    sample_count += 1
                    sine.append(float(row.get("ResolverSine", 0)))
                    cosine.append(float(row.get("ResolverCosine", 0)))
                    speed.append(float(row.get("ns", 0)))
                    gate = int(row["MCU_stGateDrv"])
                    gates.add(gate)
                    if gate == 2:
                        print("ABORT: PWM active unexpectedly", flush=True)
                        break
                    if sample_count % 20 == 0:
                        print(f"SAMPLE rpm={speed[-1]:.0f} "
                              f"sin={sine[-1]:.0f} cos={cosine[-1]:.0f} "
                              f"angle={float(row.get('ResolverTheta', 0)):.3f} "
                              f"Udc={float(row.get('Udc', 0)):.1f}", flush=True)
            finally:
                await ws.send(json.dumps({"cmd": "Stop"}))
    if not sample_count:
        print("NO_TELEMETRY", flush=True)
        return
    print(f"DONE samples={sample_count} gate_states={sorted(gates)} "
          f"sin_range={min(sine):.0f}..{max(sine):.0f} "
          f"cos_range={min(cosine):.0f}..{max(cosine):.0f} "
          f"max_abs_rpm={max(map(abs, speed)):.0f}", flush=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="ws://127.0.0.1:9000")
    parser.add_argument("--log", required=True)
    parser.add_argument("--duration-s", type=float, default=45.0)
    asyncio.run(capture(parser.parse_args()))
