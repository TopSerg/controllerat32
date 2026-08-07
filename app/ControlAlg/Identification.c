/*
 * File: Identification.c
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
#include "systemDefinations.h"
#include "testSignals.h"
#include "Identification.h"
#include <string.h>
#include "platform_math.h"
#include "ControlSystem_v2_types.h"
#include <math.h>
#include "ControlSystem_v2_private.h"

/* Named constants for Chart: '<S309>/Chart' */
#define IN_DoneState                   ((uint8_T)1U)
#define IN_InitState                   ((uint8_T)2U)
#define IN_Step1                       ((uint8_T)3U)
#define IN_Step2                       ((uint8_T)4U)
#define IN_Step3                       ((uint8_T)5U)
#define IN_Step4                       ((uint8_T)6U)
#define IN_Step5                       ((uint8_T)7U)
#define IN_Step6                       ((uint8_T)8U)

/* Named constants for Chart: '<S313>/Chart' */
#define IN_AccelState                  ((uint8_T)1U)
#define IN_DoneState_j                 ((uint8_T)2U)
#define IN_DynamicTest                 ((uint8_T)3U)
#define IN_InitState_b                 ((uint8_T)4U)
#define IN_StaticTest                  ((uint8_T)5U)
#define IN_Step1_f                     ((uint8_T)6U)
#define IN_Step6_m                     ((uint8_T)7U)

/* Named constants for Chart: '<S311>/Chart' */
#define IN_calcState                   ((uint8_T)6U)

/* Forward declaration for local functions */
static real32_T speedToAngle(real32_T Wref, real32_T Ts, real32_T AngleLast);
static boolean_T checkDir(real32_T Wref, real32_T Wreal);
static real32_T checkResPoles(real32_T Wref, real32_T Wreal);

/* Function for Chart: '<S313>/Chart' */
static real32_T speedToAngle(real32_T Wref, real32_T Ts, real32_T AngleLast)
{
  real32_T angle;
  angle = AngleLast + (Wref * Ts);
  if ((real_T)angle > 6.2831853071795862) {
    angle -= 6.28318548F;
  } else if (angle < 0.0F) {
    angle += 6.28318548F;
  } else {
    /* no actions */
  }

  return angle;
}

/* Function for Chart: '<S313>/Chart' */
static boolean_T checkDir(real32_T Wref, real32_T Wreal)
{
  return (Wref / Wreal) > 0.0F;
}

/* Function for Chart: '<S313>/Chart' */
static real32_T checkResPoles(real32_T Wref, real32_T Wreal)
{
  return roundf(platform_abs(Wref) / Wreal);
}

/* System initialize for atomic system: '<S3>/Identification' */
void Identification_Init(void)
{
  /* SystemInitialize for IfAction SubSystem: '<S298>/ENCODER_Ident' */
  /* SystemInitialize for Chart: '<S307>/Chart' incorporates:
   *  DataStoreWrite: '<S307>/Data Store Write'
   *  DataStoreWrite: '<S307>/Data Store Write1'
   */
  (void)memset(&SystemParameters.hallTableDirect[0], 0, (uint16_T)(6U * sizeof
    (uint16_T)));
  TestRefSignals.IdTest = 0.0F;
  TestRefSignals.IqTest = 0.0F;
  TestRefSignals.VdTest = 0.0F;
  TestRefSignals.VqTest = 0.0F;
  TestRefSignals.WelTest = 0.0F;
  TestRefSignals.ThetaElTest = 0.0F;
  TestRefSignals.fixedAngle = false;
  TestRefSignals.voltageControl = false;
  TestRefSignals.testActive = false;
  TestRefSignals.testDone = false;

  /* End of SystemInitialize for SubSystem: '<S298>/ENCODER_Ident' */
}

/* Output and update for atomic system: '<S3>/Identification' */
void Identification(boolean_T OnSignal, WorkModeType workMode, TestSignals_st
                    *Out1_c)
{
  /* SwitchCase: '<S298>/Switch Case' */
  switch (workMode) {
   case HALL_ID:
    /* Outputs for IfAction SubSystem: '<S298>/HALL_Ident' incorporates:
     *  ActionPort: '<S302>/Action Port'
     */
    /* Chart: '<S309>/Chart' incorporates:
     *  DataStoreWrite: '<S309>/Data Store Write'
     *  Inport: '<Root>/inSignals'
     */
    if ((uint16_T)DW_l.is_active_c2_ControlSystem_v2 == 0U) {
      DW_l.is_active_c2_ControlSystem_v2 = 1U;
      DW_l.is_c2_ControlSystem_v2 = IN_InitState;
      DW_l.stepsCnt_e = 0U;
      DW_l.delayCnt_g = 0U;
      DW_l.treshold_f = 5000U;
      DW_l.testDone_j = false;
      DW_l.inProgress_a = false;
    } else {
      switch (DW_l.is_c2_ControlSystem_v2) {
       case IN_DoneState:
        DW_l.testDone_j = true;
        break;

       case IN_InitState:
        DW_l.inProgress_a = false;
        if (OnSignal) {
          DW_l.is_c2_ControlSystem_v2 = IN_Step1;
          DW_l.inProgress_a = true;
        }
        break;

       case IN_Step1:
        DW_l.inProgress_a = true;
        if (DW_l.delayCnt_g > DW_l.treshold_f) {
          DW_l.delayCnt_g = 0U;
          DW_l.is_c2_ControlSystem_v2 = IN_Step2;
        } else {
          DW_l.delayCnt_g++;
          DW_l.thetaRef_i = 1.0F;
          TestedParams.hallTable[0] = inSignals.HallCnt;
        }
        break;

       case IN_Step2:
        if (DW_l.delayCnt_g > DW_l.treshold_f) {
          DW_l.delayCnt_g = 0U;
          DW_l.is_c2_ControlSystem_v2 = IN_Step3;
        } else {
          DW_l.delayCnt_g++;
          DW_l.thetaRef_i = 2.0F;
          TestedParams.hallTable[1] = inSignals.HallCnt;
        }
        break;

       case IN_Step3:
        if (DW_l.delayCnt_g > DW_l.treshold_f) {
          DW_l.delayCnt_g = 0U;
          DW_l.is_c2_ControlSystem_v2 = IN_Step4;
        } else {
          DW_l.delayCnt_g++;
          DW_l.thetaRef_i = 3.0F;
          TestedParams.hallTable[2] = inSignals.HallCnt;
        }
        break;

       case IN_Step4:
        if (DW_l.delayCnt_g > DW_l.treshold_f) {
          DW_l.delayCnt_g = 0U;
          DW_l.is_c2_ControlSystem_v2 = IN_Step5;
        } else {
          DW_l.delayCnt_g++;
          DW_l.thetaRef_i = 4.0F;
          TestedParams.hallTable[3] = inSignals.HallCnt;
        }
        break;

       case IN_Step5:
        if (DW_l.delayCnt_g > DW_l.treshold_f) {
          DW_l.delayCnt_g = 0U;
          DW_l.is_c2_ControlSystem_v2 = IN_Step6;
          DW_l.stepsCnt_e++;
        } else {
          DW_l.delayCnt_g++;
          DW_l.thetaRef_i = 5.0F;
          TestedParams.hallTable[4] = inSignals.HallCnt;
        }
        break;

       default:
        /* case IN_Step6: */
        if (DW_l.delayCnt_g > DW_l.treshold_f) {
          DW_l.delayCnt_g = 0U;
          DW_l.is_c2_ControlSystem_v2 = IN_Step1;
          DW_l.inProgress_a = true;
        } else if (DW_l.stepsCnt_e == 4U) {
          DW_l.is_c2_ControlSystem_v2 = IN_DoneState;
          DW_l.testDone_j = true;
        } else {
          DW_l.delayCnt_g++;
          DW_l.thetaRef_i = 6.0F;
          TestedParams.hallTable[5] = inSignals.HallCnt;
        }
        break;
      }
    }

    /* End of Chart: '<S309>/Chart' */

    /* BusAssignment: '<S302>/Bus Assignment' incorporates:
     *  DataStoreRead: '<S302>/Data Store Read'
     */
    *Out1_c = TestRefSignals;
    Out1_c->ThetaElTest = DW_l.thetaRef_i;
    Out1_c->testActive = DW_l.inProgress_a;
    Out1_c->testDone = DW_l.testDone_j;
    Out1_c->fixedAngle = true;
    Out1_c->voltageControl = false;

    /* End of Outputs for SubSystem: '<S298>/HALL_Ident' */
    break;

   case RESOLVER_ID:
    /* Outputs for IfAction SubSystem: '<S298>/RESOLVER_Ident' incorporates:
     *  ActionPort: '<S304>/Action Port'
     */
    /* Chart: '<S313>/Chart' incorporates:
     *  DataStoreRead: '<S304>/Data Store Read'
     *  DataStoreRead: '<S304>/Data Store Read1'
     *  DataStoreWrite: '<S313>/Data Store Write'
     */
    if ((uint16_T)DW_l.is_active_c4_ControlSystem_v2 == 0U) {
      DW_l.is_active_c4_ControlSystem_v2 = 1U;
      DW_l.is_c4_ControlSystem_v2 = IN_InitState_b;
      TestedParams.resolverDir = 0U;
      TestedParams.resolverPoles = 0U;
      DW_l.delayCnt_o = 0U;
      DW_l.WelRated = 0.0F;
      DW_l.ThetaElTest = 0.0F;
      TestedParams.resolverZero = 0.0F;
      DW_l.tSample = 0.001F;
      DW_l.inProgress_c = false;
      DW_l.testDone_d = false;
    } else {
      switch (DW_l.is_c4_ControlSystem_v2) {
       case IN_AccelState:
        if (DW_l.WelRated > TestRefSignals.WelTest) {
          DW_l.is_c4_ControlSystem_v2 = IN_DynamicTest;
        } else {
          DW_l.WelRated += 100.0F * DW_l.tSample;
          DW_l.ThetaElTest = speedToAngle(DW_l.WelRated, DW_l.tSample,
            DW_l.ThetaElTest);
        }
        break;

       case IN_DoneState_j:
        if (!OnSignal) {
          DW_l.is_c4_ControlSystem_v2 = IN_InitState_b;
          TestedParams.resolverDir = 0U;
          TestedParams.resolverPoles = 0U;
          DW_l.delayCnt_o = 0U;
          DW_l.WelRated = 0.0F;
          DW_l.ThetaElTest = 0.0F;
          TestedParams.resolverZero = 0.0F;
          DW_l.tSample = 0.001F;
          DW_l.inProgress_c = false;
          DW_l.testDone_d = false;
        } else {
          DW_l.testDone_d = true;
        }
        break;

       case IN_DynamicTest:
        if (DW_l.delayCnt_o > 10000U) {
          DW_l.delayCnt_o = 0U;
          DW_l.is_c4_ControlSystem_v2 = IN_Step6_m;
          TestedParams.resolverDir = (uint16_T)(checkDir(TestRefSignals.WelTest,
            Control.Welectrical) ? 1U : 0U);
          TestedParams.resolverPoles = (uint16_T)checkResPoles
            (Control.Welectrical, TestRefSignals.WelTest);
        } else {
          DW_l.delayCnt_o++;
          DW_l.ThetaElTest = speedToAngle(DW_l.WelRated, DW_l.tSample,
            DW_l.ThetaElTest);
        }
        break;

       case IN_InitState_b:
        if (OnSignal) {
          DW_l.is_c4_ControlSystem_v2 = IN_Step1_f;
          DW_l.inProgress_c = true;
        }
        break;

       case IN_StaticTest:
        if (DW_l.delayCnt_o > 50U) {
          DW_l.delayCnt_o = 0U;
          DW_l.is_c4_ControlSystem_v2 = IN_AccelState;
        } else {
          DW_l.delayCnt_o++;
          TestedParams.resolverZero = Control.ThetaElectrical;
        }
        break;

       case IN_Step1_f:
        if (DW_l.delayCnt_o > 250U) {
          DW_l.delayCnt_o = 0U;
          DW_l.is_c4_ControlSystem_v2 = IN_StaticTest;
        } else {
          DW_l.delayCnt_o++;
        }
        break;

       default:
        /* case IN_Step6: */
        DW_l.is_c4_ControlSystem_v2 = IN_DoneState_j;
        DW_l.testDone_d = true;
        break;
      }
    }

    /* End of Chart: '<S313>/Chart' */

    /* BusAssignment: '<S304>/Bus Assignment' incorporates:
     *  DataStoreRead: '<S304>/Data Store Read'
     */
    *Out1_c = TestRefSignals;
    Out1_c->ThetaElTest = DW_l.ThetaElTest;
    Out1_c->testActive = DW_l.inProgress_c;
    Out1_c->testDone = DW_l.testDone_d;
    Out1_c->fixedAngle = true;
    Out1_c->voltageControl = false;

    /* End of Outputs for SubSystem: '<S298>/RESOLVER_Ident' */
    break;

   case ENCODER_ID:
    /* Outputs for IfAction SubSystem: '<S298>/ENCODER_Ident' incorporates:
     *  ActionPort: '<S301>/Action Port'
     */
    /* BusAssignment: '<S301>/Bus Assignment' incorporates:
     *  DataStoreRead: '<S301>/Data Store Read'
     */
    *Out1_c = TestRefSignals;

    /* Chart: '<S307>/Chart' incorporates:
     *  DataStoreWrite: '<S307>/Data Store Write'
     *  DataStoreWrite: '<S307>/Data Store Write1'
     *  Inport: '<Root>/inSignals'
     */
    if ((uint16_T)DW_l.is_active_c6_ControlSystem_v2 == 0U) {
      DW_l.is_active_c6_ControlSystem_v2 = 1U;
      DW_l.is_c6_ControlSystem_v2 = IN_InitState;
      DW_l.stepsCnt = 0U;
      DW_l.delayCnt_p = 0U;
      DW_l.treshold_g = 250U;
    } else {
      switch (DW_l.is_c6_ControlSystem_v2) {
       case IN_DoneState:
        DW_l.inProgress_n = false;
        TestRefSignals.testActive = false;
        break;

       case IN_InitState:
        if (OnSignal) {
          DW_l.is_c6_ControlSystem_v2 = IN_Step1;
        }
        break;

       case IN_Step1:
        if (DW_l.delayCnt_p > DW_l.treshold_g) {
          DW_l.delayCnt_p = 0U;
          DW_l.is_c6_ControlSystem_v2 = IN_Step2;
        } else {
          DW_l.delayCnt_p++;
          DW_l.thetaRef = 1.0F;
          SystemParameters.hallTableDirect[0] = inSignals.HallCnt;
          DW_l.inProgress_n = true;
          TestRefSignals.testActive = true;
        }
        break;

       case IN_Step2:
        if (DW_l.delayCnt_p > DW_l.treshold_g) {
          DW_l.delayCnt_p = 0U;
          DW_l.is_c6_ControlSystem_v2 = IN_Step3;
        } else {
          DW_l.delayCnt_p++;
          DW_l.thetaRef = 2.0F;
          SystemParameters.hallTableDirect[1] = inSignals.HallCnt;
        }
        break;

       case IN_Step3:
        if (DW_l.delayCnt_p > DW_l.treshold_g) {
          DW_l.delayCnt_p = 0U;
          DW_l.is_c6_ControlSystem_v2 = IN_Step4;
        } else {
          DW_l.delayCnt_p++;
          DW_l.thetaRef = 3.0F;
          SystemParameters.hallTableDirect[2] = inSignals.HallCnt;
        }
        break;

       case IN_Step4:
        if (DW_l.delayCnt_p > DW_l.treshold_g) {
          DW_l.delayCnt_p = 0U;
          DW_l.is_c6_ControlSystem_v2 = IN_Step5;
        } else {
          DW_l.delayCnt_p++;
          DW_l.thetaRef = 4.0F;
          SystemParameters.hallTableDirect[3] = inSignals.HallCnt;
        }
        break;

       case IN_Step5:
        if (DW_l.delayCnt_p > DW_l.treshold_g) {
          DW_l.delayCnt_p = 0U;
          DW_l.is_c6_ControlSystem_v2 = IN_Step6;
          DW_l.stepsCnt++;
        } else {
          DW_l.delayCnt_p++;
          DW_l.thetaRef = 5.0F;
          SystemParameters.hallTableDirect[4] = inSignals.HallCnt;
        }
        break;

       default:
        /* case IN_Step6: */
        if (DW_l.delayCnt_p > DW_l.treshold_g) {
          DW_l.delayCnt_p = 0U;
          DW_l.is_c6_ControlSystem_v2 = IN_Step1;
        } else if (DW_l.stepsCnt == 4U) {
          DW_l.is_c6_ControlSystem_v2 = IN_DoneState;
          DW_l.inProgress_n = false;
          TestRefSignals.testActive = false;
        } else {
          DW_l.delayCnt_p++;
          DW_l.thetaRef = 6.0F;
          SystemParameters.hallTableDirect[5] = inSignals.HallCnt;
        }
        break;
      }
    }

    /* End of Chart: '<S307>/Chart' */

    /* BusAssignment: '<S301>/Bus Assignment' */
    Out1_c->ThetaElTest = DW_l.thetaRef;
    Out1_c->testActive = DW_l.inProgress_n;
    Out1_c->fixedAngle = true;
    Out1_c->voltageControl = false;

    /* End of Outputs for SubSystem: '<S298>/ENCODER_Ident' */
    break;

   case RS:
    /* Outputs for IfAction SubSystem: '<S298>/RS_Ident' incorporates:
     *  ActionPort: '<S305>/Action Port'
     */
    /* Chart: '<S315>/Chart' incorporates:
     *  DataStoreRead: '<S305>/Data Store Read'
     *  DataStoreRead: '<S305>/Data Store Read1'
     *  DataStoreWrite: '<S315>/Data Store Write'
     */
    if ((uint16_T)DW_l.is_active_c1_ControlSystem_v2 == 0U) {
      DW_l.is_active_c1_ControlSystem_v2 = 1U;
      DW_l.is_c1_ControlSystem_v2 = IN_InitState;
      DW_l.delayCnt_b = 0U;
      DW_l.inProgress_g = false;
      DW_l.testDone_o = false;
      DW_l.Uz_e = 0.0F;
      DW_l.Isumm_h = 0.0F;
      DW_l.Isumm2 = 0.0F;
      DW_l.Usumm_p = 0.0F;
      DW_l.Usumm2 = 0.0F;
    } else {
      switch (DW_l.is_c1_ControlSystem_v2) {
       case IN_DoneState:
        if (!OnSignal) {
          DW_l.is_c1_ControlSystem_v2 = IN_InitState;
          DW_l.delayCnt_b = 0U;
          DW_l.inProgress_g = false;
          DW_l.testDone_o = false;
          DW_l.Uz_e = 0.0F;
          DW_l.Isumm_h = 0.0F;
          DW_l.Isumm2 = 0.0F;
          DW_l.Usumm_p = 0.0F;
          DW_l.Usumm2 = 0.0F;
        } else {
          DW_l.testDone_o = true;
          DW_l.Uz_e = 0.0F;
          TestedParams.Rs = ((((DW_l.Usumm2 - DW_l.Usumm_p) / (DW_l.Isumm2 -
            DW_l.Isumm_h)) / 1.5F) * 1.73205078F) / 2.0F;
        }
        break;

       case IN_InitState:
        DW_l.inProgress_g = false;
        if (OnSignal) {
          DW_l.is_c1_ControlSystem_v2 = IN_Step1;
          DW_l.inProgress_g = true;
        }
        break;

       case IN_Step1:
        DW_l.inProgress_g = true;
        if (Control.Iampl > TestRefSignals.IdTest) {
          DW_l.delayCnt_b = 0U;
          DW_l.is_c1_ControlSystem_v2 = IN_Step2;
        } else {
          DW_l.Uz_e += 0.001F;
        }
        break;

       case IN_Step2:
        if (DW_l.delayCnt_b > 10U) {
          DW_l.delayCnt_b = 0U;
          DW_l.is_c1_ControlSystem_v2 = IN_Step3;
        } else {
          DW_l.delayCnt_b++;
        }
        break;

       case IN_Step3:
        if (DW_l.delayCnt_b > 500U) {
          DW_l.delayCnt_b = 0U;
          DW_l.is_c1_ControlSystem_v2 = IN_Step4;
        } else {
          DW_l.delayCnt_b++;
          DW_l.Isumm_h += Control.Iampl;
          DW_l.Usumm_p += Control.UmodActual;
        }
        break;

       case IN_Step4:
        if (Control.Iampl > (TestRefSignals.IdTest + 25.0F)) {
          DW_l.delayCnt_b = 0U;
          DW_l.is_c1_ControlSystem_v2 = IN_Step5;
        } else {
          DW_l.Uz_e += 0.001F;
        }
        break;

       case IN_Step5:
        if (DW_l.delayCnt_b > 10U) {
          DW_l.delayCnt_b = 0U;
          DW_l.is_c1_ControlSystem_v2 = IN_Step6;
        } else {
          DW_l.delayCnt_b++;
        }
        break;

       default:
        /* case IN_Step6: */
        if (DW_l.delayCnt_b > 500U) {
          DW_l.delayCnt_b = 0U;
          DW_l.is_c1_ControlSystem_v2 = IN_DoneState;
        } else {
          DW_l.delayCnt_b++;
          DW_l.Isumm2 += Control.Iampl;
          DW_l.Usumm2 += Control.UmodActual;
        }
        break;
      }
    }

    /* End of Chart: '<S315>/Chart' */

    /* BusAssignment: '<S305>/Bus Assignment' incorporates:
     *  DataStoreRead: '<S305>/Data Store Read'
     */
    *Out1_c = TestRefSignals;
    Out1_c->VdTest = DW_l.Uz_e;
    Out1_c->VqTest = 0.0F;
    Out1_c->testActive = DW_l.inProgress_g;
    Out1_c->testDone = DW_l.testDone_o;
    Out1_c->WelTest = 0.0F;
    Out1_c->ThetaElTest = 0.0F;
    Out1_c->fixedAngle = true;
    Out1_c->voltageControl = true;

    /* End of Outputs for SubSystem: '<S298>/RS_Ident' */
    break;

   case LS:
    /* Outputs for IfAction SubSystem: '<S298>/LS_Ident' incorporates:
     *  ActionPort: '<S303>/Action Port'
     */
    /* Chart: '<S311>/Chart' incorporates:
     *  DataStoreRead: '<S303>/Data Store Read1'
     */
    if ((uint16_T)DW_l.is_active_c5_ControlSystem_v2 == 0U) {
      DW_l.is_active_c5_ControlSystem_v2 = 1U;
      DW_l.is_c5_ControlSystem_v2 = IN_InitState;
      DW_l.delayCnt = 0U;
      DW_l.treshold = 50U;
      DW_l.Uz = 0.0F;
      DW_l.Isumm = 0.0F;
      DW_l.Usumm = 0.0F;
      DW_l.Imin = 900.0F;
      DW_l.Imax = 0.0F;
      DW_l.inProgress = false;
      DW_l.testDone = false;
    } else {
      switch (DW_l.is_c5_ControlSystem_v2) {
       case IN_DoneState:
        if (!OnSignal) {
          DW_l.is_c5_ControlSystem_v2 = IN_InitState;
          DW_l.delayCnt = 0U;
          DW_l.treshold = 50U;
          DW_l.Uz = 0.0F;
          DW_l.Isumm = 0.0F;
          DW_l.Usumm = 0.0F;
          DW_l.Imin = 900.0F;
          DW_l.Imax = 0.0F;
          DW_l.inProgress = false;
          DW_l.testDone = false;
        } else {
          DW_l.testDone = true;
        }
        break;

       case IN_InitState:
        if (OnSignal) {
          DW_l.is_c5_ControlSystem_v2 = IN_Step1;
          DW_l.inProgress = true;
        }
        break;

       case IN_Step1:
        if (Control.Iampl > (real32_T)DW_l.treshold) {
          DW_l.delayCnt = 0U;
          DW_l.is_c5_ControlSystem_v2 = IN_Step2;
        } else {
          DW_l.inProgress = true;
          DW_l.Uz += 0.01F;
        }
        break;

       case IN_Step2:
        if (DW_l.delayCnt > 10U) {
          DW_l.delayCnt = 0U;
          DW_l.is_c5_ControlSystem_v2 = IN_Step3;
        } else {
          DW_l.delayCnt++;
        }
        break;

       case IN_Step3:
        if (DW_l.delayCnt > 500U) {
          DW_l.is_c5_ControlSystem_v2 = IN_calcState;
          DW_l.Uz = 0.0F;
          DW_l.delayCnt = 1U;
          DW_l.Ls = (DW_l.Usumm / DW_l.Isumm) / 1.73205078F;
        } else {
          DW_l.delayCnt++;
          DW_l.Isumm += Control.Iampl;
          DW_l.Usumm += Control.UmodActual;
          DW_l.Imin = platform_min(Control.Iampl, DW_l.Imin);
          DW_l.Imax = platform_max(Control.Iampl, DW_l.Imax);
        }
        break;

       default:
        /* case IN_calcState: */
        if (DW_l.delayCnt > 10U) {
          DW_l.is_c5_ControlSystem_v2 = IN_DoneState;
          DW_l.testDone = true;
        } else {
          DW_l.Uz = 0.0F;
          DW_l.delayCnt++;
          DW_l.Ls = (DW_l.Usumm / DW_l.Isumm) / 1.73205078F;
        }
        break;
      }
    }

    /* End of Chart: '<S311>/Chart' */

    /* Gain: '<S311>/toUH' incorporates:
     *  Bias: '<S311>/Bias'
     *  DataStoreRead: '<S303>/Data Store Read'
     *  DataStoreWrite: '<S311>/Data Store Write'
     *  Product: '<S311>/Divide'
     */
    TestedParams.Ld = 1.0E+6F * (DW_l.Ls / (TestRefSignals.WelTest + 1.0F));

    /* DataStoreWrite: '<S311>/Data Store Write' */
    TestedParams.Lq = TestedParams.Ld;

    /* BusAssignment: '<S303>/Bus Assignment' incorporates:
     *  DataStoreRead: '<S303>/Data Store Read'
     */
    *Out1_c = TestRefSignals;
    Out1_c->VdTest = DW_l.Uz;
    Out1_c->VqTest = 0.0F;
    Out1_c->testActive = DW_l.inProgress;
    Out1_c->testDone = DW_l.testDone;
    Out1_c->fixedAngle = true;
    Out1_c->voltageControl = true;

    /* End of Outputs for SubSystem: '<S298>/LS_Ident' */
    break;

   default:
    /* Outputs for IfAction SubSystem: '<S298>/Switch Case Action Subsystem3' incorporates:
     *  ActionPort: '<S306>/Action Port'
     */
    /* DataStoreRead: '<S306>/Data Store Read' */
    *Out1_c = TestRefSignals;

    /* End of Outputs for SubSystem: '<S298>/Switch Case Action Subsystem3' */
    break;
  }

  /* End of SwitchCase: '<S298>/Switch Case' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
