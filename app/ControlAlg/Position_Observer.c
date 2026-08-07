/*
 * File: Position_Observer.c
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
#include "Position_Observer.h"
#include "platform_math.h"
#include "rtwtypes.h"
#include "ControlSystem_v2_private.h"

/* Output and update for atomic system: '<S10>/Position_observer' */
void Position_Observer(const Observer_st *Observer_in, Observer_st *Observer_out)
{
  real32_T Add;
  real32_T Add2;
  real32_T Add_g1;
  real32_T L1_w_calc;
  real32_T Product1_ma;
  real32_T Product1_nd;
  real32_T Switch1_d;
  real32_T Switch1_ko;
  boolean_T LowerRelop1;

  /* Sum: '<S166>/Add' */
  Add = Observer_in->SensorAngle_est - Observer_in->SensorAngle_raw;

  /* Switch: '<S171>/Switch' incorporates:
   *  Bias: '<S171>/Bias'
   */
  if (Add > 3.14159274F) {
    Add -= 6.28318548F;
  }

  /* End of Switch: '<S171>/Switch' */

  /* Switch: '<S171>/Switch1' incorporates:
   *  Bias: '<S171>/Bias1'
   */
  if (Add < -3.14159274F) {
    Add += 6.28318548F;
  }

  /* End of Switch: '<S171>/Switch1' */

  /* Switch: '<S172>/Switch' incorporates:
   *  Bias: '<S172>/Bias'
   */
  if (Add > 3.14159274F) {
    Add -= 6.28318548F;
  }

  /* End of Switch: '<S172>/Switch' */

  /* Switch: '<S172>/Switch1' incorporates:
   *  Bias: '<S172>/Bias1'
   */
  if (Add < -3.14159274F) {
    Add += 6.28318548F;
  }

  /* End of Switch: '<S172>/Switch1' */

  /* Abs: '<S167>/Abs' */
  Add_g1 = platform_abs(Observer_in->SensorSpeed_est);

  /* Sum: '<S167>/Add' incorporates:
   *  Constant: '<S167>/Constant1'
   *  Product: '<S167>/Product'
   */
  Add_g1 = Observer_in->L0 - (Add_g1 * 2.30769238E-5F);

  /* RelationalOperator: '<S173>/LowerRelop1' */
  LowerRelop1 = (Add_g1 > Observer_in->L0);

  /* Switch: '<S173>/Switch2' incorporates:
   *  Constant: '<S167>/Constant'
   *  RelationalOperator: '<S173>/UpperRelop'
   *  Switch: '<S173>/Switch'
   */
  if (LowerRelop1) {
    Add_g1 = Observer_in->L0;
  } else if (Add_g1 < 0.8F) {
    /* Switch: '<S173>/Switch' incorporates:
     *  Constant: '<S167>/Constant'
     */
    Add_g1 = 0.8F;
  } else {
    /* no actions */
  }

  /* End of Switch: '<S173>/Switch2' */

  /* Fcn: '<S167>/L1_w_calc' */
  L1_w_calc = (2.0F * Add_g1) - 2.0F;

  /* Fcn: '<S165>/Angle_calcutation' */
  Switch1_ko = (Observer_in->SensorSpeed_est * Observer_in->Ts) + (Add *
    L1_w_calc);

  /* Sum: '<S162>/Add2' incorporates:
   *  Product: '<S162>/Product'
   */
  Add2 = Switch1_ko + Observer_in->SensorAngle_est;

  /* Switch: '<S168>/Switch' incorporates:
   *  Bias: '<S168>/Bias'
   */
  if (Add2 > 6.28318548F) {
    Add2 -= 6.28318548F;
  }

  /* End of Switch: '<S168>/Switch' */

  /* Fcn: '<S167>/L2_w_calc' */
  Add_g1 = ((2.0F * Add_g1) - 1.0F - (Add_g1 * Add_g1)) / Observer_in->Ts;

  /* Fcn: '<S165>/Speed_calculation' */
  Add = Observer_in->SensorSpeed_est + (Add_g1 * Add);

  /* Product: '<S163>/Product' */
  Switch1_d = Observer_in->SensorToMechanical * Switch1_ko;

  /* Sum: '<S163>/Add2' */
  Switch1_d += Observer_in->MechanicalAngle_est;

  /* Switch: '<S169>/Switch' incorporates:
   *  Bias: '<S169>/Bias'
   */
  if (Switch1_d > 6.28318548F) {
    Switch1_d -= 6.28318548F;
  }

  /* End of Switch: '<S169>/Switch' */

  /* Product: '<S163>/Product1' */
  Product1_ma = Observer_in->SensorToMechanical * Add;

  /* Product: '<S161>/Product' */
  Product1_nd = Observer_in->SensorToMechanical *
    Observer_in->MechanicalToElectrical;

  /* Sum: '<S164>/Add2' incorporates:
   *  Product: '<S164>/Product'
   */
  Switch1_ko = (Product1_nd * Switch1_ko) + Observer_in->ElectricalAngle_est;

  /* Switch: '<S170>/Switch' incorporates:
   *  Bias: '<S170>/Bias'
   */
  if (Switch1_ko > 6.28318548F) {
    Switch1_ko -= 6.28318548F;
  }

  /* End of Switch: '<S170>/Switch' */

  /* BusAssignment: '<S161>/Bus Assignment' */
  *Observer_out = *Observer_in;

  /* Switch: '<S168>/Switch1' */
  if (Add2 >= 0.0F) {
    /* BusAssignment: '<S161>/Bus Assignment' */
    Observer_out->SensorAngle_est = Add2;
  } else {
    /* BusAssignment: '<S161>/Bus Assignment' incorporates:
     *  Bias: '<S168>/Bias1'
     */
    Observer_out->SensorAngle_est = Add2 + 6.28318548F;
  }

  /* End of Switch: '<S168>/Switch1' */

  /* BusAssignment: '<S161>/Bus Assignment' incorporates:
   *  Product: '<S162>/Product1'
   */
  Observer_out->SensorSpeed_est = Add;

  /* Switch: '<S169>/Switch1' */
  if (Switch1_d >= 0.0F) {
    /* BusAssignment: '<S161>/Bus Assignment' */
    Observer_out->MechanicalAngle_est = Switch1_d;
  } else {
    /* BusAssignment: '<S161>/Bus Assignment' incorporates:
     *  Bias: '<S169>/Bias1'
     */
    Observer_out->MechanicalAngle_est = Switch1_d + 6.28318548F;
  }

  /* End of Switch: '<S169>/Switch1' */

  /* BusAssignment: '<S161>/Bus Assignment' */
  Observer_out->MechanicalSpeed_est = Product1_ma;

  /* Switch: '<S170>/Switch1' */
  if (Switch1_ko >= 0.0F) {
    /* BusAssignment: '<S161>/Bus Assignment' */
    Observer_out->ElectricalAngle_est = Switch1_ko;
  } else {
    /* BusAssignment: '<S161>/Bus Assignment' incorporates:
     *  Bias: '<S170>/Bias1'
     */
    Observer_out->ElectricalAngle_est = Switch1_ko + 6.28318548F;
  }

  /* End of Switch: '<S170>/Switch1' */

  /* BusAssignment: '<S161>/Bus Assignment' incorporates:
   *  Product: '<S164>/Product1'
   */
  Observer_out->ElectricalSpeed_est = Product1_nd * Add;
  Observer_out->L1_w = L1_w_calc;
  Observer_out->L2_w = Add_g1;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
