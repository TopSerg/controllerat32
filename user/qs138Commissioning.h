#pragma once

/*
 * Conservative first-start profile for the QS138 3000 W QSD138A-70.
 *
 * The manufacturer publishes the rated data, but not the phase resistance,
 * dq inductances, flux linkage or the fitted SIN/COS sensor pole count. The
 * values marked PROVISIONAL therefore have to be replaced after measurement.
 * Keeping them in one file prevents old MotorCAD data from another motor from
 * silently becoming the active profile.
 */

#define QS138_MOTOR_POLE_PAIRS                    (5U)

/* PROVISIONAL: initial low-current values; identify on the actual motor. */
#define QS138_PHASE_RESISTANCE_OHM                (0.0087F)
#define QS138_LD_UH                               (49.0F)
#define QS138_LQ_UH                               (62.0F)
#define QS138_LINE_LINE_EMF_CONSTANT              (0.180F)

/* PROVISIONAL: verify from a passive hand-rotation SIN/COS capture. */
#define QS138_RESOLVER_CYCLES_PER_MECH_REV        (5.0F)

/* Current-loop bandwidth is deliberately low for first commissioning. */
#define QS138_CURRENT_LOOP_BANDWIDTH_RAD_S        (500.0F)

/* Bench limits for the present 61 V / 10 A laboratory supply. */
#define QS138_MAX_PHASE_CURRENT_A                 (10.0F)
#define QS138_MAX_D_AXIS_CURRENT_A                (10.0F)
#define QS138_MAX_DC_DISCHARGE_CURRENT_A          (8.0F)
#define QS138_MAX_DC_CHARGE_CURRENT_A             (1.0F)
#define QS138_OVERCURRENT_TRIP_A                  (20U)
#define QS138_OVERVOLTAGE_TRIP_V                  (70U)

#define QS138_UDC_LOW_LIMIT_V                     (30)
#define QS138_UDC_LOW_DERATING_RANGE_V            (5)
#define QS138_UDC_HIGH_LIMIT_V                    (65)
#define QS138_UDC_HIGH_DERATING_RANGE_V           (3)

/* 1000 rpm, expressed in the rad/s units used by Control.Wmechanical. */
#define QS138_COMMISSIONING_MAX_SPEED_RAD_S       (104.719755F)
#define QS138_SPEED_DERATING_RANGE_RAD_S          (20)

/* A 0x300 current command is normally sent every 10 ms. */
#define QS138_CURRENT_COMMAND_TIMEOUT_MS          (100U)

