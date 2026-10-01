# QS138 / Gen1.1: CAN diagnosis and resolver calibration, 2026-09-30

Stand: left QS138 on Gen1.1 (`controllerat32`), right motor on Sintech,
StandRumoteUI through PCAN-USB FD at 1 Mbit/s. All test commands were
`Id=Iq=0` in normal CAN mode.

## Why PWM stopped near the reported speed 230

The old `GateDrv: 2 -> 1` event was followed by CAN `0x2C6` payload
`20 00 00 40 00 00 00 00`: `FaultLevel=1`, software fault bit 1
`OverCurrent`, hardware/sensor/CAN faults zero. CAN `0x300` continued at
about 13 ms intervals with zero Id/Iq commands, `Enable=1`, consecutive
counters and successful PCAN writes. The current-command timeout was not the
cause of this shutdown. Sampled dq currents reached about 19 A before the
trip; the firmware checks raw phase current against 20 A between telemetry
samples.

The QS138 setup used `resolverSwap=0`. The generated model then calls
`atan2(ResolverCosine, ResolverSine)`, which reverses the measured electrical
angle relative to the induced phase-current vector. A constant shift cannot
fix reversed rotation. Changing `resolverSwap` to 1 let the inverter remain
in PWMrun while the external drive accelerated it past the old trip point.

`McuActualSpeed` currently carries `Control.Wmechanical` in rad/s, although
StandRumoteUI labels that field as rpm. Convert displayed values by
`rpm = rad/s * 60 / (2*pi)`. The original displayed 230 corresponds to about
2200 rpm; the first captured trip at raw 20 corresponds to about 191 rpm.

## Electrical zero

Calibration is meaningful only while `GateDrv=2` and CAN `0x082` status bit 0
is set (`|Welectrical| > 300 rad/s`). The `Flux position error` is filtered;
wait for it to settle before interpreting it. Changing the correction in a
latched FreeWheel state does not validate the electrical zero.

| Runtime correction | Steady flux position error | Observation |
| --- | ---: | --- |
| 1.172 rad | about -0.504 rad | PWMrun, electrical speed over threshold |
| 1.676 rad | about -1.001 rad | PWMrun, 11 s of valid samples |
| 0.675 rad | -0.0018 to +0.0025 rad | PWMrun, 29 valid CAN samples, about 0.56 s |

The signed response to correction supports `resolverShift=0.675f`, now saved
with `resolverSwap=1` in `user/getBoardSettings.c` and flashed to device 240.
After flashing, passive CAN `0x082` reports default correction 0.6750 rad.
The firmware image SHA-256 is
`bf0a5faec09f775babdb6ce1337a436c91c1a4c3c7fce9a76abe9b043023d5d5`.

With the flashed default correction, a further zero-current run reached
raw speed -45 rad/s (about 430 rpm), remained in PWMrun, and measured at most
4.5 A dq-current magnitude. CAN `0x2C6` had no fault during that capture.

## Remaining operating issue

Automatic Stop while the external motor was still driving at about 100 to
124 rad/s was followed by a separate latched `OverCurrent`; the fault was
observed passively after the capture closed PCAN. Its exact phase-current
mechanism is not established. Let the external shaft stop before closing
normal CAN mode. The server now sends `0x046 RequestedState=Ready` and one
disabled `0x300` before closing PCAN; this avoids leaving a torque-control
request active and avoids sending several `0x300` counters in one MCU tick.
The last high-speed Stop fault predated the change to one disabled `0x300`.
A stationary test of the final Stop sequence showed three `0x046 Ready`
frames, one disabled `0x300`, `GateDrv=1`, and a zero `0x2C6` afterward.
The final Stop sequence has not been validated while the external motor is
still driving at high speed.

Motor resistance, inductances and back-EMF constant in
`user/qs138Commissioning.h` remain provisional. This calibration establishes
resolver direction and electrical zero for the measured direction; it does
not validate full-torque operation.

## Evidence files

- `C:/Project/StandRumoteUI/client/can_trace_live2_20260930.csv`: original
  PWMrun-to-FreeWheel OverCurrent with regular `0x300`.
- `C:/Project/StandRumoteUI/client/can_trace_swap1_fluxsteady_20260930.csv`:
  correction 1.172 rad, valid flux-error samples.
- `C:/Project/StandRumoteUI/client/can_trace_shift1676_20260930.csv`:
  correction 1.676 rad, settled flux-error plateau.
- `C:/Project/StandRumoteUI/client/can_trace_shift0675_20260930.csv`:
  correction 0.675 rad, near-zero flux-error samples.
- `C:/Project/StandRumoteUI/client/can_trace_final_default0675_20260930.csv`:
  low-speed confirmation after flashing the default correction.
- `C:/Project/StandRumoteUI/client/can_trace_stop_single300_20260930.csv`:
  final stationary Stop sequence.

`tools/analyze_can_transition.py` decodes the frame-level transition;
`tools/summarize_can_capture.py` prints currents, speeds, faults and command
cadence for any of these CSV traces.
