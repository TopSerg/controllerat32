/*
 * File: Protections.c
 *
 * Code generated for Simulink model 'ControlSystem_v2'.
 *
 * Model version                  : 5.217
 * Simulink Coder version         : 9.8 (R2022b) 13-May-2022
 * C/C++ source code generated on : Mon Apr 21 10:17:07 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: STMicroelectronics->ST10/Super10
 * Code generation objectives:
 *    1. MISRA C:2012 guidelines
 *    2. Execution efficiency
 * Validation result: Not run
 */

#include "ControlSystem_v2.h"
#include "ControlSystem_v2_types.h"
#include "rtwtypes.h"
#include "Protections.h"
#include "platform_math.h"
#include "ControlSystem_v2_private.h"
#include "qs138TestSafety.h"
#include "adcInit.h"

/* Current offsets are owned by user/main.c. */
extern float IaOffset;
extern float IbOffset;
extern float IcOffset;
extern volatile uint8_t g_resolverSignalsReady;

volatile QS138FlightRecorderSample
  g_qs138FlightRecorder[QS138_FLIGHT_RECORDER_CAPACITY];
volatile uint16_t g_qs138FlightRecorderWriteIndex = 0U;
volatile uint16_t g_qs138FlightRecorderTriggerIndex = 0xFFFFU;
volatile uint16_t g_qs138FlightRecorderValidSamples = 0U;
volatile uint16_t g_qs138FlightRecorderPostRemaining = 0U;
volatile uint8_t g_qs138FlightRecorderTriggered = 0U;
volatile uint8_t g_qs138FlightRecorderFrozen = 0U;
volatile uint8_t g_qs138FlightRecorderArmed = 0U;
volatile uint32_t g_qs138FlightRecorderSequence = 0U;

volatile uint8_t g_qs138CurrentOffsetsFrozen = 0U;
volatile uint8_t g_qs138LatePwmEnableBlocked = 0U;
volatile real32_T g_qs138FrozenIaOffset = 0.0F;
volatile real32_T g_qs138FrozenIbOffset = 0.0F;
volatile real32_T g_qs138FrozenIcOffset = 0.0F;
static uint16_t g_qs138OffsetStableSamples = 0U;

static void QS138TestSafetyUpdateCurrentOffsets(void)
{
  const boolean_T pwmInactive = !Control.stat.mod_Active;
  const boolean_T rotorStationary =
    (platform_abs(Control.Wmechanical) <=
     QS138_OFFSET_FREEZE_MAX_SPEED_RAD_S);

  if (!pwmInactive) {
    return;
  }

  if (g_qs138CurrentOffsetsFrozen == 0U) {
    /* main.c keeps adapting the offsets while PWM is off.  Wait until the
     * resolver is live and the rotor has been stationary long enough, then
     * accept the converged values as the commissioning baseline. */
    if ((g_resolverSignalsReady != 0U) && rotorStationary) {
      if (g_qs138OffsetStableSamples < QS138_OFFSET_FREEZE_STABLE_SAMPLES) {
        g_qs138OffsetStableSamples++;
      }
      if (g_qs138OffsetStableSamples >= QS138_OFFSET_FREEZE_STABLE_SAMPLES) {
        g_qs138FrozenIaOffset = IaOffset;
        g_qs138FrozenIbOffset = IbOffset;
        g_qs138FrozenIcOffset = IcOffset;
        g_qs138CurrentOffsetsFrozen = 1U;
      }
    } else {
      g_qs138OffsetStableSamples = 0U;
    }
    return;
  }

  if (rotorStationary) {
    /* It is safe to let the legacy zero estimator refine its baseline while
     * the rotor is genuinely stationary. */
    g_qs138FrozenIaOffset = IaOffset;
    g_qs138FrozenIbOffset = IbOffset;
    g_qs138FrozenIcOffset = IcOffset;
  } else {
    /* Never let back-EMF/noise observed during a PWM-off coast become a new
     * current-sensor zero.  main.c may have adjusted the values earlier in
     * this ISR; restore the last stationary baseline for the next sample. */
    IaOffset = g_qs138FrozenIaOffset;
    IbOffset = g_qs138FrozenIbOffset;
    IcOffset = g_qs138FrozenIcOffset;
  }
}

void QS138TestSafetyResetFlightRecorder(void)
{
  g_qs138FlightRecorderWriteIndex = 0U;
  g_qs138FlightRecorderTriggerIndex = 0xFFFFU;
  g_qs138FlightRecorderValidSamples = 0U;
  g_qs138FlightRecorderPostRemaining = 0U;
  g_qs138FlightRecorderTriggered = 0U;
  g_qs138FlightRecorderFrozen = 0U;
  g_qs138FlightRecorderSequence = 0U;
  g_qs138FlightRecorderArmed =
    (g_qs138CurrentOffsetsFrozen != 0U) ? 1U : 0U;
}

boolean_T QS138TestSafetyAllowControlStart(void)
{
  const boolean_T resolverReady = (g_resolverSignalsReady != 0U);
  const boolean_T offsetsReady = (g_qs138CurrentOffsetsFrozen != 0U);
  const boolean_T speedSafe =
    (platform_abs(Control.Wmechanical) <= QS138_PWM_START_MAX_SPEED_RAD_S);

  if (resolverReady && offsetsReady && speedSafe) {
    g_qs138LatePwmEnableBlocked = 0U;
    return true;
  }

  g_qs138LatePwmEnableBlocked = 1U;
  return false;
}

void QS138TestSafetyTick(const inSignals_st *io,
                         boolean_T overCurrentInstantaneous,
                         const Errors_st *errorBefore)
{
  uint16_t idx;
  uint8_t justTriggered = 0U;
  volatile QS138FlightRecorderSample *sample;

  QS138TestSafetyUpdateCurrentOffsets();

  /* The current sensors report a large apparent phase current while their
   * startup offsets are still converging.  The 20 A protection remains fully
   * active, but those pre-calibration samples are not useful flight-recorder
   * triggers.  Arm the recorder only after the stationary offset baseline has
   * been accepted. */
  if (g_qs138CurrentOffsetsFrozen == 0U) {
    g_qs138FlightRecorderArmed = 0U;
    return;
  }

  /* First sample after offset acceptance starts a clean ring history.  This
   * guarantees that a startup transient cannot consume the one-shot buffer
   * needed for the rotating test. */
  if (g_qs138FlightRecorderArmed == 0U) {
    QS138TestSafetyResetFlightRecorder();
    return;
  }

  if (g_qs138FlightRecorderFrozen != 0U) {
    return;
  }

  idx = g_qs138FlightRecorderWriteIndex;
  sample = &g_qs138FlightRecorder[idx];

  sample->sequence = g_qs138FlightRecorderSequence++;
  sample->rawIa = (uint16_t)adc_preempt_value[IA_CH];
  sample->rawIb = (uint16_t)adc_preempt_value[IB_CH];
  sample->rawIc = (uint16_t)adc_preempt_value[IC_CH];
  sample->rawUdc = (uint16_t)adc_preempt_value[UDC_CH];

  sample->Ia = io->Ia;
  sample->Ib = io->Ib;
  sample->Ic = io->Ic;
  sample->Id = Control.Id;
  sample->Iq = Control.Iq;
  sample->IdRef = Control.IdRefReg;
  sample->IqRef = Control.IqRefReg;
  sample->Ud = Control.Ud;
  sample->Uq = Control.Uq;
  sample->UmodRef = Control.UmodRef;
  sample->Udc = io->Vdc;
  sample->thetaElectrical = Control.ThetaElectrical;
  sample->wElectrical = Control.Welectrical;
  sample->wMechanical = Control.Wmechanical;

  /* PWMcalc runs after the control/protection section.  These are therefore
   * the duties that were physically applied during the ADC sample, i.e. the
   * previous control-cycle result, which is exactly what is useful around a
   * fast over-current event. */
  sample->pwmTa = SVPWM.Ta;
  sample->pwmTb = SVPWM.Tb;
  sample->pwmTc = SVPWM.Tc;

  sample->modActive = Control.stat.mod_Active ? 1U : 0U;
  sample->overCurrentInstant = overCurrentInstantaneous ? 1U : 0U;
  sample->overCurrentLatchedBefore = errorBefore->OverCurrent ? 1U : 0U;
  sample->globalErrorBefore = errorBefore->GlobalError ? 1U : 0U;
  sample->hardwareFaultInput = (io->Fault != 0U) ? 1U : 0U;

  if ((g_qs138FlightRecorderTriggered == 0U) &&
      overCurrentInstantaneous) {
    g_qs138FlightRecorderTriggered = 1U;
    g_qs138FlightRecorderTriggerIndex = idx;
    g_qs138FlightRecorderPostRemaining =
      QS138_FLIGHT_RECORDER_POST_TRIGGER_SAMPLES;
    justTriggered = 1U;
  }

  idx++;
  if (idx >= QS138_FLIGHT_RECORDER_CAPACITY) {
    idx = 0U;
  }
  g_qs138FlightRecorderWriteIndex = idx;

  if (g_qs138FlightRecorderValidSamples < QS138_FLIGHT_RECORDER_CAPACITY) {
    g_qs138FlightRecorderValidSamples++;
  }

  if ((g_qs138FlightRecorderTriggered != 0U) && (justTriggered == 0U)) {
    if (g_qs138FlightRecorderPostRemaining > 0U) {
      g_qs138FlightRecorderPostRemaining--;
    }
    if (g_qs138FlightRecorderPostRemaining == 0U) {
      g_qs138FlightRecorderFrozen = 1U;
    }
  }
}

/*
 * Output and update for atomic system: '<S153>/Protections'
 * Block description for: '<S153>/Protections'
 *   Function processes input signals and forms converter/motor protection state.
 */
void Protections(const inSignals_st *IO, const Errors_st *Error_st, boolean_T
                 ClrErr, Errors_st *Error)
{
  boolean_T OR_be;
  boolean_T OR_e;
  boolean_T OR_k;
  boolean_T OR_l;
  boolean_T overCurrentInstantaneous;

  /* The commissioning trip is intentionally based on instantaneous phase
   * current, not Id/Iq command.  Keep this protection unchanged. */
  overCurrentInstantaneous =
    ((platform_abs(IO->Ia) > (real32_T)TripLevels.OverCurrent_level) ||
     (platform_abs(IO->Ib) > (real32_T)TripLevels.OverCurrent_level) ||
     (platform_abs(IO->Ic) > (real32_T)TripLevels.OverCurrent_level));

  OR_k = (overCurrentInstantaneous || Error_st->OverCurrent);

  /* Capture every 10 kHz protection/control sample after current-offset
   * calibration and freeze around the first instantaneous over-current. */
  QS138TestSafetyTick(IO, overCurrentInstantaneous, Error_st);

  /* Logic: '<S157>/OR' incorporates:
   *  DataStoreRead: '<S157>/Data Store Read'
   *  RelationalOperator: '<S157>/Relational Operator'
   */
  OR_l = ((IO->Vdc > (real32_T)TripLevels.OverVoltage_level) ||
          Error_st->OverVoltage);

  /* Logic: '<S158>/OR' incorporates:
   *  DataStoreRead: '<S158>/Data Store Read'
   *  RelationalOperator: '<S158>/Relational Operator'
   */
  OR_be = ((IO->Tmotor > (real32_T)TripLevels.OverTemp_motor_level) ||
           Error_st->OverTemperature_motor);

  /* Logic: '<S159>/OR' incorporates:
   *  DataStoreRead: '<S159>/Data Store Read'
   *  MinMax: '<S159>/Max'
   *  RelationalOperator: '<S159>/Relational Operator'
   */
  OR_e = ((platform_max(platform_max(IO->Tigbt1, IO->Tigbt3), IO->Tigbt2) >
           (real32_T)TripLevels.OverTemp_power_level) ||
          Error_st->OverTemperature_power);

  /* If: '<S154>/If' */
  if (!ClrErr) {
    /* Outputs for IfAction SubSystem: '<S154>/If Action Subsystem' incorporates:
     *  ActionPort: '<S155>/Action Port'
     */
    Error->ClearErrors = false;
    Error->GlobalError = (OR_k || OR_l || OR_be || OR_e || (IO->Fault != 0U));
    Error->OverCurrent = OR_k;
    Error->OverVoltage = OR_l;
    Error->OverTemperature_power = OR_e;
    Error->OverTemperature_motor = OR_be;
    Error->HardWareFault = (IO->Fault != 0U);
  } else {
    /* Outputs for IfAction SubSystem: '<S154>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S156>/Action Port'
     */
    Error->ClearErrors = false;
    Error->GlobalError = false;
    Error->OverCurrent = false;
    Error->OverVoltage = false;
    Error->OverTemperature_power = false;
    Error->OverTemperature_motor = false;
    Error->HardWareFault = false;
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
