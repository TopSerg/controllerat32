# QS138 next test: over-current / high-speed current-loop diagnosis

Date: 2026-10-06
Branch: `test/overcurrent-flight-recorder`

## Purpose

The first running test with PWM armed from standstill showed that the previous 1100 rpm event was not only a late-PWM pickup problem. With `IdRef = IqRef = 0`, measured q-axis current already exceeded 8 A at about 124 rpm and the unchanged 20 A software `OverCurrent` tripped near 413 rpm.

The frozen 10 kHz recorder showed that the q-axis PI was producing approximately the motor back-EMF voltage with the correct sign while the external drive accelerated the shaft very quickly. With the generated decoupling/feed-forward disabled, the PI can build that voltage only from current error. The measured current error was consistent with the voltage ramp required by the provisional QS138 back-EMF constant.

The next test therefore adds a **separate, bounded q-axis back-EMF feed-forward** without enabling the generated `DecouplingEnable` path.

This branch intentionally keeps:

- `QS138_OVERCURRENT_TRIP_A = 20 A` unchanged;
- `SystemParameters.DecouplingEnable = 0` unchanged;
- provisional QS138 Rs/Ld/Lq/EMF values unchanged;
- the existing current PI gains unchanged.

## Startup finding and recorder-arm fix

The first attempt with the flight recorder stopped in section A before PWM enable. Before current-sensor offset calibration completed, the raw converted currents were approximately 31-33 A while the shaft was stationary and PWM was off. That exceeded the unchanged 20 A instantaneous software threshold and froze the one-shot recorder on a startup calibration transient. After the current offsets were accepted, the measured phase currents returned close to zero.

The recorder is therefore deliberately **unarmed until current-offset calibration has completed**:

- `g_qs138FlightRecorderArmed == 0` while `g_qs138CurrentOffsetsFrozen == 0`;
- the 20 A protection itself remains active and unchanged during startup;
- when offsets become valid for the first time, recorder metadata/history are reset and `g_qs138FlightRecorderArmed` becomes `1`;
- only post-calibration instantaneous over-current may trigger/freeze the flight recorder.

This change affects diagnostics only. It does not weaken or raise the actual `OverCurrent` protection.

## Stage B2 back-EMF feed-forward

The B2 feed-forward is intentionally independent of the generated decoupling code.

Default values:

- `g_qs138BackEmfFeedForwardEnable = 1`;
- `g_qs138BackEmfFeedForwardGain = 0.75`;
- `Uq_ff_raw = gain * Control.Welectrical * Control.motorParams.motorEmf`;
- command magnitude is limited to `min(20 V, 0.35 * Udc)`;
- the contribution is applied through generated `VqPlay`, which is summed after `PID_IQ.Out`;
- it is active only while direct-current test mode is active, PWM is active, fixed-angle/voltage-control modes are off and there is no latched global fault.

The feed-forward does **not** alter `DecouplingEnable`, motor parameters or the 20 A protection.

JTAG-visible feed-forward state:

- `g_qs138BackEmfFeedForwardEnable`;
- `g_qs138BackEmfFeedForwardGain`;
- `g_qs138BackEmfFeedForwardRawV`;
- `g_qs138BackEmfFeedForwardCommandV`;
- `g_qs138BackEmfFeedForwardLimited`;
- `VqPlay`.

The flight recorder now stores both `UqPi` and `UqFeedForward`. Its legacy `Uq` column is recorded as their sum so the total q-axis voltage request remains directly visible. The sample size is now 96 bytes; the complete 256-sample buffer is 24576 bytes (`0x6000`). Use the updated `tools/qs138_jtag.py` and `tools/decode_qs138_flight_recorder.py` from the same branch.

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

Each recorder sample contains raw phase-current ADC counts, converted Ia/Ib/Ic, Id/Iq, active Id/Iq references, Ud, total Uq, Uq PI component, Uq back-EMF feed-forward component, UmodRef, Udc, electrical angle/speed, mechanical speed, last-applied Ta/Tb/Tc, modulation state and protection flags.

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
4. Confirm through JTAG before PWM:
   - `g_qs138BackEmfFeedForwardEnable == 1`;
   - `g_qs138BackEmfFeedForwardGain == 0.75`;
   - `g_qs138BackEmfFeedForwardCommandV == 0` or very close to zero;
   - `VqPlay == 0` or very close to zero.
5. Record the three frozen current offsets and DC-bus voltage. Verify phase currents are near zero after calibration.
6. Command CURRENT/CONTROL with `Id = 0 A`, `Iq = 0 A` while the shaft is still stopped.
7. Confirm PWM/control becomes active without a current step. If a phase current exceeds 8 A, or the drive trips, stop here and read the frozen flight recorder.

### B2. Slow external-spin test with bounded back-EMF feed-forward

Do **not** disable PWM between speed points. Keep `IdRef = 0 A`, `IqRef = 0 A` for the whole first sweep.

The previous run accelerated at roughly several thousand rpm/s. Do not repeat that ramp. For B2, command the external/right drive so the common shaft accelerates no faster than approximately **100-200 rpm/s**.

First use these low-speed plateaus:

- 100 rpm
- 200 rpm
- 300 rpm
- 400 rpm
- 500 rpm

Hold each point for at least 2 s. At each plateau verify:

- `|Id| < 2 A` preferred;
- `|Iq| < 2 A` preferred;
- no phase current above 8 A;
- `UqFeedForward` has the same sign as `Welectrical`;
- `UqPi` is substantially smaller in magnitude than in the previous no-FF run;
- `g_qs138BackEmfFeedForwardLimited == 0`;
- PWM duties remain away from 0/1 saturation;
- no resolver discontinuity or fault.

If 500 rpm is clean, continue slowly through:

- 600 rpm
- 900 rpm
- 1100 rpm
- 1300 rpm

If 1300 rpm is clean, continue only after reviewing current and voltage headroom at 1500, 1700 and 1900 rpm.

### B2 abort criteria

Stop the external drive and disable CONTROL immediately if any of the following occurs:

- any phase current reaches 8 A during this diagnostic run;
- `|Id|` or `|Iq|` reaches 5 A with zero references;
- current grows monotonically instead of settling at a plateau;
- `g_qs138BackEmfFeedForwardLimited` becomes 1 below the expected high-speed region;
- DC bus approaches the configured over-voltage limit;
- `UmodRef` approaches the available voltage limit / PWM saturation;
- resolver angle/speed becomes discontinuous;
- any software or hardware fault is raised.

Do not increase the feed-forward gain during a run. If `0.75` is clearly insufficient, stop, preserve logs and change only that one variable for the next run.

### C. Small current command only after zero-current B2 sweep is stable

At a speed that was stable in B2 (start at 600-900 rpm, not at the highest point):

1. Keep PWM continuously active.
2. Apply `Iq = +0.5 A` for 2 s, then return to 0 A.
3. Apply `Iq = +1.0 A` for 2 s, then return to 0 A.
4. Only if both are stable, apply `Iq = +2.0 A`.
5. Keep `Id = 0 A` for this test.
6. For each step compare IqRef/Iq, IdRef/Id, `UqPi`, `UqFeedForward`, total Uq, phase-current peaks and settling time.

Do not tune PI gains or enable generated decoupling during this run.

## If OverCurrent occurs

1. Send/hold disable; stop the external drive and let the shaft come to rest.
2. Do not raise the 20 A threshold.
3. Before clearing/resetting or reflashing, verify `g_qs138FlightRecorderFrozen == 1`.
4. Read `g_qs138FlightRecorder` and metadata through JTAG using the updated `tools/qs138_jtag.py`.
5. Preserve the raw array and decode it in sequence order around `triggerIndex` with the updated `tools/decode_qs138_flight_recorder.py`.
6. Compare at least the final 5 ms before trigger: phase-current slope, theta, speed, `UqPi`, `UqFeedForward`, total Uq, UmodRef and last-applied PWM duties.

## Expected B2 diagnostic outcome

If the no-feed-forward explanation is correct, a controlled 100-200 rpm/s ramp with `Kff = 0.75` should reduce the zero-reference q-axis current drastically compared with the previous run and should allow the shaft to pass the previous 413 rpm trip point without approaching 20 A.

If large current still develops while `UqFeedForward` has the expected sign and magnitude, the next priority becomes electrical-angle accuracy, phase/current-channel correspondence and the provisional motor parameters rather than increasing the protection threshold.
