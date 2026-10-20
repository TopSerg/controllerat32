"""Open the existing stand GUI connected to this workstation's stand server."""

import sys
from pathlib import Path


CLIENT_DIR = Path(r"C:\Project\StandRumoteUI\client")
sys.path.insert(0, str(CLIENT_DIR))

import app  # noqa: E402


app.DEFAULT_WS_ADDR = "10.55.123.215:9000"
app.main()
