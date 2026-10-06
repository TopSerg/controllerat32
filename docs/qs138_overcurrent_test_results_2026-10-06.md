# QS138 over-current test results — 2026-10-06

Branch: `test/overcurrent-flight-recorder`. Tested firmware base: `de8e969f736b5228ef2f431f54ff58d3183324da` (`build/debug/AT32_Base.bin`, SHA-256 `A0A84AB159AB8C585E953683A29B32BDDC2C2C5867A85112570E87B031A4E925`). The procedure is in [qs138_next_test_overcurrent_2026-10-06.md](qs138_next_test_overcurrent_2026-10-06.md).

## Stage A — stationary commissioning

Before PWM, JTAG confirmed `g_resolverSignalsReady=1`, `g_qs138CurrentOffsetsFrozen=1`, `g_qs138FlightRecorderArmed=1`, `g_qs138FlightRecorderTriggered=0`, and `g_qs138FlightRecorderFrozen=0`. The shaft was stationary, the calibrated phase currents were near zero, and the DC bus was about 61 V. With `IdRef=IqRef=0`, `GateDrv=2` and no fault. The left motor made a steady audible hum while stationary; the supply remained in CV. The first stationary recording was stopped when motor power was switched off. Its [CAN log](../test_logs/qs138_de8e969_staged_can.csv) and [host telemetry](../test_logs/qs138_de8e969_staged_telemetry.jsonl) are retained.

## Stage B — first external-spin plateau

With left PWM active and both current references zero, the right motor began turning the common shaft toward 300 rpm. Measured current rose before the first plateau: host telemetry first saw `|Id|` or `|Iq|` above 8 A at about 124 rpm. The software `OverCurrent` protection then tripped near 413 rpm; CAN `0x2C6` reported its over-current bit and `GateDrv` went from 2 to 1. No later speed plateau or nonzero current command was attempted. The operator reported that the left motor appeared to accelerate; motor power was switched off after the fault.

JTAG captured the frozen recorder **before reset or fault clear**: [raw dump](../test_logs/qs138_de8e969_overcurrent_B_20261006.bin), [decoded samples](../test_logs/qs138_de8e969_overcurrent_B_20261006.csv), and [metadata](../test_logs/qs138_de8e969_overcurrent_B_20261006.json). The corresponding [raw CAN log](../test_logs/qs138_de8e969_retry_can.csv) and [host telemetry](../test_logs/qs138_de8e969_retry_telemetry.jsonl) cover the run. The recorder has 256 valid samples at 100 µs/sample and froze after the trigger and 16 post-trigger samples.

At the trigger, one measured phase current reached about −20.2 A with the protection threshold still 20 A. `Iq` was about +19.8 A while `IdRef=IqRef=0`; `Uq` was about −5.0 V and `Udc` about 59.6 V. The recorded PWM duties were about 0.54/0.46/0.52, far from the 0 or 1 limits. The rise from about 12.8 A to the trip happened in the recorder's roughly 24 ms pre-trigger window. These data show loss of zero-current regulation at low speed, without evidence of DC-voltage or PWM-duty saturation at the trip. They do not yet distinguish resolver angle/sign, phase wiring, current feedback, or controller dynamics as the root cause.

After the trip, the shaft speed continued rising while `GateDrv=1`; the log alone cannot attribute that motion solely to the left motor, because the right drive and shaft inertia were also involved. The host logged an overspeed stop near 2072 rpm. Further powered testing should wait for analysis of the low-speed current rise and the stationary hum.

## Test tooling

[`qs138_jtag.py`](../tools/qs138_jtag.py) reads the JTAG status and frozen recorder. [`qs138_overcurrent_operator.py`](../tools/qs138_overcurrent_operator.py) records host telemetry and commands. After this trip, the operator's non-fault 8 A guard was corrected to turn CONTROL off instead of merely commanding zero current. This correction has passed Python syntax checks but **has not been retested on hardware**. The firmware's `OverCurrent=20 A`, `DecouplingEnable`, and provisional QS138 parameters were not changed, and no merge into `main` was made.
