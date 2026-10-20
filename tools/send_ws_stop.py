"""Send an immediate Stop command to the local StandRumoteUI server."""

import asyncio
import json
import websockets


async def main():
    async with websockets.connect("ws://127.0.0.1:9000") as ws:
        await ws.recv()
        await ws.send(json.dumps({"cmd": "Stop"}))
        await asyncio.sleep(0.2)
        print("STOP_SENT")


asyncio.run(main())
