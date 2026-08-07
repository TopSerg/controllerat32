/*
 * File: PWMcalc.c
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
#include "rtwtypes.h"
#include "PWMcalc.h"
#include "ControlSystem_v2_types.h"
#include "ControlSystem_v2_private.h"

/*
 * Output and update for atomic system: '<S1>/PWMcalc'
 * Block description for: '<S1>/PWMcalc'
 *   Функция формирует задания для компараторов ШИМ по алгоритму Векторного
 *   ШИМ.
 */
void PWMcalc(real32_T Ualpha_n, real32_T Ubeta_c, real32_T Udc, boolean_T
             modActive, real32_T *Ta, real32_T *Tb, real32_T *Tc)
{
  PWM_st PWM_ctrl;
  real32_T Add;
  real32_T Gain_l5;
  real32_T U_beta_normalized;
  real32_T Ualpha_normalized;
  real32_T Y_b;
  real32_T Z;
  int16_T Sum;

  /* BusAssignment: '<S5>/Bus Assignment' incorporates:
   *  DataStoreRead: '<S5>/Data Store Read'
   */
  PWM_ctrl = SVPWM;
  PWM_ctrl.Ualpha = Ualpha_n;
  PWM_ctrl.Ubeta = Ubeta_c;
  PWM_ctrl.Udc = Udc;

  /* Product: '<S241>/Divide' incorporates:
   *  Bias: '<S241>/Avoid zero div'
   *  BusAssignment: '<S5>/Bus Assignment'
   *  Constant: '<S241>/Constant'
   */
  U_beta_normalized = 1.0F / (Udc + 0.01F);

  /* Product: '<S241>/Product' incorporates:
   *  BusAssignment: '<S5>/Bus Assignment'
   */
  Ualpha_normalized = Ualpha_n * U_beta_normalized;

  /* Product: '<S241>/Product1' incorporates:
   *  BusAssignment: '<S5>/Bus Assignment'
   */
  U_beta_normalized *= Ubeta_c;

  /* Sum: '<S247>/Sum' incorporates:
   *  Fcn: '<S247>/VrefA'
   *  Fcn: '<S247>/VrefB'
   *  Fcn: '<S247>/VrefC'
   *  Gain: '<S247>/Gain'
   *  Gain: '<S247>/Gain1'
   *  Switch: '<S247>/Switch'
   *  Switch: '<S247>/Switch1'
   *  Switch: '<S247>/Switch2'
   */
  Sum = (int16_T)((uint16_T)((uint16_T)((uint16_T)((U_beta_normalized >= 0.0F) ?
    ((int16_T)1) : ((int16_T)0)) + (uint16_T)((uint16_T)((((-0.5F *
    U_beta_normalized) + (0.866502523F * Ualpha_normalized)) >= 0.0F) ?
    ((int16_T)1) : ((int16_T)0)) << 1U)) + (uint16_T)((uint16_T)((((-0.5F *
    U_beta_normalized) - (0.866502523F * Ualpha_normalized)) >= 0.0F) ?
    ((int16_T)1) : ((int16_T)0)) << 2U)));

  /* Fcn: '<S248>/Z' */
  Z = 0.5F * ((-1.732F * Ualpha_normalized) + U_beta_normalized);

  /* Fcn: '<S248>/Y' */
  Y_b = 0.5F * ((1.732F * Ualpha_normalized) + U_beta_normalized);

  /* MultiPortSwitch: '<S248>/Multiport Switch' incorporates:
   *  Constant: '<S264>/Constant'
   *  Constant: '<S264>/Constant1'
   *  Constant: '<S264>/Constant2'
   *  Fcn: '<S248>/X'
   *  Gain: '<S262>/Gain1'
   *  Gain: '<S263>/Gain1'
   *  Sum: '<S267>/Add'
   *  Sum: '<S269>/Add'
   *  Sum: '<S273>/Add'
   *  Sum: '<S274>/Add'
   *  Sum: '<S277>/Add'
   *  Sum: '<S280>/Add'
   */
  switch ((uint16_T)Sum) {
   case 0:
    Add = 0.5F;
    Gain_l5 = 0.5F;
    Ualpha_normalized = 0.5F;
    break;

   case 1:
    /* Gain: '<S266>/Gain' incorporates:
     *  Constant: '<S266>/Constant'
     *  Sum: '<S266>/Add'
     */
    Gain_l5 = 0.5F * (1.0F - Z - Y_b);

    /* Sum: '<S265>/Add' */
    Add = Gain_l5 + Z;
    Ualpha_normalized = Add + Y_b;
    break;

   case 2:
    /* Gain: '<S268>/Gain' incorporates:
     *  Constant: '<S268>/Constant'
     *  Fcn: '<S248>/X'
     *  Gain: '<S259>/Gain1'
     *  Sum: '<S268>/Add'
     */
    Add = 0.5F * (1.0F - Y_b - (-U_beta_normalized));

    /* Sum: '<S270>/Add' */
    Ualpha_normalized = Add + Y_b;
    Gain_l5 = Ualpha_normalized - U_beta_normalized;
    break;

   case 3:
    /* Gain: '<S271>/Gain' incorporates:
     *  Constant: '<S271>/Constant'
     *  Fcn: '<S248>/X'
     *  Gain: '<S260>/Gain'
     *  Sum: '<S271>/Add'
     */
    Add = 0.5F * (1.0F - (-Z) - U_beta_normalized);

    /* Sum: '<S272>/Add' incorporates:
     *  Gain: '<S260>/Gain'
     */
    Gain_l5 = Add - Z;
    Ualpha_normalized = Gain_l5 + U_beta_normalized;
    break;

   case 4:
    /* Gain: '<S276>/Gain' incorporates:
     *  Constant: '<S276>/Constant'
     *  Fcn: '<S248>/X'
     *  Gain: '<S261>/Gain'
     *  Sum: '<S276>/Add'
     */
    Ualpha_normalized = 0.5F * (1.0F - (-U_beta_normalized) - Z);

    /* Sum: '<S275>/Add' incorporates:
     *  Fcn: '<S248>/X'
     */
    Gain_l5 = Ualpha_normalized - U_beta_normalized;
    Add = Gain_l5 + Z;
    break;

   case 5:
    /* Gain: '<S278>/Gain' incorporates:
     *  Constant: '<S278>/Constant'
     *  Fcn: '<S248>/X'
     *  Gain: '<S262>/Gain1'
     *  Sum: '<S278>/Add'
     */
    Gain_l5 = 0.5F * (1.0F - U_beta_normalized - (-Y_b));

    /* Sum: '<S279>/Add' incorporates:
     *  Fcn: '<S248>/X'
     */
    Ualpha_normalized = Gain_l5 + U_beta_normalized;
    Add = Ualpha_normalized - Y_b;
    break;

   case 6:
    /* Gain: '<S282>/Gain' incorporates:
     *  Constant: '<S282>/Constant'
     *  Gain: '<S263>/Gain'
     *  Gain: '<S263>/Gain1'
     *  Sum: '<S282>/Add'
     */
    Ualpha_normalized = 0.5F * (1.0F - (-Y_b) - (-Z));

    /* Sum: '<S281>/Add' incorporates:
     *  Gain: '<S263>/Gain'
     */
    Add = Ualpha_normalized - Y_b;
    Gain_l5 = Add - Z;
    break;

   default:
    Add = 0.5F;
    Gain_l5 = 0.5F;
    Ualpha_normalized = 0.5F;
    break;
  }

  /* End of MultiPortSwitch: '<S248>/Multiport Switch' */

  /* Switch: '<S243>/Switch' incorporates:
   *  If: '<S244>/If'
   *  If: '<S245>/If'
   *  If: '<S246>/If'
   *  Product: '<S243>/Product1'
   */
  if (modActive) {
    /* If: '<S244>/If' incorporates:
     *  Constant: '<S244>/Constant'
     *  Constant: '<S244>/Constant1'
     *  SignalConversion generated from: '<S249>/In1'
     *  SignalConversion generated from: '<S250>/In1'
     */
    if (Add > 1.0F) {
      /* Outputs for IfAction SubSystem: '<S244>/If Action Subsystem' incorporates:
       *  ActionPort: '<S249>/Action Port'
       */
      Add = 1.0F;

      /* End of Outputs for SubSystem: '<S244>/If Action Subsystem' */
    } else if (Add < 0.0F) {
      /* Outputs for IfAction SubSystem: '<S244>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S250>/Action Port'
       */
      Add = 0.0F;

      /* End of Outputs for SubSystem: '<S244>/If Action Subsystem1' */
    } else {
      /* no actions */
    }

    U_beta_normalized = Add * PWM_ctrl.PRD_VAL;

    /* If: '<S245>/If' incorporates:
     *  Constant: '<S245>/Constant'
     *  Constant: '<S245>/Constant1'
     *  If: '<S244>/If'
     *  Product: '<S243>/Product1'
     *  SignalConversion generated from: '<S252>/In1'
     *  SignalConversion generated from: '<S253>/In1'
     */
    if (Gain_l5 > 1.0F) {
      /* Outputs for IfAction SubSystem: '<S245>/If Action Subsystem' incorporates:
       *  ActionPort: '<S252>/Action Port'
       */
      Gain_l5 = 1.0F;

      /* End of Outputs for SubSystem: '<S245>/If Action Subsystem' */
    } else if (Gain_l5 < 0.0F) {
      /* Outputs for IfAction SubSystem: '<S245>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S253>/Action Port'
       */
      Gain_l5 = 0.0F;

      /* End of Outputs for SubSystem: '<S245>/If Action Subsystem1' */
    } else {
      /* no actions */
    }

    Z = Gain_l5 * PWM_ctrl.PRD_VAL;

    /* If: '<S246>/If' incorporates:
     *  Constant: '<S246>/Constant'
     *  Constant: '<S246>/Constant1'
     *  If: '<S245>/If'
     *  Product: '<S243>/Product1'
     *  SignalConversion generated from: '<S255>/In1'
     *  SignalConversion generated from: '<S256>/In1'
     */
    if (Ualpha_normalized > 1.0F) {
      /* Outputs for IfAction SubSystem: '<S246>/If Action Subsystem' incorporates:
       *  ActionPort: '<S255>/Action Port'
       */
      Ualpha_normalized = 1.0F;

      /* End of Outputs for SubSystem: '<S246>/If Action Subsystem' */
    } else if (Ualpha_normalized < 0.0F) {
      /* Outputs for IfAction SubSystem: '<S246>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S256>/Action Port'
       */
      Ualpha_normalized = 0.0F;

      /* End of Outputs for SubSystem: '<S246>/If Action Subsystem1' */
    } else {
      /* no actions */
    }

    Ualpha_normalized *= PWM_ctrl.PRD_VAL;
  } else {
    /* Gain: '<S243>/Gain' */
    U_beta_normalized = 0.5F * PWM_ctrl.PRD_VAL;
    Z = U_beta_normalized;
    Ualpha_normalized = U_beta_normalized;
  }

  /* End of Switch: '<S243>/Switch' */

  /* BusAssignment: '<S243>/Bus Assignment' incorporates:
   *  DataStoreWrite: '<S5>/Data Store Write'
   */
  SVPWM = PWM_ctrl;
  SVPWM.Ta = U_beta_normalized;
  SVPWM.Tb = Z;
  SVPWM.Tc = Ualpha_normalized;
  SVPWM.sector = (uint16_T)Sum;

  /* SignalConversion generated from: '<S5>/Bus Selector' incorporates:
   *  DataStoreWrite: '<S5>/Data Store Write'
   */
  *Ta = SVPWM.Ta;

  /* SignalConversion generated from: '<S5>/Bus Selector' incorporates:
   *  DataStoreWrite: '<S5>/Data Store Write'
   */
  *Tb = SVPWM.Tb;

  /* SignalConversion generated from: '<S5>/Bus Selector' incorporates:
   *  DataStoreWrite: '<S5>/Data Store Write'
   */
  *Tc = SVPWM.Tc;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
