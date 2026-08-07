/*
 * File: PIDs.c
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
#include "PIDs.h"
#include "rtwtypes.h"
#include "ControlSystem_v2_private.h"

/* Output and update for atomic system: '<S32>/PID_Id' */
void PID_Id(const PID_st *PID_in, PID_st *PID_out)
{
  real32_T Err;
  real32_T OutMax;
  real32_T OutMin;
  real32_T Product_lu;
  real32_T Sum_be;
  real32_T Ui;

  /* Sum: '<S37>/Sum' */
  Err = PID_in->Ref - PID_in->Fdb;

  /* Product: '<S43>/Product' */
  Product_lu = Err * PID_in->Kp;

  /* Product: '<S42>/err*Ki' */
  Sum_be = Err * PID_in->Ki;

  /* Product: '<S42>/Product1' */
  Ui = PID_in->SatErr * PID_in->Kc;

  /* Sum: '<S42>/Satt+' */
  Sum_be += Ui;

  /* SignalConversion generated from: '<S42>/Bus Selector' */
  Ui = PID_in->Ui;

  /* If: '<S44>/If' */
  if (PID_in->defaultBit) {
    /* Outputs for IfAction SubSystem: '<S44>/Ui=0' incorporates:
     *  ActionPort: '<S45>/Action Port'
     */
    /* SignalConversion generated from: '<S45>/In1' incorporates:
     *  Constant: '<S44>/UiInit'
     */
    Ui = 0.0F;

    /* End of Outputs for SubSystem: '<S44>/Ui=0' */
  }

  /* End of If: '<S44>/If' */

  /* Sum: '<S42>/Ui+' */
  Ui += Sum_be;

  /* Sum: '<S38>/OutPreSat' */
  Sum_be = Product_lu + Ui;

  /* SignalConversion generated from: '<S36>/Bus Selector' */
  OutMax = PID_in->OutMax;

  /* SignalConversion generated from: '<S36>/Bus Selector' */
  OutMin = PID_in->OutMin;

  /* If: '<S36>/If' incorporates:
   *  RelationalOperator: '<S36>/OutPreSat > OutMax'
   *  RelationalOperator: '<S36>/OutPreSat<OutMin'
   */
  if (Sum_be > OutMax) {
    /* Outputs for IfAction SubSystem: '<S36>/If Action Subsystem' incorporates:
     *  ActionPort: '<S39>/Action Port'
     */
    /* SignalConversion generated from: '<S39>/In1' */
    OutMin = OutMax;

    /* End of Outputs for SubSystem: '<S36>/If Action Subsystem' */
  } else if (Sum_be >= OutMin) {
    /* Outputs for IfAction SubSystem: '<S36>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S40>/Action Port'
     */
    /* SignalConversion generated from: '<S40>/In1' */
    OutMin = Sum_be;

    /* End of Outputs for SubSystem: '<S36>/If Action Subsystem1' */
  } else {
    /* no actions */
  }

  /* End of If: '<S36>/If' */

  /* BusAssignment: '<S35>/Bus Assignment' incorporates:
   *  Sum: '<S36>/Sum'
   */
  *PID_out = *PID_in;
  PID_out->SatErr = OutMin - Sum_be;
  PID_out->Out = OutMin;
  PID_out->Ui = Ui;
  PID_out->OutPreSat = Sum_be;
  PID_out->Up = Product_lu;
  PID_out->err = Err;
}

/* Output and update for atomic system: '<S33>/PID_Iq' */
void PID_Iq(const PID_st *PID_in, PID_st *PID_out)
{
  real32_T Err;
  real32_T OutMax;
  real32_T OutMin;
  real32_T Product_lt;
  real32_T Sum_e;
  real32_T Ui;

  /* Sum: '<S49>/Sum' */
  Err = PID_in->Ref - PID_in->Fdb;

  /* Product: '<S55>/Product' */
  Product_lt = Err * PID_in->Kp;

  /* Product: '<S54>/err*Ki' */
  Sum_e = Err * PID_in->Ki;

  /* Product: '<S54>/Product1' */
  Ui = PID_in->SatErr * PID_in->Kc;

  /* Sum: '<S54>/Satt+' */
  Sum_e += Ui;

  /* SignalConversion generated from: '<S54>/Bus Selector' */
  Ui = PID_in->Ui;

  /* If: '<S56>/If' */
  if (PID_in->defaultBit) {
    /* Outputs for IfAction SubSystem: '<S56>/Ui=0' incorporates:
     *  ActionPort: '<S57>/Action Port'
     */
    /* SignalConversion generated from: '<S57>/In1' incorporates:
     *  Constant: '<S56>/UiInit'
     */
    Ui = 0.0F;

    /* End of Outputs for SubSystem: '<S56>/Ui=0' */
  }

  /* End of If: '<S56>/If' */

  /* Sum: '<S54>/Ui+' */
  Ui += Sum_e;

  /* Sum: '<S50>/OutPreSat' */
  Sum_e = Product_lt + Ui;

  /* SignalConversion generated from: '<S48>/Bus Selector' */
  OutMax = PID_in->OutMax;

  /* SignalConversion generated from: '<S48>/Bus Selector' */
  OutMin = PID_in->OutMin;

  /* If: '<S48>/If' incorporates:
   *  RelationalOperator: '<S48>/OutPreSat > OutMax'
   *  RelationalOperator: '<S48>/OutPreSat<OutMin'
   */
  if (Sum_e > OutMax) {
    /* Outputs for IfAction SubSystem: '<S48>/If Action Subsystem' incorporates:
     *  ActionPort: '<S51>/Action Port'
     */
    /* SignalConversion generated from: '<S51>/In1' */
    OutMin = OutMax;

    /* End of Outputs for SubSystem: '<S48>/If Action Subsystem' */
  } else if (Sum_e >= OutMin) {
    /* Outputs for IfAction SubSystem: '<S48>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S52>/Action Port'
     */
    /* SignalConversion generated from: '<S52>/In1' */
    OutMin = Sum_e;

    /* End of Outputs for SubSystem: '<S48>/If Action Subsystem1' */
  } else {
    /* no actions */
  }

  /* End of If: '<S48>/If' */

  /* BusAssignment: '<S47>/Bus Assignment' incorporates:
   *  Sum: '<S48>/Sum'
   */
  *PID_out = *PID_in;
  PID_out->SatErr = OutMin - Sum_e;
  PID_out->Out = OutMin;
  PID_out->Ui = Ui;
  PID_out->OutPreSat = Sum_e;
  PID_out->Up = Product_lt;
  PID_out->err = Err;
}

/* Output and update for atomic system: '<S23>/PID_fw' */
void PID_fw(const PID_st *PID_in, PID_st *PID_out)
{
  real32_T Err;
  real32_T OutMax;
  real32_T OutMin;
  real32_T Product_ji;
  real32_T Sum_c;
  real32_T Ui;

  /* Sum: '<S63>/Sum' */
  Err = PID_in->Ref - PID_in->Fdb;

  /* Product: '<S69>/Product' */
  Product_ji = Err * PID_in->Kp;

  /* Product: '<S68>/err*Ki' */
  Sum_c = Err * PID_in->Ki;

  /* Product: '<S68>/Product1' */
  Ui = PID_in->SatErr * PID_in->Kc;

  /* Sum: '<S68>/Satt+' */
  Sum_c += Ui;

  /* SignalConversion generated from: '<S68>/Bus Selector' */
  Ui = PID_in->Ui;

  /* If: '<S70>/If' */
  if (PID_in->defaultBit) {
    /* Outputs for IfAction SubSystem: '<S70>/Ui=0' incorporates:
     *  ActionPort: '<S71>/Action Port'
     */
    /* SignalConversion generated from: '<S71>/In1' incorporates:
     *  Constant: '<S70>/UiInit'
     */
    Ui = 0.0F;

    /* End of Outputs for SubSystem: '<S70>/Ui=0' */
  }

  /* End of If: '<S70>/If' */

  /* Sum: '<S68>/Ui+' */
  Ui += Sum_c;

  /* Sum: '<S64>/OutPreSat' */
  Sum_c = Product_ji + Ui;

  /* SignalConversion generated from: '<S62>/Bus Selector' */
  OutMax = PID_in->OutMax;

  /* SignalConversion generated from: '<S62>/Bus Selector' */
  OutMin = PID_in->OutMin;

  /* If: '<S62>/If' incorporates:
   *  RelationalOperator: '<S62>/OutPreSat > OutMax'
   *  RelationalOperator: '<S62>/OutPreSat<OutMin'
   */
  if (Sum_c > OutMax) {
    /* Outputs for IfAction SubSystem: '<S62>/If Action Subsystem' incorporates:
     *  ActionPort: '<S65>/Action Port'
     */
    /* SignalConversion generated from: '<S65>/In1' */
    OutMin = OutMax;

    /* End of Outputs for SubSystem: '<S62>/If Action Subsystem' */
  } else if (Sum_c >= OutMin) {
    /* Outputs for IfAction SubSystem: '<S62>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S66>/Action Port'
     */
    /* SignalConversion generated from: '<S66>/In1' */
    OutMin = Sum_c;

    /* End of Outputs for SubSystem: '<S62>/If Action Subsystem1' */
  } else {
    /* no actions */
  }

  /* End of If: '<S62>/If' */

  /* BusAssignment: '<S61>/Bus Assignment' incorporates:
   *  Sum: '<S62>/Sum'
   */
  *PID_out = *PID_in;
  PID_out->SatErr = OutMin - Sum_c;
  PID_out->Out = OutMin;
  PID_out->Ui = Ui;
  PID_out->OutPreSat = Sum_c;
  PID_out->Up = Product_ji;
  PID_out->err = Err;
}

/* Output and update for atomic system: '<S111>/PID_Idc' */
void PID_Idc(const PID_st *PID_in, PID_st *PID_out)
{
  real32_T Err;
  real32_T OutMax;
  real32_T OutMin;
  real32_T Product_h3;
  real32_T Sum_i;
  real32_T Ui;

  /* Sum: '<S122>/Sum' */
  Err = PID_in->Ref - PID_in->Fdb;

  /* Product: '<S128>/Product' */
  Product_h3 = Err * PID_in->Kp;

  /* Product: '<S127>/err*Ki' */
  Sum_i = Err * PID_in->Ki;

  /* Product: '<S127>/Product1' */
  Ui = PID_in->SatErr * PID_in->Kc;

  /* Sum: '<S127>/Satt+' */
  Sum_i += Ui;

  /* SignalConversion generated from: '<S127>/Bus Selector' */
  Ui = PID_in->Ui;

  /* If: '<S129>/If' */
  if (PID_in->defaultBit) {
    /* Outputs for IfAction SubSystem: '<S129>/Ui=0' incorporates:
     *  ActionPort: '<S130>/Action Port'
     */
    /* SignalConversion generated from: '<S130>/In1' incorporates:
     *  Constant: '<S129>/UiInit'
     */
    Ui = 0.0F;

    /* End of Outputs for SubSystem: '<S129>/Ui=0' */
  }

  /* End of If: '<S129>/If' */

  /* Sum: '<S127>/Ui+' */
  Ui += Sum_i;

  /* Sum: '<S123>/OutPreSat' */
  Sum_i = Product_h3 + Ui;

  /* SignalConversion generated from: '<S121>/Bus Selector' */
  OutMax = PID_in->OutMax;

  /* SignalConversion generated from: '<S121>/Bus Selector' */
  OutMin = PID_in->OutMin;

  /* If: '<S121>/If' incorporates:
   *  RelationalOperator: '<S121>/OutPreSat > OutMax'
   *  RelationalOperator: '<S121>/OutPreSat<OutMin'
   */
  if (Sum_i > OutMax) {
    /* Outputs for IfAction SubSystem: '<S121>/If Action Subsystem' incorporates:
     *  ActionPort: '<S124>/Action Port'
     */
    /* SignalConversion generated from: '<S124>/In1' */
    OutMin = OutMax;

    /* End of Outputs for SubSystem: '<S121>/If Action Subsystem' */
  } else if (Sum_i >= OutMin) {
    /* Outputs for IfAction SubSystem: '<S121>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S125>/Action Port'
     */
    /* SignalConversion generated from: '<S125>/In1' */
    OutMin = Sum_i;

    /* End of Outputs for SubSystem: '<S121>/If Action Subsystem1' */
  } else {
    /* no actions */
  }

  /* End of If: '<S121>/If' */

  /* BusAssignment: '<S120>/Bus Assignment' incorporates:
   *  Sum: '<S121>/Sum'
   */
  *PID_out = *PID_in;
  PID_out->SatErr = OutMin - Sum_i;
  PID_out->Out = OutMin;
  PID_out->Ui = Ui;
  PID_out->OutPreSat = Sum_i;
  PID_out->Up = Product_h3;
  PID_out->err = Err;
}

/* Output and update for atomic system: '<S342>/PID_trq' */
void PID_trq(const PID_st *PID_in, PID_st *PID_out)
{
  real32_T Err;
  real32_T OutMax;
  real32_T OutMin;
  real32_T Product_m;
  real32_T Sum_e;
  real32_T Ui;

  /* Sum: '<S347>/Sum' */
  Err = PID_in->Ref - PID_in->Fdb;

  /* Product: '<S353>/Product' */
  Product_m = Err * PID_in->Kp;

  /* Product: '<S352>/err*Ki' */
  Sum_e = Err * PID_in->Ki;

  /* Product: '<S352>/Product1' */
  Ui = PID_in->SatErr * PID_in->Kc;

  /* Sum: '<S352>/Satt+' */
  Sum_e += Ui;

  /* SignalConversion generated from: '<S352>/Bus Selector' */
  Ui = PID_in->Ui;

  /* If: '<S354>/If' */
  if (PID_in->defaultBit) {
    /* Outputs for IfAction SubSystem: '<S354>/Ui=0' incorporates:
     *  ActionPort: '<S355>/Action Port'
     */
    /* SignalConversion generated from: '<S355>/In1' incorporates:
     *  Constant: '<S354>/UiInit'
     */
    Ui = 0.0F;

    /* End of Outputs for SubSystem: '<S354>/Ui=0' */
  }

  /* End of If: '<S354>/If' */

  /* Sum: '<S352>/Ui+' */
  Ui += Sum_e;

  /* Sum: '<S348>/OutPreSat' */
  Sum_e = Product_m + Ui;

  /* SignalConversion generated from: '<S346>/Bus Selector' */
  OutMax = PID_in->OutMax;

  /* SignalConversion generated from: '<S346>/Bus Selector' */
  OutMin = PID_in->OutMin;

  /* If: '<S346>/If' incorporates:
   *  RelationalOperator: '<S346>/OutPreSat > OutMax'
   *  RelationalOperator: '<S346>/OutPreSat<OutMin'
   */
  if (Sum_e > OutMax) {
    /* Outputs for IfAction SubSystem: '<S346>/If Action Subsystem' incorporates:
     *  ActionPort: '<S349>/Action Port'
     */
    /* SignalConversion generated from: '<S349>/In1' */
    OutMin = OutMax;

    /* End of Outputs for SubSystem: '<S346>/If Action Subsystem' */
  } else if (Sum_e >= OutMin) {
    /* Outputs for IfAction SubSystem: '<S346>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S350>/Action Port'
     */
    /* SignalConversion generated from: '<S350>/In1' */
    OutMin = Sum_e;

    /* End of Outputs for SubSystem: '<S346>/If Action Subsystem1' */
  } else {
    /* no actions */
  }

  /* End of If: '<S346>/If' */

  /* BusAssignment: '<S345>/Bus Assignment' incorporates:
   *  Sum: '<S346>/Sum'
   */
  *PID_out = *PID_in;
  PID_out->SatErr = OutMin - Sum_e;
  PID_out->Out = OutMin;
  PID_out->Ui = Ui;
  PID_out->OutPreSat = Sum_e;
  PID_out->Up = Product_m;
  PID_out->err = Err;
}

/* Output and update for atomic system: '<S322>/PID_spd' */
void PID_spd_d(const PID_st *PID_in, PID_st *PID_out)
{
  real32_T Err;
  real32_T OutMax;
  real32_T OutMin;
  real32_T Product_m;
  real32_T Sum_j;
  real32_T Ui;

  /* Sum: '<S329>/Sum' */
  Err = PID_in->Ref - PID_in->Fdb;

  /* Product: '<S335>/Product' */
  Product_m = Err * PID_in->Kp;

  /* Product: '<S334>/err*Ki' */
  Sum_j = Err * PID_in->Ki;

  /* Product: '<S334>/Product1' */
  Ui = PID_in->SatErr * PID_in->Kc;

  /* Sum: '<S334>/Satt+' */
  Sum_j += Ui;

  /* SignalConversion generated from: '<S334>/Bus Selector' */
  Ui = PID_in->Ui;

  /* If: '<S336>/If' */
  if (PID_in->defaultBit) {
    /* Outputs for IfAction SubSystem: '<S336>/Ui=0' incorporates:
     *  ActionPort: '<S337>/Action Port'
     */
    /* SignalConversion generated from: '<S337>/In1' incorporates:
     *  Constant: '<S336>/UiInit'
     */
    Ui = 0.0F;

    /* End of Outputs for SubSystem: '<S336>/Ui=0' */
  }

  /* End of If: '<S336>/If' */

  /* Sum: '<S334>/Ui+' */
  Ui += Sum_j;

  /* Sum: '<S330>/OutPreSat' */
  Sum_j = Product_m + Ui;

  /* SignalConversion generated from: '<S328>/Bus Selector' */
  OutMax = PID_in->OutMax;

  /* SignalConversion generated from: '<S328>/Bus Selector' */
  OutMin = PID_in->OutMin;

  /* If: '<S328>/If' incorporates:
   *  RelationalOperator: '<S328>/OutPreSat > OutMax'
   *  RelationalOperator: '<S328>/OutPreSat<OutMin'
   */
  if (Sum_j > OutMax) {
    /* Outputs for IfAction SubSystem: '<S328>/If Action Subsystem' incorporates:
     *  ActionPort: '<S331>/Action Port'
     */
    /* SignalConversion generated from: '<S331>/In1' */
    OutMin = OutMax;

    /* End of Outputs for SubSystem: '<S328>/If Action Subsystem' */
  } else if (Sum_j >= OutMin) {
    /* Outputs for IfAction SubSystem: '<S328>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S332>/Action Port'
     */
    /* SignalConversion generated from: '<S332>/In1' */
    OutMin = Sum_j;

    /* End of Outputs for SubSystem: '<S328>/If Action Subsystem1' */
  } else {
    /* no actions */
  }

  /* End of If: '<S328>/If' */

  /* BusAssignment: '<S325>/Bus Assignment' incorporates:
   *  Sum: '<S328>/Sum'
   */
  *PID_out = *PID_in;
  PID_out->SatErr = OutMin - Sum_j;
  PID_out->Out = OutMin;
  PID_out->Ui = Ui;
  PID_out->OutPreSat = Sum_j;
  PID_out->Up = Product_m;
  PID_out->err = Err;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
