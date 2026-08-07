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

/*
 * Output and update for atomic system: '<S153>/Protections'
 * Block description for: '<S153>/Protections'
 *   Функция обрабатывает входные сигналы и формирует статус защит
 *   преобразователя и мотора
 */
void Protections(const inSignals_st *IO, const Errors_st *Error_st, boolean_T
                 ClrErr, Errors_st *Error)
{
  boolean_T OR_be;
  boolean_T OR_e;
  boolean_T OR_k;
  boolean_T OR_l;

  /* Logic: '<S160>/OR' incorporates:
   *  Abs: '<S160>/Abs'
   *  Abs: '<S160>/Abs1'
   *  Abs: '<S160>/Abs2'
   *  DataStoreRead: '<S160>/Data Store Read'
   *  RelationalOperator: '<S160>/Relational Operator'
   *  RelationalOperator: '<S160>/Relational Operator1'
   *  RelationalOperator: '<S160>/Relational Operator2'
   */
  OR_k = ((platform_abs(IO->Ia) > (real32_T)TripLevels.OverCurrent_level) ||
          (platform_abs(IO->Ib) > (real32_T)TripLevels.OverCurrent_level) ||
          (platform_abs(IO->Ic) > (real32_T)TripLevels.OverCurrent_level) ||
          Error_st->OverCurrent);

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
    /* SignalConversion generated from: '<S155>/In1' incorporates:
     *  BusAssignment: '<S154>/Bus Assignment'
     *  DataTypeConversion: '<S154>/Data Type Conversion'
     *  Logic: '<S154>/OR'
     */
    Error->ClearErrors = false;
    Error->GlobalError = (OR_k || OR_l || OR_be || OR_e || (IO->Fault != 0U));
    Error->OverCurrent = OR_k;
    Error->OverVoltage = OR_l;
    Error->OverTemperature_power = OR_e;
    Error->OverTemperature_motor = OR_be;
    Error->HardWareFault = (IO->Fault != 0U);

    /* End of Outputs for SubSystem: '<S154>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S154>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S156>/Action Port'
     */
    /* SignalConversion generated from: '<S156>/In1' incorporates:
     *  BusAssignment: '<S154>/Bus Assignment1'
     *  Constant: '<S154>/Constant'
     */
    Error->ClearErrors = false;
    Error->GlobalError = false;
    Error->OverCurrent = false;
    Error->OverVoltage = false;
    Error->OverTemperature_power = false;
    Error->OverTemperature_motor = false;
    Error->HardWareFault = false;

    /* End of Outputs for SubSystem: '<S154>/If Action Subsystem1' */
  }

  /* End of If: '<S154>/If' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
