# QS138 next test: over-current / high-speed current-loop diagnosis

Date: 2026-10-06
Branch: `test/overcurrent-flight-recorder`

## Purpose

The previous test tripped software `OverCurrent` at about 1100 rpm when PWM was enabled only after the external motor had already accelerated the QS138. `Iq = 2 A` had not yet been commanded. This test avoids that hard pickup and records the fast event at the 10 kHz control/protection rate.

This branch intentionally keeps:

- `QS138_OVERCURRENT_TRIP_A = 20 A` unchanged;
- `DecouplingEnable = 0` unchanged;
- provisional QS138 Rs/Ld/Lq/EMF values unchanged.

Changing those at the same time would hide the original behavior.

## Startup finding and recorder-arm fix

The first attempt with the flight recorder stopped in section A before PWM enable. Before current-sensor offset calibration completed, the raw converted currents were approximately 31-33 A while the shaft was stationary and PWM was off. That exceeded the unchanged 20 A instantaneous software threshold and froze the one-shot recorder on a startup calibration transient. After the current offsets were accepted, the measured phase currents returned close to zero.

The recorder is therefore now deliberately **unarmed until current-offset calibration has completed**:

- `g_qs138FlightRecorderArmed == 0` while `g_qs138CurrentOffsetsFrozen == 0`;
- the 20 A protection itself remains active and unchanged during startup;
- when offsets become valid for the first time, recorder metadata/history are reset and `g_qs138FlightRecorderArmed` becomes `1`;
- only post-calibration instantaneous over-current may trigger/freeze the flight recorder.

This change affects diagnostics only. It does not weaken or raise the actual `OverCurrent` protection.

## Added commissioning protections

1. A 256-sample flight recorder is written on every protection/control sample after current-offset calibration (10 kHz, 100 us/sample).
2. The first post-calibration instantaneous phase-current trip is the trigger. Sixteen samples are retained after the trigger, then the buffer freezes.
3. A new transition from READY to CONTROL is blocked while the rotor speed magnitude is above 10 rad/s (~95 rpm), while resolver data is not ready, or before stationary current-offset calibration has been accepted.
4. Current-sensor offsets are accepted only after resolver data is live and the rotor has remained below 1 rad/s for 2000 control samples (~0.2 s). Once accepted, PWM-off rotation cannot train the offsets; the last stationary baseline is restored.

## JTAG symbols

Recorder metadata:

- `g_qs138FlightRecorder`
- `g_qs138FlightRecorderWriteIndex`
- `g_qs138FlightRecorderTriggerIndex`
- `g_qs138FlightRecorderValidSamples`
- `g_qs138FlightRecorderTriggered`
- `g_qs138FlightRecorderFrozen`
- `g_qs138FlightRecorderArmed`
- `g_qs138FlightRecorderSequence`

Safety status:

- `g_qs138CurrentOffsetsFrozen` -- must become 1 before ARM/CONTROL.
- `g_qs138FlightRecorderArmed` -- must become 1 after offsets are accepted and before the rotating test.
- `g_qs138LatePwmEnableBlocked` -- becomes 1 when a new CONTROL start is rejected by the commissioning interlock.
- `g_qs138FrozenIaOffset`, `g_qs138FrozenIbOffset`, `g_qs138FrozenIcOffset`.

Each recorder sample contains raw phase-current ADC counts, converted Ia/Ib/Ic, Id/Iq, active Id/Iq references, Ud/Uq, UmodRef, Udc, electrical angle/speed, mechanical speed, last-applied Ta/Tb/Tc, modulation state and protection flags.

When the recorder is full/frozen, `writeIndex` points to the next slot. If `validSamples == 256`, the oldest sample is at `writeIndex`; the trigger itself is at `triggerIndex`.

## Test procedure

### A. Stationary commissioning check

1. Both motors mechanically stopped. External/right drive disabled.
2. Power-cycle the inverter/controller with JTAG connected.
3. Keep all current commands disabled and wait until:
   - `g_resolverSignalsReady == 1`;
   - `g_qs138CurrentOffsetsFrozen == 1`;
   - `g_qs138FlightRecorderArmed == 1`;
   - `g_qs138FlightRecorderTriggered == 0`;
   - `g_qs138FlightRecorderFrozen == 0`;
   - `abs(Control.Wmechanical) < 1 rad/s`;
   - no persistent global/hardware/software fault is present after startup initialization.
4. Record the three frozen current offsets and DC-bus voltage. Verify phase currents are near zero after calibration.
5. Command CURRENT/CONTROL with `Id = 0 A`, `Iq = 0 A` while the shaft is still stopped.
6. Confirm PWM/control becomes active without a current step. If a phase current exceeds 10 A, or the drive trips, stop here and read the frozen flight recorder.

### B. External-spin test with PWM continuously active

Do **not** disable PWM between speed points. Keep `IdRef = 0 A`, `IqRef = 0 A` for the whole first sweep.

Ramp the external/right motor slowly through these plateaus:

- 300 rpm
- 600 rpm
- 900 rpm
- 1100 rpm
- 1300 rpm

Hold each point for at least 2 s and log:

- Ia/Ib/Ic;
- Id/Iq and IdRef/IqRef;
- Ud/Uq and UmodRef;
- Udc;
- theta electrical, electrical/mechanical speed;
- FluxPositionError;
- PWM/modulation active state;
- all fault bits.

Abort the sweep immediately if any of the following occurs:

- any phase current reaches 12 A or shows a rapidly increasing oscillation;
- `|Id|` or `|Iq|` unexpectedly exceeds 8 A with zero references;
- DC bus approaches its configured over-voltage limit;
- `UmodRef` approaches the available voltage limit / PWM saturation;
- resolver angle/speed becomes discontinuous;
- any software or hardware fault is raised.

If 1300 rpm is clean, continue in smaller steps (1500, 1700, 1900 rpm). Do not proceed toward 2200 rpm until the 1900 rpm point is stable and voltage headroom remains.

### C. Small current command only after zero-current sweep is stable

At a speed that was stable in section B (start at 600-900 rpm, not at the highest point):

1. Keep PWM continuously active.
2. Apply `Iq = +0.5 A` for 2 s, then return to 0 A.
3. Apply `Iq = +1.0 A` for 2 s, then return to 0 A.
4. Only if both are stable, apply `Iq = +2.0 A`.
5. Keep `Id = 0 A` for this test.
6. For each step compare IqRef/Iq, IdRef/Id, Ud/Uq, phase-current peaks and settling time.

Do not tune PI gains or enable decoupling during this run. The purpose is to distinguish a running-start transient from the original high-speed regulation problem.

## If OverCurrent occurs

1. Send/hold disable; stop the external drive and let the shaft come to rest.
2. Do not raise the 20 A threshold.
3. Before clearing/resetting or reflashing, verify `g_qs138FlightRecorderFrozen == 1`.
4. Read `g_qs138FlightRecorder` and metadata through JTAG.
5. Preserve the raw array and decode it in sequence order around `triggerIndex` with `tools/decode_qs138_flight_recorder.py`.
6. The key comparison is the 2-5 samples before the trigger versus the trigger sample: phase-current slope, theta, speed, Uq/Ud, UmodRef and last-applied PWM duties.

## Expected diagnostic outcome

- If the previous trip was caused by hard connection of a spinning PMSM to near-zero inverter voltage, section B should pass 1100 rpm when PWM has been active since standstill.
- If current still becomes unstable at a repeatable speed with PWM continuously active, the next focus is voltage headroom, q-axis back-EMF feed-forward/decoupling, electrical-angle accuracy and current-loop tuning.
