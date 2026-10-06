#pragma once

#include <stdint.h>
#include "ControlSystem_v2_types.h"

/*
 * QS138 commissioning-only safety helpers.
 *
 * This module deliberately does NOT change the 20 A over-current trip,
 * enable the generated decoupling path, or alter provisional motor
 * parameters.  It only makes the next bench test observable, prevents a hard
 * PWM pickup on a rotor that is already spinning, and provides a separately
 * bounded q-axis back-EMF feed-forward for the staged B2 test.
 */

#define QS138_FLIGHT_RECORDER_CAPACITY             (256U)
#define QS138_FLIGHT_RECORDER_POST_TRIGGER_SAMPLES (16U)
#define QS138_OFFSET_FREEZE_STABLE_SAMPLES         (2000U)
#define QS138_OFFSET_FREEZE_MAX_SPEED_RAD_S         (1.0F)
#define QS138_PWM_START_MAX_SPEED_RAD_S             (10.0F)

/* Stage B2 feed-forward.  This is intentionally separate from
 * SystemParameters.DecouplingEnable, which remains zero. */
#define QS138_BACK_EMF_FF_DEFAULT_GAIN              (0.75F)
#define QS138_BACK_EMF_FF_MAX_ABS_V                 (20.0F)
#define QS138_BACK_EMF_FF_MAX_UDC_FRACTION          (0.35F)

typedef struct {
    uint32_t sequence;
    uint16_t rawIa;
    uint16_t rawIb;
    uint16_t rawIc;
    uint16_t rawUdc;

    real32_T Ia;
    real32_T Ib;
    real32_T Ic;
    real32_T Id;
    real32_T Iq;
    real32_T IdRef;
    real32_T IqRef;
    real32_T Ud;
    real32_T Uq;
    real32_T UqPi;
    real32_T UqFeedForward;
    real32_T UmodRef;
    real32_T Udc;
    real32_T thetaElectrical;
    real32_T wElectrical;
    real32_T wMechanical;
    real32_T pwmTa;
    real32_T pwmTb;
    real32_T pwmTc;

    uint8_t modActive;
    uint8_t overCurrentInstant;
    uint8_t overCurrentLatchedBefore;
    uint8_t globalErrorBefore;
    uint8_t hardwareFaultInput;
} QS138FlightRecorderSample;

/* JTAG-visible recorder state.  The recorder is intentionally unarmed until
 * the current-sensor offsets have been accepted.  When
 * g_qs138FlightRecorderFrozen == 1 the array is stable and can be read
 * without racing the ISR. */
extern volatile QS138FlightRecorderSample
    g_qs138FlightRecorder[QS138_FLIGHT_RECORDER_CAPACITY];
extern volatile uint16_t g_qs138FlightRecorderWriteIndex;
extern volatile uint16_t g_qs138FlightRecorderTriggerIndex;
extern volatile uint16_t g_qs138FlightRecorderValidSamples;
extern volatile uint16_t g_qs138FlightRecorderPostRemaining;
extern volatile uint8_t g_qs138FlightRecorderTriggered;
extern volatile uint8_t g_qs138FlightRecorderFrozen;
extern volatile uint8_t g_qs138FlightRecorderArmed;
extern volatile uint32_t g_qs138FlightRecorderSequence;

/* JTAG-visible commissioning interlocks/status. */
extern volatile uint8_t g_qs138CurrentOffsetsFrozen;
extern volatile uint8_t g_qs138LatePwmEnableBlocked;
extern volatile real32_T g_qs138FrozenIaOffset;
extern volatile real32_T g_qs138FrozenIbOffset;
extern volatile real32_T g_qs138FrozenIcOffset;

/* JTAG-visible Stage B2 back-EMF feed-forward controls/diagnostics.
 * Enable/gain are intentionally writable in RAM for a guarded bench test. */
extern volatile uint8_t g_qs138BackEmfFeedForwardEnable;
extern volatile real32_T g_qs138BackEmfFeedForwardGain;
extern volatile real32_T g_qs138BackEmfFeedForwardRawV;
extern volatile real32_T g_qs138BackEmfFeedForwardCommandV;
extern volatile uint8_t g_qs138BackEmfFeedForwardLimited;

void QS138TestSafetyTick(const inSignals_st *io,
                         boolean_T overCurrentInstantaneous,
                         const Errors_st *errorBefore);
boolean_T QS138TestSafetyAllowControlStart(void);
void QS138TestSafetyResetFlightRecorder(void);
