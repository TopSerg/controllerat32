/*
 * File: ControlSystem_v2.c
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
#include "ControlSystem_v2_private.h"
#include "platform_math.h"
#include "TrqCalc.h"
#include "utils.h"
#include "PIDs.h"
#include "ControlSystem_v2_types.h"
#include "Protections.h"
#include "Position_Observer.h"
#include "PWMcalc.h"
#include "testSignals.h"
#include "Logic.h"
#include "Identification.h"
#include <string.h>

/* Exported block signals */
inSignals_st inSignals;                /* '<Root>/inSignals' */
ExtRef_st extRef;                      /* '<Root>/extRef' */
InternalRef_st internalRef;            /* '<S3>/Bus Creator' */

/* Exported block parameters */
real32_T VdPlay = 0.0F;                /* Variable: VdPlay
                                        * Referenced by: '<S16>/Constant'
                                        */
real32_T VqPlay = 0.0F;                /* Variable: VqPlay
                                        * Referenced by: '<S16>/Constant1'
                                        */

/* Exported block states */
Control_st Control;                    /* '<Root>/Data Store Memory3' */
SystemParams_st SystemParameters;      /* '<Root>/Data Store Memory1' */
PID_st PID_IDC;                        /* '<S111>/Pid_Idc_Init'
                                        * Структура для регулятора тока Idc ( ограничение заряда разряда)
                                        */
PID_st PID_FW;                         /* '<S23>/Pid_FW_Init' */
PID_st PID_ID;                         /* '<S32>/Pid_Id_Init' */
PID_st PID_IQ;                         /* '<S33>/Pid_Iq_Init' */
Observer_st PositionObserver;          /* '<S10>/Observer_Init' */
TestedParams_st TestedParams;          /* '<Root>/Data Store Memory4'
                                        * Стуктура с парметрами полученными в результате тестирования системы
                                        */
TestSignals_st TestRefSignals;         /* '<Root>/Data Store Memory' */
PWM_st SVPWM;                          /* Simulink.Signal object 'SVPWM' */
LPF_st lpfUdc;                         /* Simulink.Signal object 'lpfUdc' */
LPF_st lpfFluxErrorAngle;          /* Simulink.Signal object 'lpfFluxErrorAngle'
                                    * Фильтр для расчета угла ошибки по потоку
                                    */
TripLevels_st TripLevels;              /* '<Root>/Data Store Memory2' */
real32_T debugAngle;                   /* '<S29>/Unit Delay' */
PID_st PID_spd;                        /* '<S322>/Pid_SPD_Init'
                                        * Регултор скорости
                                        */
PID_st PID_Trq;                        /* '<S342>/Pid_TRQ_Init'
                                        * Структура PID регулятора для момента
                                        */

/* Exported data definition */

/* Definition for custom storage class: Localizable */
static real32_T ratedSpd;              /* '<S339>/Unit Delay' */
static real32_T ratedTrq;              /* '<S343>/Unit Delay' */

/* Volatile memory section */
/* Definition for custom storage class: Volatile */
volatile real32_T ResSignalAmpl;       /* '<S194>/Add' */

/* Амплитуда сигнала резольвера */

/* Data with Exported storage */
real32_T DirectRef;                    /* '<Root>/DirectRef' */
real32_T Out1;                         /* '<Root>/Out1' */
outSinals_st outSignals;               /* '<Root>/outSignals' */

/* Block signals and states (default storage) */
DW DW_l;
uint16_T look2_is16lu32n16tu16_pbinlcse(int16_T u0, int16_T u1, const int16_T
  bp0[], const int16_T bp1[], const uint16_T table[], uint32_T prevIndex[],
  const uint32_T maxIndex[], uint32_T stride)
{
  uint32_T bpIndices[2];
  uint32_T fractions[2];
  uint32_T bpIdx;
  uint32_T found;
  uint32_T frac;
  uint32_T iLeft;
  int16_T bpLeftVar;
  uint16_T y;
  uint16_T yL_0d0;
  uint16_T yL_1d;
  uint16_T yR_0d0;

  /* Column-major Lookup 2-D
     Search method: 'binary'
     Use previous index: 'on'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
     Rounding mode: 'simplest'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'on'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
     Rounding mode: 'simplest'
   */
  if (u0 <= bp0[0UL]) {
    bpIdx = 0UL;
    frac = 0UL;
  } else if (u0 < bp0[maxIndex[0UL]]) {
    /* Binary Search using Previous Index */
    bpIdx = prevIndex[0UL];
    iLeft = 0UL;
    frac = maxIndex[0UL];
    found = 0UL;
    while (found == 0UL) {
      if (u0 < bp0[bpIdx]) {
        frac = (uint32_T)(bpIdx - 1UL);
        bpIdx = (uint32_T)((uint32_T)((uint32_T)(bpIdx + iLeft) - 1UL) >> 1UL);
      } else if (u0 < bp0[(uint32_T)(bpIdx + 1UL)]) {
        found = 1UL;
      } else {
        iLeft = (uint32_T)(bpIdx + 1UL);
        bpIdx = (uint32_T)((uint32_T)((uint32_T)(bpIdx + frac) + 1UL) >> 1UL);
      }
    }

    bpLeftVar = bp0[bpIdx];
    frac = (uint32_T)((uint32_T)((uint32_T)((uint16_T)((uint16_T)u0 - (uint16_T)
      bpLeftVar)) << 16U) / (uint32_T)((uint16_T)((uint16_T)bp0[(uint32_T)(bpIdx
      + 1UL)] - (uint16_T)bpLeftVar)));
  } else {
    bpIdx = (uint32_T)(maxIndex[0UL] - 1UL);
    frac = 65536UL;
  }

  prevIndex[0UL] = bpIdx;
  fractions[0UL] = frac;
  bpIndices[0UL] = bpIdx;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'on'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
     Rounding mode: 'simplest'
   */
  if (u1 <= bp1[0UL]) {
    bpIdx = 0UL;
    frac = 0UL;
  } else if (u1 < bp1[maxIndex[1UL]]) {
    /* Binary Search using Previous Index */
    bpIdx = prevIndex[1UL];
    iLeft = 0UL;
    frac = maxIndex[1UL];
    found = 0UL;
    while (found == 0UL) {
      if (u1 < bp1[bpIdx]) {
        frac = (uint32_T)(bpIdx - 1UL);
        bpIdx = (uint32_T)((uint32_T)((uint32_T)(bpIdx + iLeft) - 1UL) >> 1UL);
      } else if (u1 < bp1[(uint32_T)(bpIdx + 1UL)]) {
        found = 1UL;
      } else {
        iLeft = (uint32_T)(bpIdx + 1UL);
        bpIdx = (uint32_T)((uint32_T)((uint32_T)(bpIdx + frac) + 1UL) >> 1UL);
      }
    }

    bpLeftVar = bp1[bpIdx];
    frac = (uint32_T)((uint32_T)((uint32_T)((uint16_T)((uint16_T)u1 - (uint16_T)
      bpLeftVar)) << 16U) / (uint32_T)((uint16_T)((uint16_T)bp1[(uint32_T)(bpIdx
      + 1UL)] - (uint16_T)bpLeftVar)));
  } else {
    bpIdx = (uint32_T)(maxIndex[1UL] - 1UL);
    frac = 65536UL;
  }

  prevIndex[1UL] = bpIdx;

  /* Column-major Interpolation 2-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Rounding mode: 'simplest'
     Overflow mode: 'wrapping'
   */
  iLeft = (uint32_T)((uint32_T)(bpIdx * stride) + bpIndices[0UL]);
  yR_0d0 = table[(uint32_T)(iLeft + 1UL)];
  yL_0d0 = table[iLeft];
  if (yR_0d0 >= yL_0d0) {
    yL_1d = (uint16_T)((uint16_T)((uint32_T)((uint32_T)((uint32_T)((uint16_T)
      (yR_0d0 - yL_0d0)) * fractions[0UL]) >> 16U)) + yL_0d0);
  } else {
    yL_1d = (uint16_T)(yL_0d0 - (uint16_T)((uint32_T)((uint32_T)((uint32_T)
      ((uint16_T)(yL_0d0 - yR_0d0)) * fractions[0UL]) >> 16U)));
  }

  iLeft += stride;
  yR_0d0 = table[(uint32_T)(iLeft + 1UL)];
  yL_0d0 = table[iLeft];
  if (yR_0d0 >= yL_0d0) {
    yR_0d0 = (uint16_T)((uint16_T)((uint32_T)((uint32_T)((uint32_T)((uint16_T)
      (yR_0d0 - yL_0d0)) * fractions[0UL]) >> 16U)) + yL_0d0);
  } else {
    yR_0d0 = (uint16_T)(yL_0d0 - (uint16_T)((uint32_T)((uint32_T)((uint32_T)
      ((uint16_T)(yL_0d0 - yR_0d0)) * fractions[0UL]) >> 16U)));
  }

  if (yR_0d0 >= yL_1d) {
    y = (uint16_T)((uint16_T)((uint32_T)((uint32_T)((uint32_T)((uint16_T)(yR_0d0
      - yL_1d)) * frac) >> 16U)) + yL_1d);
  } else {
    y = (uint16_T)(yL_1d - (uint16_T)((uint32_T)((uint32_T)((uint32_T)((uint16_T)
      (yL_1d - yR_0d0)) * frac) >> 16U)));
  }

  return y;
}

/*
 * Output and update for atomic system:
 *    '<S188>/getHallIndex'
 *    '<S186>/getHallIndex'
 */
void getHallIndex(const uint16_T HallArr[6], uint16_T posCnt, uint16_T
                  *hallindex_p)
{
  int16_T b_i;
  int16_T i;
  boolean_T exitg1;
  i = 1;
  b_i = 0;
  exitg1 = false;
  while (((exitg1 ? ((uint16_T)1U) : ((uint16_T)0U)) == false) && (b_i < 6)) {
    i = (int16_T)(b_i + 1);
    if (HallArr[b_i] == posCnt) {
      exitg1 = true;
    } else {
      b_i++;
    }
  }

  *hallindex_p = (uint16_T)i;
}

/*
 * Output and update for action system:
 *    '<S223>/Switch Case Action Subsystem'
 *    '<S223>/Switch Case Action Subsystem1'
 */
void SwitchCaseActionSubsystem(real32_T IdRef_MTPA, real32_T IqRef_MTPA,
  real32_T *Out1, real32_T *Out2)
{
  /* SignalConversion generated from: '<S226>/IdRef_MTPA' */
  *Out1 = IdRef_MTPA;

  /* SignalConversion generated from: '<S226>/IqRef_MTPA' */
  *Out2 = IqRef_MTPA;
}

/* Output and update for atomic system: '<S222>/refCurrentCalc' */
void refCurrentCalc(real32_T SilencyCoef, real32_T motorEmf, real32_T ImRef_c,
                    motorTypeType motorType, real32_T *IdRef_MTPA, real32_T
                    *IqRef_MTPA)
{
  real32_T Divide_ka;

  /* Product: '<S229>/Divide' incorporates:
   *  Bias: '<S229>/Bias'
   *  Gain: '<S229>/Gain'
   *  Gain: '<S229>/Gain1'
   *  Math: '<S229>/Pow'
   *  Math: '<S229>/Pow2'
   *  Math: '<S229>/Pow3'
   *  Product: '<S229>/Product1'
   *  Sqrt: '<S229>/Sqrt'
   *  Sum: '<S229>/Add1'
   *  Sum: '<S229>/Add2'
   *
   * About '<S229>/Pow':
   *  Operator: magnitude^2
   *
   * About '<S229>/Pow2':
   *  Operator: magnitude^2
   *
   * About '<S229>/Pow3':
   *  Operator: magnitude^2
   */
  Divide_ka = (platform_sqrt(((8.0F * (SilencyCoef * SilencyCoef)) * (ImRef_c *
    ImRef_c)) + (motorEmf * motorEmf)) - motorEmf) / ((4.0F * SilencyCoef) +
    0.0001F);

  /* SwitchCase: '<S223>/Switch Case' incorporates:
   *  Constant: '<S225>/Constant'
   *  Math: '<S230>/Pow2'
   *  Math: '<S230>/Pow3'
   *  Product: '<S230>/Product'
   *  Signum: '<S230>/Sign'
   *  Sqrt: '<S230>/Sqrt'
   *  Sum: '<S230>/Add1'
   *
   * About '<S230>/Pow2':
   *  Operator: magnitude^2
   *
   * About '<S230>/Pow3':
   *  Operator: magnitude^2
   */
  switch (motorType) {
   case SMPSM:
    /* Outputs for IfAction SubSystem: '<S223>/Switch Case Action Subsystem' incorporates:
     *  ActionPort: '<S226>/Action Port'
     */
    SwitchCaseActionSubsystem(0.0F, ImRef_c, IdRef_MTPA, IqRef_MTPA);

    /* End of Outputs for SubSystem: '<S223>/Switch Case Action Subsystem' */
    break;

   case IPSM:
    {
      real32_T tmp;

      /* Signum: '<S230>/Sign' */
      if (ImRef_c < 0.0F) {
        tmp = -1.0F;
      } else {
        tmp = (real32_T)((ImRef_c > 0.0F) ? ((int16_T)1) : ((int16_T)0));
      }

      /* Outputs for IfAction SubSystem: '<S223>/Switch Case Action Subsystem1' incorporates:
       *  ActionPort: '<S227>/Action Port'
       */
      SwitchCaseActionSubsystem(Divide_ka, platform_sqrt((ImRef_c * ImRef_c) -
        (Divide_ka * Divide_ka)) * tmp, IdRef_MTPA, IqRef_MTPA);

      /* End of Outputs for SubSystem: '<S223>/Switch Case Action Subsystem1' */
    }
    break;

   default:
    /* Outputs for IfAction SubSystem: '<S223>/Switch Case Action Subsystem2' incorporates:
     *  ActionPort: '<S228>/Action Port'
     */
    /* SignalConversion generated from: '<S228>/IdRef_MTPA' */
    *IdRef_MTPA = 0.0F;

    /* SignalConversion generated from: '<S228>/IqRef_MTPA' */
    *IqRef_MTPA = 0.0F;

    /* End of Outputs for SubSystem: '<S223>/Switch Case Action Subsystem2' */
    break;
  }

  /* End of SwitchCase: '<S223>/Switch Case' */
}

/* Output and update for atomic system: '<S3>/workModeChoise' */
void workModeChoise(real32_T refValue_m, real32_T limitHigh, real32_T limitLow,
                    regModeType regMode, boolean_T clearErrSignal_g, boolean_T
                    onSignal_a, real32_T *ImRef_m, boolean_T *OnSignal,
                    boolean_T *ClearErrSignal, DW_workModeChoise *localDW)
{
  real32_T Add_e;
  real32_T Add_nx;
  real32_T Product_n3;
  real32_T tmp;

  /* Outputs for Atomic SubSystem: '<S320>/TrqCalc' */
  /* DataStoreRead: '<S300>/Data Store Read' incorporates:
   *  SignalConversion generated from: '<S320>/Bus Selector3'
   * */
  TrqCalc(Control.Id, Control.Iq, Control.ImaxRef, Control.motorParams.motorEmf,
          Control.motorParams.motorPoles, Control.motorParams.motorLq,
          Control.motorParams.motorLd, Control.Tmotor, &localDW->TrqCalc_m);

  /* End of Outputs for SubSystem: '<S320>/TrqCalc' */

  /* Abs: '<S361>/Abs' incorporates:
   *  DataStoreRead: '<S300>/Data Store Read'
   */
  Product_n3 = platform_abs(Control.Welectrical);

  /* Abs: '<S361>/Abs1' incorporates:
   *  DataStoreRead: '<S300>/Data Store Read'
   */
  Add_e = platform_abs(Control.UqRef);

  /* If: '<S361>/If' incorporates:
   *  Constant: '<S361>/Constant'
   *  DataStoreRead: '<S300>/Data Store Read'
   *  DataStoreRead: '<S361>/Data Store Read'
   *  SignalConversion generated from: '<S371>/In1'
   *  Trigonometry: '<S361>/Atan2'
   */
  if (Product_n3 > 300.0F) {
    tmp = platform_atan2(Control.UdRef, Add_e);
  } else {
    /* Outputs for IfAction SubSystem: '<S361>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S371>/Action Port'
     */
    tmp = 0.0F;

    /* End of Outputs for SubSystem: '<S361>/If Action Subsystem1' */
  }

  /* Outputs for Atomic SubSystem: '<S361>/LPF_FluxErr' */
  LPF(tmp, lpfFluxErrorAngle.k1, lpfFluxErrorAngle.k2, Control.motorFluxPosError,
      &Add_nx);

  /* End of If: '<S361>/If' */
  /* End of Outputs for SubSystem: '<S361>/LPF_FluxErr' */

  /* If: '<S361>/If1' incorporates:
   *  Bias: '<S361>/Bias1'
   *  Constant: '<S361>/Constant'
   *  DataStoreRead: '<S300>/Data Store Read'
   *  DataStoreRead: '<S361>/Data Store Read1'
   *  Product: '<S361>/Divide'
   *  SignalConversion generated from: '<S373>/In1'
   */
  if (Product_n3 > 300.0F) {
    tmp = Add_e / (Product_n3 + 0.1F);
  } else {
    /* Outputs for IfAction SubSystem: '<S361>/If Action Subsystem3' incorporates:
     *  ActionPort: '<S373>/Action Port'
     */
    tmp = 0.0F;

    /* End of Outputs for SubSystem: '<S361>/If Action Subsystem3' */
  }

  /* Outputs for Atomic SubSystem: '<S361>/LPF_FluxErr1' */
  LPF(tmp, lpfFluxErrorAngle.k1, lpfFluxErrorAngle.k2, Control.motorEmfCalc,
      &Add_e);

  /* End of If: '<S361>/If1' */
  /* End of Outputs for SubSystem: '<S361>/LPF_FluxErr1' */

  /* Sum: '<S366>/Add1' incorporates:
   *  Constant: '<S366>/Filter_Constant'
   *  Constant: '<S366>/One'
   *  DataStoreRead: '<S300>/Data Store Read'
   *  Product: '<S366>/Product'
   *  Product: '<S366>/Product1'
   *  UnitDelay: '<S366>/Unit Delay'
   */
  localDW->UnitDelay_DSTATE = (Control.Idc * 0.15F) + (0.85F *
    localDW->UnitDelay_DSTATE);

  /* BusAssignment: '<S320>/Bus Assignment' incorporates:
   *  DataStoreWrite: '<S300>/Data Store Write'
   */
  Control.motorTorque = localDW->TrqCalc_m.Torque_calc;
  Control.motorTorqueLimit = localDW->TrqCalc_m.TeMAxCalcc;
  Control.motorParams.motorLd = localDW->TrqCalc_m.Merge1_i;
  Control.motorParams.motorLq = localDW->TrqCalc_m.Merge_e;
  Control.motorParams.motorRs = localDW->TrqCalc_m.Product_ob;
  Control.motorParams.motorSilence = localDW->TrqCalc_m.Add_ct;

  /* If: '<S360>/If' incorporates:
   *  Abs: '<S360>/Abs'
   *  Constant: '<S367>/Constant'
   *  RelationalOperator: '<S367>/Compare'
   */
  if (platform_abs(localDW->TrqCalc_m.Add_ct) <= 1.0E-6F) {
    /* Outputs for IfAction SubSystem: '<S360>/If Action Subsystem' incorporates:
     *  ActionPort: '<S368>/Action Port'
     */
    /* BusAssignment: '<S320>/Bus Assignment' incorporates:
     *  Constant: '<S360>/Constant1'
     *  DataStoreWrite: '<S300>/Data Store Write'
     *  SignalConversion generated from: '<S368>/In1'
     */
    Control.motorParams.motorType = SMPSM;

    /* End of Outputs for SubSystem: '<S360>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S360>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S369>/Action Port'
     */
    /* BusAssignment: '<S320>/Bus Assignment' incorporates:
     *  Constant: '<S360>/Constant'
     *  DataStoreWrite: '<S300>/Data Store Write'
     *  SignalConversion generated from: '<S369>/In1'
     */
    Control.motorParams.motorType = IPSM;

    /* End of Outputs for SubSystem: '<S360>/If Action Subsystem1' */
  }

  /* End of If: '<S360>/If' */

  /* BusAssignment: '<S320>/Bus Assignment' incorporates:
   *  DataStoreRead: '<S300>/Data Store Read'
   *  DataStoreWrite: '<S300>/Data Store Write'
   *  Product: '<S359>/Product'
   *  UnitDelay: '<S366>/Unit Delay'
   */
  Control.motorFluxPosError = Add_nx;
  Control.motorEmfCalc = Add_e;
  Control.PelectricalFilt = localDW->UnitDelay_DSTATE * Control.UdcFiltered;
  Control.IdcFilt = localDW->UnitDelay_DSTATE;

  /* SwitchCase: '<S300>/Switch Case' incorporates:
   *  Constant: '<S321>/Constant'
   *  Constant: '<S341>/T_1ms'
   *  DataStoreRead: '<S339>/Data Store Read1'
   *  DataStoreRead: '<S343>/Data Store Read1'
   *  DataStoreWrite: '<S300>/Data Store Write'
   *  Product: '<S324>/Product'
   *  Product: '<S339>/Product'
   *  Product: '<S339>/Product1'
   *  Product: '<S341>/Product'
   *  Product: '<S343>/Product'
   *  Product: '<S343>/Product1'
   *  UnitDelay: '<S339>/Unit Delay'
   *  UnitDelay: '<S343>/Unit Delay'
   */
  switch (regMode) {
   case TrqCtrl:
    {
      boolean_T NOT_p;

      /* Outputs for IfAction SubSystem: '<S300>/TrqRegulator' incorporates:
       *  ActionPort: '<S319>/Action Port'
       */
      /* Logic: '<S341>/AND' incorporates:
       *  DataStoreWrite: '<S300>/Data Store Write'
       *  Logic: '<S341>/NOT'
       */
      NOT_p = (Control.stat.mod_Active && !Control.stat.testActive);

      /* MinMax: '<S344>/Max1' incorporates:
       *  DataStoreWrite: '<S300>/Data Store Write'
       */
      Add_e = platform_min(Control.motorTorqueLimit, limitHigh);

      /* Outputs for Atomic SubSystem: '<S343>/RateLimiter' */
      RateLimiter(ratedTrq, NOT_p ? refValue_m : 0.0F, 0.001F *
                  SystemParameters.Rate_Down, SystemParameters.Rate_Up * 0.001F,
                  &Product_n3);

      /* End of Outputs for SubSystem: '<S343>/RateLimiter' */

      /* Switch: '<S358>/Switch2' incorporates:
       *  Constant: '<S341>/T_1ms'
       *  DataStoreRead: '<S343>/Data Store Read1'
       *  Product: '<S341>/Product'
       *  Product: '<S343>/Product'
       *  Product: '<S343>/Product1'
       *  RelationalOperator: '<S358>/LowerRelop1'
       *  UnitDelay: '<S343>/Unit Delay'
       */
      if (Product_n3 <= Add_e) {
        /* MinMax: '<S344>/Max' incorporates:
         *  DataStoreWrite: '<S300>/Data Store Write'
         *  Gain: '<S344>/Gain1'
         */
        Add_e = platform_max(-Control.motorTorqueLimit, limitLow);

        /* Switch: '<S358>/Switch' incorporates:
         *  RelationalOperator: '<S358>/UpperRelop'
         */
        if (Product_n3 >= Add_e) {
          Add_e = Product_n3;
        }

        /* End of Switch: '<S358>/Switch' */
      }

      /* End of Switch: '<S358>/Switch2' */

      /* Gain: '<S342>/Gain' incorporates:
       *  DataStoreWrite: '<S300>/Data Store Write'
       */
      PID_Trq.OutMin = -Control.ImaxRef;

      /* BusAssignment: '<S342>/Bus Assignment' incorporates:
       *  DataStoreRead: '<S342>/Data Store Read'
       *  DataStoreWrite: '<S300>/Data Store Write'
       *  Logic: '<S342>/NOT'
       */
      PID_Trq.Ref = Add_e;
      PID_Trq.Fdb = Control.motorTorque;
      PID_Trq.OutMax = Control.ImaxRef;
      PID_Trq.defaultBit = !NOT_p;
      PID_Trq.Kp = SystemParameters.TrqKp;
      PID_Trq.Ki = SystemParameters.TrqKi;

      /* Outputs for Atomic SubSystem: '<S342>/PID_trq' */
      PID_trq(&PID_Trq, &PID_Trq);

      /* End of Outputs for SubSystem: '<S342>/PID_trq' */

      /* SignalConversion generated from: '<S342>/Bus Selector' */
      Add_e = PID_Trq.Out;

      /* Update for UnitDelay: '<S343>/Unit Delay' */
      ratedTrq = Product_n3;

      /* End of Outputs for SubSystem: '<S300>/TrqRegulator' */
    }
    break;

   case SpdCtrl:
    /* Outputs for IfAction SubSystem: '<S300>/SpeedRegulator' incorporates:
     *  ActionPort: '<S318>/Action Port'
     */
    /* Outputs for Atomic SubSystem: '<S339>/RateLimiter' */
    RateLimiter(ratedSpd, Control.stat.mod_Active ? refValue_m : 0.0F, 0.001F *
                SystemParameters.Rate_Down, SystemParameters.Rate_Up * 0.001F,
                &Product_n3);

    /* End of Outputs for SubSystem: '<S339>/RateLimiter' */

    /* Logic: '<S322>/NOT' incorporates:
     *  Constant: '<S321>/Constant'
     *  DataStoreRead: '<S339>/Data Store Read1'
     *  DataStoreWrite: '<S300>/Data Store Write'
     *  Product: '<S324>/Product'
     *  Product: '<S339>/Product'
     *  Product: '<S339>/Product1'
     *  UnitDelay: '<S339>/Unit Delay'
     */
    PID_spd.defaultBit = !Control.stat.mod_Active;

    /* Product: '<S323>/Divide' incorporates:
     *  Constant: '<S323>/Constant1'
     *  DataStoreWrite: '<S300>/Data Store Write'
     *  Switch: '<S327>/Switch2'
     */
    PID_spd.OutMin = 1.0F / Control.motorParams.motorKt;

    /* Product: '<S323>/Product' incorporates:
     *  Switch: '<S327>/Switch2'
     */
    Add_e = limitHigh * PID_spd.OutMin;

    /* Switch: '<S326>/Switch2' incorporates:
     *  DataStoreWrite: '<S300>/Data Store Write'
     *  RelationalOperator: '<S326>/LowerRelop1'
     *  Switch: '<S326>/Switch'
     */
    if (Add_e > Control.ImaxRef) {
      PID_spd.OutMax = Control.ImaxRef;
    } else {
      PID_spd.OutMax = Add_e;
    }

    /* End of Switch: '<S326>/Switch2' */

    /* Product: '<S323>/Product1' incorporates:
     *  Switch: '<S327>/Switch2'
     */
    Add_e = PID_spd.OutMin * limitLow;

    /* Switch: '<S327>/Switch' incorporates:
     *  DataStoreWrite: '<S300>/Data Store Write'
     *  Gain: '<S322>/Gain'
     *  RelationalOperator: '<S327>/UpperRelop'
     *  Switch: '<S327>/Switch2'
     */
    if (Add_e < -Control.ImaxRef) {
      PID_spd.OutMin = -Control.ImaxRef;
    } else {
      PID_spd.OutMin = Add_e;
    }

    /* End of Switch: '<S327>/Switch' */

    /* BusAssignment: '<S322>/Bus Assignment' incorporates:
     *  DataStoreRead: '<S322>/Data Store Read'
     *  DataStoreWrite: '<S300>/Data Store Write'
     */
    PID_spd.Ref = Product_n3;
    PID_spd.Fdb = Control.Wmechanical;
    PID_spd.Kp = SystemParameters.SpeedKp;
    PID_spd.Ki = SystemParameters.SpeedKi;

    /* Outputs for Atomic SubSystem: '<S322>/PID_spd' */
    PID_spd_d(&PID_spd, &PID_spd);

    /* End of Outputs for SubSystem: '<S322>/PID_spd' */

    /* SignalConversion generated from: '<S322>/Bus Selector' */
    Add_e = PID_spd.Out;

    /* Update for UnitDelay: '<S339>/Unit Delay' */
    ratedSpd = Product_n3;

    /* End of Outputs for SubSystem: '<S300>/SpeedRegulator' */
    break;

   default:
    /* Outputs for IfAction SubSystem: '<S300>/CurrentRefGenerator' incorporates:
     *  ActionPort: '<S317>/Action Port'
     */
    /* SignalConversion generated from: '<S317>/Iq' */
    Add_e = refValue_m;

    /* End of Outputs for SubSystem: '<S300>/CurrentRefGenerator' */
    break;
  }

  /* End of SwitchCase: '<S300>/Switch Case' */

  /* SignalConversion generated from: '<S300>/onSignal' */
  *OnSignal = onSignal_a;

  /* Product: '<S300>/Product' */
  *ImRef_m = (*OnSignal) ? Add_e : 0.0F;

  /* SignalConversion generated from: '<S300>/clearErrSignal' */
  *ClearErrSignal = clearErrSignal_g;
}

/* Model step function */
void isrADC(void)
{
  Errors_st Errors;
  real32_T Add;
  real32_T BusAssignment_o_IqRefReg;
  real32_T Control_f5_Ialpha;
  real32_T Control_f5_Ibeta;
  real32_T Control_f5_UdRef;
  real32_T Control_f5_UdcFiltered;
  real32_T Control_j_Tigbt;
  real32_T Control_lb_Iq;
  real32_T Dref;
  real32_T Merge_k5_idx_0;
  real32_T Merge_k5_idx_1;
  real32_T Merge_k5_idx_2;
  real32_T Product1_ck;
  real32_T Product1_f;
  real32_T Product1_il;
  real32_T Product_c_tmp;
  real32_T Product_ce;
  real32_T Product_g3;
  real32_T Qref;
  real32_T Switch1_e;
  real32_T Udz;
  real32_T Uqz;
  real32_T Vqff;
  real32_T limitCoefHigh_a;
  real32_T limitCoefHigh_g;
  uint16_T hallindex;
  boolean_T Control_lb_stat_driveMode;
  boolean_T Control_lb_stat_ready;
  boolean_T NOT;
  boolean_T PID_FW_tmp;
  boolean_T RelationalOperator1;
  boolean_T RelationalOperator1_a;
  boolean_T RelationalOperator_al;
  boolean_T RelationalOperator_h;
  boolean_T RelationalOperator_o;
  boolean_T fixedAngle;
  boolean_T voltageControl;

  /* RootInportFunctionCallGenerator generated from: '<Root>/isrADC' incorporates:
   *  SubSystem: '<Root>/ADCisr'
   */
  /* Outputs for Atomic SubSystem: '<S153>/Protections'
   *
   * Block description for '<S153>/Protections':
   *  Функция обрабатывает входные сигналы
   *  и формирует статус защит
   *  преобразователя и мотора
   */
  /* Inport: '<Root>/inSignals' incorporates:
   *  DataStoreRead: '<S4>/Data Store Read'
   */
  Protections(&inSignals, &Control.errors, Control.command.clrFault, &Errors);

  /* End of Outputs for SubSystem: '<S153>/Protections' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  /* If: '<S232>/If' incorporates:
   *  DataStoreRead: '<S232>/Data Store Read'
   */
  if (SystemParameters.phase_inverse > 0U) {
    /* Outputs for IfAction SubSystem: '<S232>/If Action Subsystem' incorporates:
     *  ActionPort: '<S233>/Action Port'
     */
    /* SignalConversion generated from: '<S233>/Out1' incorporates:
     *  Inport: '<Root>/inSignals'
     *  SignalConversion generated from: '<S200>/Bus Selector'
     * */
    Merge_k5_idx_0 = inSignals.Ic;
    Merge_k5_idx_1 = inSignals.Ib;
    Merge_k5_idx_2 = inSignals.Ia;

    /* End of Outputs for SubSystem: '<S232>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S232>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S234>/Action Port'
     */
    /* SignalConversion generated from: '<S234>/In1' incorporates:
     *  Inport: '<Root>/inSignals'
     *  SignalConversion generated from: '<S200>/Bus Selector'
     * */
    Merge_k5_idx_0 = inSignals.Ia;
    Merge_k5_idx_1 = inSignals.Ib;
    Merge_k5_idx_2 = inSignals.Ic;

    /* End of Outputs for SubSystem: '<S232>/If Action Subsystem1' */
  }

  /* End of If: '<S232>/If' */

  /* BusAssignment: '<S200>/Bus Assignment' incorporates:
   *  Inport: '<Root>/inSignals'
   *  MinMax: '<S231>/Max'
   */
  Control_j_Tigbt = platform_max(platform_max(inSignals.Tigbt1, inSignals.Tigbt2),
    inSignals.Tigbt3);

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */

  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* If: '<S206>/If' incorporates:
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S206>/TestRefSignals'
   *  DataStoreRead: '<S214>/Data Store Read2'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  If: '<S214>/If'
   *  SignalConversion generated from: '<S206>/reference_BusSelector'
   */
  if (TestRefSignals.testActive) {
    /* Outputs for IfAction SubSystem: '<S206>/Switch Case Action Subsystem1' incorporates:
     *  ActionPort: '<S211>/Action Port'
     */
    /* Merge generated from: '<S206>/Merge' incorporates:
     *  DataTypeConversion: '<S211>/Data Type Conversion'
     */
    fixedAngle = TestRefSignals.fixedAngle;

    /* Merge generated from: '<S206>/Merge' incorporates:
     *  DataTypeConversion: '<S211>/Data Type Conversion1'
     */
    voltageControl = TestRefSignals.voltageControl;

    /* Merge generated from: '<S206>/Merge' incorporates:
     *  DataTypeConversion: '<S211>/Data Type Conversion2'
     */
    Qref = TestRefSignals.IqTest;

    /* Merge generated from: '<S206>/Merge' incorporates:
     *  DataTypeConversion: '<S211>/Data Type Conversion3'
     */
    Dref = TestRefSignals.IdTest;

    /* Merge generated from: '<S206>/Merge' incorporates:
     *  DataTypeConversion: '<S211>/Data Type Conversion4'
     */
    Switch1_e = TestRefSignals.ThetaElTest;
	if (!fixedAngle) {
	  if (SystemParameters.phase_inverse > 0U) {
	    Switch1_e = Control.ThetaElectrical - 2.09439516F -
	      SystemParameters.correctionTheta;
	  } else {
	    Switch1_e = Control.ThetaElectrical + SystemParameters.correctionTheta;
	  }
	}
	while (Switch1_e >= TwoPI) {
	  Switch1_e -= TwoPI;
	}
	while (Switch1_e < 0.0F) {
	  Switch1_e += TwoPI;
	}
    /* End of Outputs for SubSystem: '<S206>/Switch Case Action Subsystem1' */
  } else {
    /* Outputs for IfAction SubSystem: '<S206>/Switch Case Action Subsystem' incorporates:
     *  ActionPort: '<S210>/Action Port'
     */
    if (SystemParameters.phase_inverse > 0U) {
      /* Outputs for IfAction SubSystem: '<S214>/If Action Subsystem' incorporates:
       *  ActionPort: '<S216>/Action Port'
       */
      /* If: '<S214>/If' incorporates:
       *  BusAssignment: '<S8>/Bus Assignment'
       *  DataStoreRead: '<S218>/Data Store Read1'
       *  DataStoreRead: '<S4>/Data Store Read'
       *  Gain: '<S218>/Gain'
       *  Sum: '<S216>/Add'
       *  Sum: '<S219>/Add1'
       */
      Switch1_e = Control.ThetaElectrical - 2.09439516F -
        SystemParameters.correctionTheta;

      /* End of Outputs for SubSystem: '<S214>/If Action Subsystem' */
    } else {
      /* Outputs for IfAction SubSystem: '<S214>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S217>/Action Port'
       */
      /* If: '<S214>/If' incorporates:
       *  BusAssignment: '<S8>/Bus Assignment'
       *  DataStoreRead: '<S220>/Data Store Read1'
       *  DataStoreRead: '<S4>/Data Store Read'
       *  Sum: '<S221>/Add1'
       */
      Switch1_e = Control.ThetaElectrical + SystemParameters.correctionTheta;

      /* End of Outputs for SubSystem: '<S214>/If Action Subsystem1' */
    }

    /* Switch: '<S215>/Switch' incorporates:
     *  Bias: '<S215>/Bias'
     */
    if (Switch1_e > TwoPI) {
      Switch1_e -= TwoPI;
    }

    /* End of Switch: '<S215>/Switch' */

    /* Switch: '<S212>/Switch' incorporates:
     *  BusAssignment: '<S8>/Bus Assignment'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  Switch: '<S215>/Switch1'
     */
    if (Control.command.fixedAngle) {
      /* Merge generated from: '<S206>/Merge' */
      Switch1_e = Control.ThetaElectrCorr;
    } else if (Switch1_e < 0.0F) {
      /* Merge generated from: '<S206>/Merge' incorporates:
       *  Bias: '<S215>/Bias1'
       */
      Switch1_e += TwoPI;
    } else {
      /* no actions */
    }

    /* End of Switch: '<S212>/Switch' */

    /* Outputs for Atomic SubSystem: '<S222>/refCurrentCalc' */
    refCurrentCalc(Control.motorParams.motorSilence,
                   Control.motorParams.motorEmf, internalRef.ImRef,
                   Control.motorParams.motorType, &Dref, &Qref);

    /* End of Outputs for SubSystem: '<S222>/refCurrentCalc' */

    /* Merge generated from: '<S206>/Merge' incorporates:
     *  BusAssignment: '<S8>/Bus Assignment'
     *  Constant: '<S210>/Constant'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  DataTypeConversion: '<S210>/Data Type Conversion'
     *  SignalConversion generated from: '<S206>/reference_BusSelector'
     */
    fixedAngle = false;

    /* Merge generated from: '<S206>/Merge' incorporates:
     *  Constant: '<S210>/Constant1'
     *  DataTypeConversion: '<S210>/Data Type Conversion1'
     */
    voltageControl = false;

    /* End of Outputs for SubSystem: '<S206>/Switch Case Action Subsystem' */
  }

  /* End of If: '<S206>/If' */

  /* Sum: '<S209>/Add1' incorporates:
   *  Constant: '<S209>/Filter_Constant'
   *  Constant: '<S209>/One'
   *  Product: '<S209>/Product'
   *  Product: '<S209>/Product1'
   *  UnitDelay: '<S209>/Unit Delay'
   */
  DW_l.UnitDelay_DSTATE = (Qref * 0.25F) + (0.75F * DW_l.UnitDelay_DSTATE);

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */

  /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
  /* Abs: '<S202>/Abs' incorporates:
   *  Abs: '<S235>/Abs'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S4>/Data Store Read'
   */
  Udz = platform_abs(Control.Welectrical);
  limitCoefHigh_a = Udz;

  /* RelationalOperator: '<S204>/Compare' incorporates:
   *  BusAssignment: '<S8>/Bus Assignment'
   *  Constant: '<S204>/Constant'
   *  DataStoreRead: '<S4>/Data Store Read'
   */
  RelationalOperator_al = (Control.UdcFiltered >= 15.0F);

  /* Switch: '<S202>/Switch' incorporates:
   *  Abs: '<S202>/Abs'
   *  BusAssignment: '<S199>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  Constant: '<S203>/Constant'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  Logic: '<S202>/AND'
   *  Logic: '<S202>/AND1'
   *  RelationalOperator: '<S203>/Compare'
   */
  if (Control.stat.mod_Active && (Udz >= 5.0F)) {
    RelationalOperator1_a = Control.stat.mod_Active;
  } else {
    /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
    RelationalOperator1_a = (internalRef.onSignal && Control.stat.ready &&
      RelationalOperator_al);

    /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  }

  /* Logic: '<S202>/AND2' incorporates:
   *  BusAssignment: '<S8>/Bus Assignment'
   *  Logic: '<S202>/NOT'
   *  Switch: '<S202>/Switch'
   */
  RelationalOperator1_a = (!Errors.GlobalError && RelationalOperator1_a);

  /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */

  /* Outputs for Atomic SubSystem: '<S201>/LPF_Udc' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  /* BusAssignment: '<S200>/Bus Assignment' incorporates:
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S201>/Data Store Read'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  Inport: '<Root>/inSignals'
   */
  LPF(inSignals.Vdc, lpfUdc.k1, lpfUdc.k2, Control.UdcFiltered, &limitCoefHigh_a);

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */
  /* End of Outputs for SubSystem: '<S201>/LPF_Udc' */

  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* Product: '<S235>/Product' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  UnitDelay: '<S209>/Unit Delay'
   */
  Product1_ck = Control.Welectrical * DW_l.UnitDelay_DSTATE;

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */

  /* Outputs for IfAction SubSystem: '<S235>/If Action Subsystem1' incorporates:
   *  ActionPort: '<S240>/Action Port'
   */
  /* Outputs for IfAction SubSystem: '<S235>/If Action Subsystem' incorporates:
   *  ActionPort: '<S239>/Action Port'
   */
  /* If: '<S235>/If' incorporates:
   *  SignalConversion generated from: '<S239>/In1'
   *  SignalConversion generated from: '<S240>/In1'
   */
  RelationalOperator_o = ((Product1_ck >= 0.0F) || (Udz <= 50.0F));

  /* End of Outputs for SubSystem: '<S235>/If Action Subsystem' */
  /* End of Outputs for SubSystem: '<S235>/If Action Subsystem1' */

  /* Outputs for Atomic SubSystem: '<S201>/LPF_Udc1' */
  /* BusAssignment: '<S8>/Bus Assignment' incorporates:
   *  DataStoreRead: '<S201>/Data Store Read1'
   *  DataStoreRead: '<S4>/Data Store Read'
   */
  LPF(Control.Welectrical, lpfUdc.k1, lpfUdc.k2, Control.WelectricalFilt,
      &Product1_ck);

  /* End of Outputs for SubSystem: '<S201>/LPF_Udc1' */

  /* BusAssignment: '<S201>/Bus Assignment' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   */
  Control_f5_UdcFiltered = limitCoefHigh_a;
  Qref = Product1_ck;

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentCalculations' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  /* Gain: '<S19>/Gain1' incorporates:
   *  BusAssignment: '<S200>/Bus Assignment'
   *  Gain: '<S19>/Gain'
   *  Sum: '<S19>/Add'
   */
  Product_ce = 0.3333F * ((2.0F * Merge_k5_idx_0) - Merge_k5_idx_1 -
    Merge_k5_idx_2);

  /* Gain: '<S19>/Gain2' incorporates:
   *  BusAssignment: '<S200>/Bus Assignment'
   *  Sum: '<S19>/Add1'
   */
  Product1_ck = 0.577292264F * (Merge_k5_idx_1 - Merge_k5_idx_2);

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Product: '<S20>/Product' */
  Product_g3 = Product_ce * Product_ce;

  /* Product: '<S20>/Product1' */
  Product1_f = Product1_ck * Product1_ck;

  /* BusAssignment: '<S17>/Bus Assignment' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   */
  Control_f5_Ialpha = Product_ce;
  Control_f5_Ibeta = Product1_ck;

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* Trigonometry: '<S21>/Cos' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   */
  Product_ce = platform_cos(Switch1_e);

  /* Trigonometry: '<S21>/Cos1' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   */
  limitCoefHigh_g = platform_sin(Switch1_e);

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Sum: '<S21>/Add' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S17>/Bus Assignment'
   *  Product: '<S21>/Product'
   *  Product: '<S21>/Product1'
   */
  Product1_il = (Control_f5_Ialpha * Product_ce) + (Product1_ck *
    limitCoefHigh_g);

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
  /* BusAssignment: '<S18>/Bus Assignment' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S17>/Bus Assignment'
   *  BusAssignment: '<S198>/Bus Assignment'
   *  BusAssignment: '<S201>/Bus Assignment'
   *  Product: '<S21>/Product2'
   *  Product: '<S21>/Product3'
   *  Sum: '<S21>/Add1'
   */
  Control_lb_stat_ready = RelationalOperator_al;

  /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
  Control_lb_stat_driveMode = RelationalOperator_o;

  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  Control_lb_Iq = (Product1_ck * Product_ce) - (Control_f5_Ialpha *
    limitCoefHigh_g);

  /* End of Outputs for SubSystem: '<S7>/CurrentCalculations' */

  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* If: '<S110>/If' incorporates:
   *  BusAssignment: '<S201>/Bus Assignment'
   */
  if (RelationalOperator_o) {
    /* Sum: '<S119>/Add1' incorporates:
     *  Constant: '<S119>/Constant'
     *  Constant: '<S119>/Constant1'
     *  DataStoreRead: '<S119>/Data Store Read'
     *  Product: '<S119>/Divide'
     *  Product: '<S119>/Product'
     *  Sum: '<S119>/Add'
     */
    Product_ce = ((limitCoefHigh_a - (real32_T)
                   SystemParameters.FuncLimits.Udc_low) * (1.0F / (real32_T)
      SystemParameters.FuncLimits.Udc_low_diap)) + 1.0F;

    /* Saturate: '<S119>/Saturation' */
    if (Product_ce > 1.0F) {
      /* Outputs for IfAction SubSystem: '<S110>/If Action Subsystem' incorporates:
       *  ActionPort: '<S117>/Action Port'
       */
      /* SignalConversion generated from: '<S117>/limitCoef' */
      Product_ce = 1.0F;

      /* End of Outputs for SubSystem: '<S110>/If Action Subsystem' */
    } else if (Product_ce < 0.0F) {
      /* Outputs for IfAction SubSystem: '<S110>/If Action Subsystem' incorporates:
       *  ActionPort: '<S117>/Action Port'
       */
      /* SignalConversion generated from: '<S117>/limitCoef' */
      Product_ce = 0.0F;

      /* End of Outputs for SubSystem: '<S110>/If Action Subsystem' */
    } else {
      /* no actions */
    }

    /* End of Saturate: '<S119>/Saturation' */
  } else {
    /* Sum: '<S116>/Add1' incorporates:
     *  Constant: '<S116>/Constant'
     *  Constant: '<S116>/Constant1'
     *  DataStoreRead: '<S116>/Data Store Read'
     *  Product: '<S116>/Divide'
     *  Product: '<S116>/Product'
     *  Sum: '<S116>/Add'
     */
    Product_ce = 1.0F - ((limitCoefHigh_a - (real32_T)
                          SystemParameters.FuncLimits.Udc_high) * (1.0F /
      (real32_T)SystemParameters.FuncLimits.Udc_high_diap));

    /* Saturate: '<S116>/Saturation' */
    if (Product_ce > 1.0F) {
      /* Outputs for IfAction SubSystem: '<S110>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S118>/Action Port'
       */
      /* SignalConversion generated from: '<S118>/In1' */
      Product_ce = 1.0F;

      /* End of Outputs for SubSystem: '<S110>/If Action Subsystem1' */
    } else if (Product_ce < 0.0F) {
      /* Outputs for IfAction SubSystem: '<S110>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S118>/Action Port'
       */
      /* SignalConversion generated from: '<S118>/In1' */
      Product_ce = 0.0F;

      /* End of Outputs for SubSystem: '<S110>/If Action Subsystem1' */
    } else {
      /* no actions */
    }

    /* End of Saturate: '<S116>/Saturation' */
  }

  /* End of If: '<S110>/If' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* RelationalOperator: '<S110>/Relational Operator' incorporates:
   *  Constant: '<S110>/Constant'
   */
  RelationalOperator_o = (Product_ce < 1.0F);

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* Sum: '<S132>/Add1' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   *  BusAssignment: '<S200>/Bus Assignment'
   *  Constant: '<S112>/Constant1'
   *  Constant: '<S132>/Constant'
   *  DataStoreRead: '<S112>/Data Store Read'
   *  Product: '<S112>/Divide'
   *  Product: '<S132>/Product'
   *  Sum: '<S132>/Add'
   */
  Uqz = 1.0F - ((Control_j_Tigbt - (real32_T)
                 SystemParameters.FuncLimits.Igbt_Thigh) * (1.0F / (real32_T)
    SystemParameters.FuncLimits.Igbt_Thigh_diap));

  /* Sum: '<S133>/Add1' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   *  BusAssignment: '<S200>/Bus Assignment'
   *  Constant: '<S112>/LimitCoef1'
   *  Constant: '<S112>/LimitVal1'
   *  Constant: '<S133>/Constant'
   *  Product: '<S133>/Product'
   *  Sum: '<S133>/Add'
   */
  Product1_ck = ((Control_j_Tigbt - -35.0F) * 0.1F) + 1.0F;

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Saturate: '<S132>/Saturation' */
  if (Uqz > 1.0F) {
    Uqz = 1.0F;
  } else if (Uqz < 0.0F) {
    Uqz = 0.0F;
  } else {
    /* no actions */
  }

  /* Saturate: '<S133>/Saturation' */
  if (Product1_ck > 1.0F) {
    Product1_ck = 1.0F;
  }

  /* MinMax: '<S112>/Min' incorporates:
   *  Saturate: '<S132>/Saturation'
   *  Saturate: '<S133>/Saturation'
   */
  Product1_ck = platform_min(Uqz, Product1_ck);

  /* RelationalOperator: '<S112>/Relational Operator' incorporates:
   *  Constant: '<S112>/Constant'
   */
  RelationalOperator_al = (Product1_ck < 1.0F);

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  /* Sum: '<S134>/Add1' incorporates:
   *  BusAssignment: '<S200>/Bus Assignment'
   *  Constant: '<S113>/Constant1'
   *  Constant: '<S134>/Constant'
   *  DataStoreRead: '<S113>/Data Store Read'
   *  Inport: '<Root>/inSignals'
   *  Product: '<S113>/Divide'
   *  Product: '<S134>/Product'
   *  Sum: '<S134>/Add'
   */
  Uqz = 1.0F - ((inSignals.Tmotor - (real32_T)
                 SystemParameters.FuncLimits.Motor_Thigh) * (1.0F / (real32_T)
    SystemParameters.FuncLimits.Motor_Thigh_diap));

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Saturate: '<S134>/Saturation' */
  if (Uqz > 1.0F) {
    Uqz = 1.0F;
  } else if (Uqz < 0.0F) {
    Uqz = 0.0F;
  } else {
    /* no actions */
  }

  /* MinMax: '<S113>/Min' incorporates:
   *  Saturate: '<S134>/Saturation'
   *  Saturate: '<S135>/Saturation'
   */
  limitCoefHigh_g = platform_min(Uqz, 1.0F);

  /* RelationalOperator: '<S113>/Relational Operator' incorporates:
   *  Constant: '<S113>/Constant'
   */
  RelationalOperator_h = (limitCoefHigh_g < 1.0F);

  /* Switch: '<S114>/Switch1' incorporates:
   *  BusAssignment: '<S18>/Bus Assignment'
   *  Constant: '<S114>/Constant3'
   *  Switch: '<S111>/Switch'
   */
  if (Control_lb_stat_driveMode) {
    /* Switch: '<S114>/Switch' incorporates:
     *  BusAssignment: '<S8>/Bus Assignment'
     *  Constant: '<S137>/Constant'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  RelationalOperator: '<S137>/Compare'
     */
    if (Control.Welectrical >= -1.0F) {
      /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
      /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
      /* Sum: '<S138>/Add1' incorporates:
       *  Abs: '<S202>/Abs'
       *  Constant: '<S114>/Constant2'
       *  Constant: '<S138>/Constant'
       *  DataStoreRead: '<S114>/Data Store Read'
       *  Product: '<S114>/Divide'
       *  Product: '<S138>/Product'
       *  Sum: '<S138>/Add'
       */
      limitCoefHigh_a = 1.0F - ((Udz - (real32_T)
        SystemParameters.FuncLimits.Speed_fwd) * (1.0F / (real32_T)
        SystemParameters.FuncLimits.Speed_fwd_diap));

      /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
      /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

      /* Saturate: '<S138>/Saturation' */
      if (limitCoefHigh_a > 1.0F) {
        limitCoefHigh_a = 1.0F;
      } else if (limitCoefHigh_a < 0.0F) {
        limitCoefHigh_a = 0.0F;
      } else {
        /* no actions */
      }

      /* End of Saturate: '<S138>/Saturation' */
    } else {
      /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
      /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
      /* Sum: '<S136>/Add1' incorporates:
       *  Abs: '<S202>/Abs'
       *  Constant: '<S114>/Constant2'
       *  Constant: '<S136>/Constant'
       *  DataStoreRead: '<S114>/Data Store Read'
       *  Product: '<S114>/Divide1'
       *  Product: '<S136>/Product'
       *  Sum: '<S136>/Add'
       */
      limitCoefHigh_a = 1.0F - ((Udz - (real32_T)
        SystemParameters.FuncLimits.Speed_rev) * (1.0F / (real32_T)
        SystemParameters.FuncLimits.Speed_rev_diap));

      /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
      /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

      /* Saturate: '<S136>/Saturation' */
      if (limitCoefHigh_a > 1.0F) {
        limitCoefHigh_a = 1.0F;
      } else if (limitCoefHigh_a < 0.0F) {
        limitCoefHigh_a = 0.0F;
      } else {
        /* no actions */
      }

      /* End of Saturate: '<S136>/Saturation' */
    }

    /* End of Switch: '<S114>/Switch' */

    /* BusAssignment: '<S111>/Bus Assignment1' incorporates:
     *  DataStoreRead: '<S109>/Data Store Read'
     */
    PID_IDC.Ref = SystemParameters.MaxDischarge;
  } else {
    limitCoefHigh_a = 1.0F;

    /* BusAssignment: '<S111>/Bus Assignment1' incorporates:
     *  Constant: '<S114>/Constant3'
     *  DataStoreRead: '<S109>/Data Store Read'
     */
    PID_IDC.Ref = SystemParameters.MaxCharge;
  }

  /* End of Switch: '<S114>/Switch1' */

  /* RelationalOperator: '<S114>/Relational Operator1' incorporates:
   *  Constant: '<S114>/Constant3'
   */
  RelationalOperator1 = (limitCoefHigh_a < 1.0F);

  /* BusAssignment: '<S111>/Bus Assignment1' incorporates:
   *  Abs: '<S111>/Abs'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  Constant: '<S111>/Constant'
   *  Constant: '<S111>/Constant1'
   *  DataStoreRead: '<S4>/Data Store Read'
   */
  PID_IDC.Fdb = platform_abs(Control.IdcFilt);
  PID_IDC.defaultBit = false;
  PID_IDC.OutMin = 0.0F;
  PID_IDC.OutMax = 1.0F;

  /* Outputs for Atomic SubSystem: '<S111>/PID_Idc' */
  PID_Idc(&PID_IDC, &PID_IDC);

  /* End of Outputs for SubSystem: '<S111>/PID_Idc' */

  /* RelationalOperator: '<S111>/Relational Operator' incorporates:
   *  Constant: '<S111>/Constant2'
   */
  NOT = (PID_IDC.Out < 1.0F);

  /* Product: '<S109>/Product' incorporates:
   *  DataStoreRead: '<S109>/Data Store Read'
   *  MinMax: '<S109>/Min'
   */
  Product_ce = SystemParameters.MaxCurrent * platform_min(platform_min
    (platform_min(platform_min(PID_IDC.Out, Product_ce), limitCoefHigh_g),
     Product1_ck), limitCoefHigh_a);

  /* BusAssignment: '<S23>/Bus Assignment' incorporates:
   *  BusAssignment: '<S18>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S23>/Data Store Read1'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  Product: '<S23>/Product'
   */
  PID_FW.Ref = SystemParameters.FieldWeakingCoef * Control_f5_UdcFiltered;
  PID_FW.Fdb = Control.UmodActual;

  /* Outputs for IfAction SubSystem: '<S22>/If Action Subsystem1' incorporates:
   *  ActionPort: '<S26>/Action Port'
   */
  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
  /* If: '<S22>/If' incorporates:
   *  BusAssignment: '<S198>/Bus Assignment'
   *  Logic: '<S23>/NOT'
   *  Logic: '<S32>/NOT'
   *  Logic: '<S33>/NOT'
   */
  PID_FW_tmp = !RelationalOperator1_a;

  /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  /* End of Outputs for SubSystem: '<S22>/If Action Subsystem1' */

  /* BusAssignment: '<S23>/Bus Assignment' incorporates:
   *  BusAssignment: '<S18>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S23>/Data Store Read1'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  Gain: '<S23>/Gain1'
   *  Gain: '<S23>/Gain2'
   *  Logic: '<S23>/NOT'
   *  Logic: '<S23>/OR'
   *  RelationalOperator: '<S23>/Relational Operator'
   */
  PID_FW.defaultBit = ((Control.UmodActual <= (0.1F * Control_f5_UdcFiltered)) ||
                       PID_FW_tmp);
  PID_FW.OutMin = -SystemParameters.MinIdCurrent;
  PID_FW.OutMax = SystemParameters.MaxIdCurrent;

  /* Outputs for Atomic SubSystem: '<S23>/PID_fw' */
  PID_fw(&PID_FW, &PID_FW);

  /* End of Outputs for SubSystem: '<S23>/PID_fw' */

  /* Sum: '<S82>/Add1' incorporates:
   *  Bias: '<S82>/Bias'
   *  Constant: '<S73>/Constant3'
   *  Constant: '<S82>/Constant'
   *  Constant: '<S82>/Constant1'
   *  DataStoreRead: '<S82>/Data Store Read1'
   *  Product: '<S82>/Divide'
   *  Product: '<S82>/Product'
   *  Sum: '<S82>/Add'
   */
  Uqz = 1.0F - ((0.0F - PID_FW.Out) * (1.0F / (SystemParameters.MinIdCurrent +
    1.0F)));

  /* Saturate: '<S82>/Saturation' */
  if (Uqz > 1.0F) {
    Uqz = 1.0F;
  } else if (Uqz < 0.0F) {
    Uqz = 0.0F;
  } else {
    /* no actions */
  }

  /* Product: '<S73>/Product' incorporates:
   *  Saturate: '<S82>/Saturation'
   */
  Vqff = Product_ce * Uqz;

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* Sum: '<S91>/Sum1' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   *  Product: '<S91>/Product'
   *  Product: '<S91>/Product1'
   *  UnitDelay: '<S209>/Unit Delay'
   */
  Product1_ck = (Dref * Dref) + (DW_l.UnitDelay_DSTATE * DW_l.UnitDelay_DSTATE);

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Outputs for IfAction SubSystem: '<S87>/D-Q Equivalence' incorporates:
   *  ActionPort: '<S88>/Action Port'
   */
  /* If: '<S88>/If' incorporates:
   *  If: '<S87>/If'
   *  Product: '<S90>/Product'
   *  RelationalOperator: '<S88>/Relational Operator'
   *  Switch: '<S90>/Switch'
   */
  if (Product1_ck >= (Vqff * Vqff)) {
    /* Outputs for IfAction SubSystem: '<S88>/Limiter' incorporates:
     *  ActionPort: '<S92>/Action Port'
     */
    /* Sqrt: '<S92>/Square Root' */
    Product1_ck = platform_sqrt(Product1_ck);

    /* Switch: '<S92>/Switch' incorporates:
     *  Constant: '<S92>/Constant'
     */
    if (Product1_ck == 0.0F) {
      Product1_ck = 1.0F;
    }

    /* Product: '<S92>/Reciprocal' incorporates:
     *  Switch: '<S92>/Switch'
     */
    Product1_ck = 1.0F / Product1_ck;

    /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
    /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
    /* Product: '<S92>/Product1' incorporates:
     *  BusAssignment: '<S199>/Bus Assignment'
     *  Product: '<S92>/Product'
     *  UnitDelay: '<S209>/Unit Delay'
     */
    limitCoefHigh_a = (Dref * Vqff) * Product1_ck;
    limitCoefHigh_g = (DW_l.UnitDelay_DSTATE * Vqff) * Product1_ck;

    /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
    /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
    /* End of Outputs for SubSystem: '<S88>/Limiter' */
  } else {
    /* Outputs for IfAction SubSystem: '<S88>/Passthrough' incorporates:
     *  ActionPort: '<S93>/Action Port'
     */
    /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
    /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
    /* SignalConversion generated from: '<S93>/dqRef' incorporates:
     *  BusAssignment: '<S199>/Bus Assignment'
     *  UnitDelay: '<S209>/Unit Delay'
     */
    limitCoefHigh_a = Dref;
    limitCoefHigh_g = DW_l.UnitDelay_DSTATE;

    /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
    /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
    /* End of Outputs for SubSystem: '<S88>/Passthrough' */
  }

  /* End of If: '<S88>/If' */
  /* End of Outputs for SubSystem: '<S87>/D-Q Equivalence' */

  /* Sum: '<S73>/Add' incorporates:
   *  BusAssignment: '<S78>/Bus Assignment'
   */
  Add = PID_FW.Out + limitCoefHigh_a;

  /* Switch: '<S83>/Switch2' incorporates:
   *  DataStoreRead: '<S73>/Data Store Read1'
   *  Gain: '<S77>/Gain'
   *  RelationalOperator: '<S83>/LowerRelop1'
   *  RelationalOperator: '<S83>/UpperRelop'
   *  Switch: '<S83>/Switch'
   */
  if (Add > SystemParameters.MaxCurrent) {
    Add = SystemParameters.MaxCurrent;
  } else if (Add < -SystemParameters.MaxCurrent) {
    /* Switch: '<S83>/Switch' incorporates:
     *  Gain: '<S77>/Gain'
     */
    Add = -SystemParameters.MaxCurrent;
  } else {
    /* no actions */
  }

  /* End of Switch: '<S83>/Switch2' */

  /* Outputs for Atomic SubSystem: '<S80>/If Action Subsystem2' */
  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
  /* Product: '<S106>/Divide' incorporates:
   *  Abs: '<S202>/Abs'
   *  Bias: '<S106>/Bias1'
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S78>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S4>/Data Store Read'
   *  Gain: '<S106>/Gain1'
   *  Product: '<S106>/Product'
   */
  Product1_ck = (0.578034699F * Control_f5_UdcFiltered) / ((Udz *
    Control.motorParams.motorLq) + 1.0E-5F);

  /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Switch: '<S107>/Switch2' incorporates:
   *  Constant: '<S106>/Constant'
   *  RelationalOperator: '<S107>/LowerRelop1'
   *  RelationalOperator: '<S107>/UpperRelop'
   *  Switch: '<S107>/Switch'
   */
  if (Product1_ck > Product_ce) {
    Product1_ck = Product_ce;
  } else if (Product1_ck < 0.0F) {
    /* Switch: '<S107>/Switch' incorporates:
     *  Constant: '<S106>/Constant'
     */
    Product1_ck = 0.0F;
  } else {
    /* no actions */
  }

  /* MinMax: '<S73>/Min' incorporates:
   *  DataStoreRead: '<S73>/Data Store Read1'
   *  Product: '<S85>/Product'
   *  Product: '<S85>/Product1'
   *  Sqrt: '<S85>/Sqrt'
   *  Sum: '<S85>/Add'
   *  Switch: '<S107>/Switch2'
   */
  Product_ce = platform_min(platform_sqrt((SystemParameters.MaxCurrent *
    SystemParameters.MaxCurrent) - (Add * Add)), Product1_ck);

  /* End of Outputs for SubSystem: '<S80>/If Action Subsystem2' */

  /* Switch: '<S108>/Switch2' incorporates:
   *  BusAssignment: '<S78>/Bus Assignment'
   *  RelationalOperator: '<S108>/LowerRelop1'
   */
  if (limitCoefHigh_g <= Product_ce) {
    /* Switch: '<S108>/Switch' incorporates:
     *  Gain: '<S81>/Gain'
     *  RelationalOperator: '<S108>/UpperRelop'
     */
    if (limitCoefHigh_g < -Product_ce) {
      Product_ce = -Product_ce;
    } else {
      Product_ce = limitCoefHigh_g;
    }

    /* End of Switch: '<S108>/Switch' */
  }

  /* End of Switch: '<S108>/Switch2' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
  /* Product: '<S76>/Product' incorporates:
   *  Abs: '<S202>/Abs'
   *  BusAssignment: '<S18>/Bus Assignment'
   *  BusAssignment: '<S75>/Bus Assignment'
   *  Gain: '<S76>/Gain'
   *  Sum: '<S76>/Add'
   */
  Uqz = (1.0E-6F * Udz) * (Product_ce - Control_lb_Iq);

  /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Saturate: '<S76>/Saturation' */
  if (Uqz > 100.0F) {
    Uqz = 100.0F;
  } else if (Uqz < 0.0F) {
    Uqz = 0.0F;
  } else {
    /* no actions */
  }

  /* Sum: '<S76>/Add1' incorporates:
   *  Saturate: '<S76>/Saturation'
   */
  Product1_ck = Add - Uqz;

  /* Sum: '<S105>/Add1' incorporates:
   *  Bias: '<S73>/Bias'
   *  Constant: '<S105>/Filter_Constant'
   *  Constant: '<S105>/One'
   *  Product: '<S105>/Product'
   *  Product: '<S105>/Product1'
   *  UnitDelay: '<S105>/Unit Delay'
   */
  DW_l.UnitDelay_DSTATE_e = ((Vqff + 2.0F) * 0.05F) + (0.95F *
    DW_l.UnitDelay_DSTATE_e);

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* If: '<S22>/If' incorporates:
   *  BusAssignment: '<S199>/Bus Assignment'
   */
  if (voltageControl) {
    /* Outputs for IfAction SubSystem: '<S22>/If Action Subsystem' incorporates:
     *  ActionPort: '<S25>/Action Port'
     */
    /* Outputs for Atomic SubSystem: '<S27>/HandModeVoltage' */
    /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
    /* Switch: '<S30>/Switch2' incorporates:
     *  BusAssignment: '<S198>/Bus Assignment'
     */
    if (RelationalOperator1_a) {
      /* Sum: '<S29>/Add' incorporates:
       *  BusAssignment: '<S8>/Bus Assignment'
       *  DataStoreRead: '<S28>/Data Store Read'
       *  DataStoreRead: '<S4>/Data Store Read'
       *  Product: '<S28>/Product'
       *  UnitDelay: '<S29>/Unit Delay'
       */
      debugAngle += TestRefSignals.WelTest * Control.Tpwm;

      /* Switch: '<S30>/Switch' */
      if (debugAngle > 6.28318548F) {
        /* Sum: '<S29>/Add' incorporates:
         *  Bias: '<S30>/Bias'
         */
        debugAngle -= TwoPI;
      }

      /* End of Switch: '<S30>/Switch' */

      /* Switch: '<S30>/Switch1' */
      if (debugAngle <= 0.0F) {
        /* Sum: '<S29>/Add' incorporates:
         *  Bias: '<S30>/Bias1'
         *  UnitDelay: '<S29>/Unit Delay'
         */
        debugAngle += TwoPI;
      }

      /* End of Switch: '<S30>/Switch1' */
    } else {
      /* Sum: '<S29>/Add' incorporates:
       *  Constant: '<S30>/Constant'
       *  UnitDelay: '<S29>/Unit Delay'
       */
      debugAngle = TwoPI;
    }

    /* End of Switch: '<S30>/Switch2' */
    /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
    /* End of Outputs for SubSystem: '<S27>/HandModeVoltage' */

    /* Merge generated from: '<S22>/Merge' incorporates:
     *  DataStoreRead: '<S28>/Data Store Read'
     *  SignalConversion generated from: '<S25>/Out1'
     */
    Udz = TestRefSignals.VdTest;

    /* Merge generated from: '<S22>/Merge' incorporates:
     *  DataStoreRead: '<S28>/Data Store Read'
     *  SignalConversion generated from: '<S25>/Out1'
     */
    Uqz = TestRefSignals.VqTest;

    /* Merge generated from: '<S22>/Merge' incorporates:
     *  SignalConversion generated from: '<S25>/Out1'
     *  UnitDelay: '<S29>/Unit Delay'
     */
    Switch1_e = debugAngle;

    /* End of Outputs for SubSystem: '<S22>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S22>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S26>/Action Port'
     */
    /* BusAssignment: '<S32>/Bus Assignment' incorporates:
     *  BusAssignment: '<S15>/Bus Assignment'
     *  BusAssignment: '<S18>/Bus Assignment'
     *  BusAssignment: '<S24>/Bus Assignment'
     *  BusAssignment: '<S78>/Bus Assignment'
     *  BusAssignment: '<S8>/Bus Assignment'
     *  DataStoreRead: '<S32>/Data Store Read1'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  Gain: '<S32>/Gain'
     *  Product: '<S32>/Product2'
     *  Sum: '<S32>/Add'
     */
    PID_ID.Ref = Product1_ck;

    /* Outputs for Atomic SubSystem: '<S7>/CurrentCalculations' */
    PID_ID.Fdb = Product1_il;

    /* End of Outputs for SubSystem: '<S7>/CurrentCalculations' */
    PID_ID.OutMax = Control_f5_UdcFiltered;
    PID_ID.OutMin = -Control_f5_UdcFiltered;
    PID_ID.defaultBit = PID_FW_tmp;
    PID_ID.SatErr = Control.UdRef - Control.Ud;
    PID_ID.Kp = Control.motorParams.motorLd * SystemParameters.BW_reg_Inv;

    /* Product: '<S32>/Product1' incorporates:
     *  BusAssignment: '<S8>/Bus Assignment'
     *  DataStoreRead: '<S32>/Data Store Read1'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  Product: '<S32>/Product'
     *  Product: '<S33>/Product1'
     */
    Vqff = (Control.motorParams.motorRs * Control.Tpwm) *
      SystemParameters.BW_reg_Inv;

    /* BusAssignment: '<S32>/Bus Assignment' incorporates:
     *  Product: '<S32>/Product1'
     */
    PID_ID.Ki = Vqff;

    /* Outputs for Atomic SubSystem: '<S32>/PID_Id' */
    PID_Id(&PID_ID, &PID_ID);

    /* End of Outputs for SubSystem: '<S32>/PID_Id' */

    /* Switch: '<S34>/Switch' incorporates:
     *  BusAssignment: '<S18>/Bus Assignment'
     *  BusAssignment: '<S75>/Bus Assignment'
     *  BusAssignment: '<S8>/Bus Assignment'
     *  Constant: '<S34>/Constant'
     *  DataStoreRead: '<S34>/Data Store Read'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  Gain: '<S34>/Gain'
     *  Gain: '<S59>/Gain'
     *  Product: '<S59>/Product'
     */
    if (SystemParameters.DecouplingEnable > 0U) {
      Udz = 1.3F * -((Control_lb_Iq * Control.motorParams.motorLq) *
                     Control.Welectrical);
    } else {
      Udz = 0.0F;
    }

    /* Merge generated from: '<S22>/Merge' incorporates:
     *  Sum: '<S31>/Add'
     *  Switch: '<S34>/Switch'
     */
    Udz += PID_ID.Out;

    /* BusAssignment: '<S33>/Bus Assignment' incorporates:
     *  BusAssignment: '<S15>/Bus Assignment'
     *  BusAssignment: '<S18>/Bus Assignment'
     *  BusAssignment: '<S24>/Bus Assignment'
     *  BusAssignment: '<S75>/Bus Assignment'
     *  BusAssignment: '<S78>/Bus Assignment'
     *  BusAssignment: '<S8>/Bus Assignment'
     *  DataStoreRead: '<S33>/Data Store Read1'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  Gain: '<S33>/Gain'
     *  Product: '<S33>/Product2'
     *  Sum: '<S33>/Add'
     */
    PID_IQ.Ref = Product_ce;
    PID_IQ.Fdb = Control_lb_Iq;
    PID_IQ.OutMax = Control_f5_UdcFiltered;
    PID_IQ.OutMin = -Control_f5_UdcFiltered;
    PID_IQ.defaultBit = PID_FW_tmp;
    PID_IQ.SatErr = Control.UqRef - Control.Uq;
    PID_IQ.Kp = Control.motorParams.motorLq * SystemParameters.BW_reg_Inv;
    PID_IQ.Ki = Vqff;

    /* Outputs for Atomic SubSystem: '<S33>/PID_Iq' */
    PID_Iq(&PID_IQ, &PID_IQ);

    /* End of Outputs for SubSystem: '<S33>/PID_Iq' */

    /* Switch: '<S34>/Switch1' incorporates:
     *  BusAssignment: '<S18>/Bus Assignment'
     *  BusAssignment: '<S8>/Bus Assignment'
     *  Constant: '<S34>/Constant'
     *  DataStoreRead: '<S34>/Data Store Read'
     *  DataStoreRead: '<S4>/Data Store Read'
     *  Gain: '<S34>/Gain1'
     *  Product: '<S59>/Product1'
     *  Product: '<S59>/Product2'
     *  Sum: '<S59>/Add'
     */
    if (SystemParameters.DecouplingEnable > 0U) {
      /* Outputs for Atomic SubSystem: '<S7>/CurrentCalculations' */
      Vqff = 1.3F * ((Control.Welectrical * Control.motorParams.motorEmf) +
                     (Control.motorParams.motorLd * Product1_il));

      /* End of Outputs for SubSystem: '<S7>/CurrentCalculations' */
    } else {
      Vqff = 0.0F;
    }

    /* End of Switch: '<S34>/Switch1' */

    /* Switch: '<S60>/Switch2' incorporates:
     *  RelationalOperator: '<S60>/LowerRelop1'
     *  RelationalOperator: '<S60>/UpperRelop'
     *  Switch: '<S60>/Switch'
     */
    if (Vqff > 0.0F) {
      Vqff = 0.0F;
    } else if (Vqff < 0.0F) {
      /* Switch: '<S60>/Switch' */
      Vqff = 0.0F;
    } else {
      /* no actions */
    }

    /* Merge generated from: '<S22>/Merge' incorporates:
     *  Sum: '<S31>/Add1'
     *  Switch: '<S60>/Switch2'
     */
    Uqz = PID_IQ.Out + Vqff;

    /* End of Outputs for SubSystem: '<S22>/If Action Subsystem1' */
  }

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* BusAssignment: '<S14>/Bus Assignment' incorporates:
   *  BusAssignment: '<S24>/Bus Assignment'
   */
  BusAssignment_o_IqRefReg = Product_ce;

  /* Outputs for Atomic SubSystem: '<S7>/VoltageLimiter' */
  /* Sum: '<S16>/Add' incorporates:
   *  BusAssignment: '<S14>/Bus Assignment'
   *  Constant: '<S16>/Constant'
   */
  Add = Udz + VdPlay;

  /* Sum: '<S16>/Add1' incorporates:
   *  BusAssignment: '<S14>/Bus Assignment'
   *  Constant: '<S16>/Constant1'
   */
  Product_ce = Uqz + VqPlay;

  /* If: '<S145>/If' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S78>/Bus Assignment'
   *  Gain: '<S145>/Gain1'
   *  RelationalOperator: '<S145>/Relational Operator'
   *  RelationalOperator: '<S145>/Relational Operator1'
   */
  if (Add > Control_f5_UdcFiltered) {
    /* Outputs for IfAction SubSystem: '<S145>/If Action Subsystem' incorporates:
     *  ActionPort: '<S147>/Action Port'
     */
    /* SignalConversion generated from: '<S147>/In1' */
    Add = Control_f5_UdcFiltered;

    /* End of Outputs for SubSystem: '<S145>/If Action Subsystem' */
  } else if (Add < -Control_f5_UdcFiltered) {
    /* Outputs for IfAction SubSystem: '<S145>/If Action Subsystem2' incorporates:
     *  ActionPort: '<S149>/Action Port'
     */
    /* SignalConversion generated from: '<S149>/In1' incorporates:
     *  Gain: '<S145>/Gain1'
     */
    Add = -Control_f5_UdcFiltered;

    /* End of Outputs for SubSystem: '<S145>/If Action Subsystem2' */
  } else {
    /* no actions */
  }

  /* End of If: '<S145>/If' */
  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */

  /* Product: '<S142>/Product' incorporates:
   *  Product: '<S144>/Product'
   */
  Product_c_tmp = Add * Add;

  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  /* Sqrt: '<S144>/Sqrt' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S78>/Bus Assignment'
   *  Product: '<S144>/Product1'
   *  Sum: '<S144>/Add'
   */
  Vqff = platform_sqrt((Control_f5_UdcFiltered * Control_f5_UdcFiltered) -
                       Product_c_tmp);

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */

  /* If: '<S146>/If' incorporates:
   *  Gain: '<S146>/Gain1'
   *  RelationalOperator: '<S146>/Relational Operator'
   *  RelationalOperator: '<S146>/Relational Operator1'
   */
  if (Product_ce > Vqff) {
    /* Outputs for IfAction SubSystem: '<S146>/If Action Subsystem' incorporates:
     *  ActionPort: '<S150>/Action Port'
     */
    /* SignalConversion generated from: '<S150>/In1' */
    Product_ce = Vqff;

    /* End of Outputs for SubSystem: '<S146>/If Action Subsystem' */
  } else if (Product_ce < -Vqff) {
    /* Outputs for IfAction SubSystem: '<S146>/If Action Subsystem2' incorporates:
     *  ActionPort: '<S152>/Action Port'
     */
    /* SignalConversion generated from: '<S152>/In1' incorporates:
     *  Gain: '<S146>/Gain1'
     */
    Product_ce = -Vqff;

    /* End of Outputs for SubSystem: '<S146>/If Action Subsystem2' */
  } else {
    /* no actions */
  }

  /* End of If: '<S146>/If' */

  /* BusAssignment: '<S16>/Bus Assignment' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   */
  Control_f5_UdRef = Add;

  /* End of Outputs for SubSystem: '<S7>/VoltageLimiter' */

  /* Outputs for Atomic SubSystem: '<S7>/VoltageControl' */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  /* Fcn: '<S15>/Lag Correction' incorporates:
   *  BusAssignment: '<S14>/Bus Assignment'
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  DataStoreRead: '<S15>/Data Store Read1'
   *  DataStoreRead: '<S4>/Data Store Read'
   */
  Vqff = Switch1_e + ((Qref * Control.Tpwm) * SystemParameters.LagCorrection);

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */

  /* Switch: '<S140>/Switch' incorporates:
   *  Bias: '<S140>/Bias'
   */
  if (Vqff > 6.28318548F) {
    Vqff -= 6.28318548F;
  }

  /* End of Switch: '<S140>/Switch' */

  /* Switch: '<S140>/Switch1' incorporates:
   *  Bias: '<S140>/Bias1'
   */
  if (Vqff < 0.0F) {
    Vqff += 6.28318548F;
  }

  /* End of Switch: '<S140>/Switch1' */

  /* Trigonometry: '<S139>/Sin' */
  Add = platform_cos(Vqff);

  /* Trigonometry: '<S139>/Sin1' */
  Vqff = platform_sin(Vqff);

  /* Outputs for Atomic SubSystem: '<S4>/PositionDataUpdate' */
  /* Outputs for Atomic SubSystem: '<S7>/VoltageLimiter' */
  /* BusAssignment: '<S9>/Bus Assignment1' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S16>/Bus Assignment'
   *  DataStoreWrite: '<S4>/Data Store Write'
   *  Product: '<S139>/Product'
   *  Product: '<S139>/Product1'
   *  Sum: '<S139>/Add'
   */
  Control.Ualpha = (Control_f5_UdRef * Add) - (Product_ce * Vqff);

  /* End of Outputs for SubSystem: '<S4>/PositionDataUpdate' */

  /* Product: '<S139>/Product3' incorporates:
   *  BusAssignment: '<S16>/Bus Assignment'
   */
  Add *= Product_ce;

  /* End of Outputs for SubSystem: '<S7>/VoltageLimiter' */

  /* Outputs for Atomic SubSystem: '<S4>/PositionDataUpdate' */
  /* BusAssignment: '<S9>/Bus Assignment1' incorporates:
   *  BusAssignment: '<S15>/Bus Assignment'
   *  DataStoreWrite: '<S4>/Data Store Write'
   *  Product: '<S139>/Product2'
   *  Sum: '<S139>/Add1'
   */
  Control.Ubeta = (Control_f5_UdRef * Vqff) + Add;

  /* End of Outputs for SubSystem: '<S4>/PositionDataUpdate' */
  /* End of Outputs for SubSystem: '<S7>/VoltageControl' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/RawAngleCalc' */
  /* SwitchCase: '<S11>/Switch Case' incorporates:
   *  DataStoreRead: '<S11>/Data Store Read1'
   */
  switch (SystemParameters.ActivePosSensor) {
   case ResolverType:
    {
      real32_T Merge1;

      /* Outputs for IfAction SubSystem: '<S11>/ResolverSensor' incorporates:
       *  ActionPort: '<S177>/Action Port'
       */
      /* If: '<S193>/If' incorporates:
       *  DataStoreRead: '<S193>/Data Store Read'
       */
      if (SystemParameters.ResolverSignalSwap > 0U) {
        /* Outputs for IfAction SubSystem: '<S193>/If Action Subsystem' incorporates:
         *  ActionPort: '<S196>/Action Port'
         */
        /* SignalConversion generated from: '<S196>/Sine' incorporates:
         *  Inport: '<Root>/inSignals'
         *  SignalConversion generated from: '<S177>/Bus Selector'
         */
        Add = inSignals.ResolverSine;

        /* SignalConversion generated from: '<S196>/Cosine' incorporates:
         *  Inport: '<Root>/inSignals'
         *  SignalConversion generated from: '<S177>/Bus Selector'
         */
        Merge1 = inSignals.ResolverCosine;

        /* End of Outputs for SubSystem: '<S193>/If Action Subsystem' */
      } else {
        /* Outputs for IfAction SubSystem: '<S193>/If Action Subsystem1' incorporates:
         *  ActionPort: '<S197>/Action Port'
         */
        /* SignalConversion generated from: '<S197>/Cosine' incorporates:
         *  Inport: '<Root>/inSignals'
         *  SignalConversion generated from: '<S177>/Bus Selector'
         */
        Add = inSignals.ResolverCosine;

        /* SignalConversion generated from: '<S197>/Sine' incorporates:
         *  Inport: '<Root>/inSignals'
         *  SignalConversion generated from: '<S177>/Bus Selector'
         */
        Merge1 = inSignals.ResolverSine;

        /* End of Outputs for SubSystem: '<S193>/If Action Subsystem1' */
      }

      /* End of If: '<S193>/If' */

      /* If: '<S190>/If' incorporates:
       *  DataStoreRead: '<S190>/Data Store Read'
       */
      if (SystemParameters.phase_inverse > 0U) {
        /* Outputs for IfAction SubSystem: '<S190>/If Action Subsystem' incorporates:
         *  ActionPort: '<S191>/Action Port'
         */
        /* Sum: '<S191>/Add' incorporates:
         *  Constant: '<S191>/Constant'
         *  Product: '<S190>/Product'
         *  Trigonometry: '<S190>/Atan1'
         */
        Vqff = TwoPI - (SystemParameters.PosSensorResolution * platform_atan2
                        (Add, Merge1));

        /* End of Outputs for SubSystem: '<S190>/If Action Subsystem' */
      } else {
        /* Outputs for IfAction SubSystem: '<S190>/If Action Subsystem1' incorporates:
         *  ActionPort: '<S192>/Action Port'
         */
        /* SignalConversion generated from: '<S192>/rawTheta' incorporates:
         *  Product: '<S190>/Product'
         *  Trigonometry: '<S190>/Atan1'
         */
        Vqff = SystemParameters.PosSensorResolution * platform_atan2(Add, Merge1);

        /* End of Outputs for SubSystem: '<S190>/If Action Subsystem1' */
      }

      /* End of If: '<S190>/If' */

      /* Sum: '<S194>/Add' incorporates:
       *  Product: '<S194>/Product'
       *  Product: '<S194>/Product1'
       */
      ResSignalAmpl = (Add * Add) + (Merge1 * Merge1);

      /* Switch: '<S195>/Switch' incorporates:
       *  Bias: '<S195>/Bias'
       */
      if (Vqff > 6.28318548F) {
        Vqff -= 6.28318548F;
      }

      /* End of Switch: '<S195>/Switch' */

      /* Switch: '<S195>/Switch1' */
      if (Vqff >= 0.0F) {
        /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
         *
         * Block description for '<S4>/Position_Estimator':
         *  Функция выполняет обработку счетчика переключений холлов для получения информации
         *   о положении вала электродвигателя.

         */
        /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
         *  Merge: '<S11>/Merge'
         */
        PositionObserver.SensorAngle_raw = Vqff;

        /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
      } else {
        /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
         *
         * Block description for '<S4>/Position_Estimator':
         *  Функция выполняет обработку счетчика переключений холлов для получения информации
         *   о положении вала электродвигателя.

         */
        /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
         *  Bias: '<S195>/Bias1'
         *  Merge: '<S11>/Merge'
         */
        PositionObserver.SensorAngle_raw = Vqff + 6.28318548F;

        /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
      }

      /* End of Switch: '<S195>/Switch1' */
      /* End of Outputs for SubSystem: '<S11>/ResolverSensor' */
    }
    break;

   case HallType:
    /* Outputs for IfAction SubSystem: '<S11>/HallSensor' incorporates:
     *  ActionPort: '<S176>/Action Port'
     */
    /* If: '<S183>/If' incorporates:
     *  DataStoreRead: '<S183>/Data Store Read'
     */
    if (SystemParameters.phase_inverse > 0U) {
      /* Outputs for IfAction SubSystem: '<S183>/If Action Subsystem2' incorporates:
       *  ActionPort: '<S185>/Action Port'
       */
      /* MATLAB Function: '<S188>/getHallIndex' incorporates:
       *  DataStoreRead: '<S188>/Data Store Read'
       *  Inport: '<Root>/inSignals'
       */
      getHallIndex(SystemParameters.hallTableDirect, inSignals.HallCnt,
                   &hallindex);

      /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
       *
       * Block description for '<S4>/Position_Estimator':
       *  Функция выполняет обработку счетчика переключений холлов для получения информации
       *   о положении вала электродвигателя.

       */
      /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
       *  Constant: '<S188>/Constant'
       *  DataStoreRead: '<S188>/Data Store Read'
       *  Merge: '<S11>/Merge'
       *  Product: '<S188>/Product'
       *  Sum: '<S188>/Add'
       */
      PositionObserver.SensorAngle_raw = TwoPI - ((real32_T)hallindex *
        SystemParameters.PosSensorResolution);

      /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
      /* End of Outputs for SubSystem: '<S183>/If Action Subsystem2' */
    } else {
      /* Outputs for IfAction SubSystem: '<S183>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S184>/Action Port'
       */
      /* MATLAB Function: '<S186>/getHallIndex' incorporates:
       *  DataStoreRead: '<S186>/Data Store Read'
       *  Inport: '<Root>/inSignals'
       */
      getHallIndex(SystemParameters.hallTableDirect, inSignals.HallCnt,
                   &hallindex);

      /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
       *
       * Block description for '<S4>/Position_Estimator':
       *  Функция выполняет обработку счетчика переключений холлов для получения информации
       *   о положении вала электродвигателя.

       */
      /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
       *  DataStoreRead: '<S186>/Data Store Read'
       *  Merge: '<S11>/Merge'
       *  Product: '<S186>/Product'
       */
      PositionObserver.SensorAngle_raw = (real32_T)hallindex *
        SystemParameters.PosSensorResolution;

      /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
      /* End of Outputs for SubSystem: '<S183>/If Action Subsystem1' */
    }

    /* End of If: '<S183>/If' */
    /* End of Outputs for SubSystem: '<S11>/HallSensor' */
    break;

   case EncoderType:
    /* Outputs for IfAction SubSystem: '<S11>/EncoderSensor' incorporates:
     *  ActionPort: '<S175>/Action Port'
     */
    /* If: '<S178>/If' incorporates:
     *  DataStoreRead: '<S178>/Data Store Read'
     */
    if (SystemParameters.phase_inverse > 0U) {
      /* Outputs for IfAction SubSystem: '<S178>/If Action Subsystem2' incorporates:
       *  ActionPort: '<S180>/Action Port'
       */
      /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
       *
       * Block description for '<S4>/Position_Estimator':
       *  Функция выполняет обработку счетчика переключений холлов для получения информации
       *   о положении вала электродвигателя.

       */
      /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
       *  Constant: '<S182>/Constant'
       *  DataStoreRead: '<S182>/Data Store Read'
       *  Inport: '<Root>/inSignals'
       *  Merge: '<S11>/Merge'
       *  Product: '<S182>/Product'
       *  Sum: '<S182>/Add'
       */
      PositionObserver.SensorAngle_raw = TwoPI -
        (SystemParameters.PosSensorResolution * (real32_T)inSignals.EncCnt);

      /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
      /* End of Outputs for SubSystem: '<S178>/If Action Subsystem2' */
    } else {
      /* Outputs for IfAction SubSystem: '<S178>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S179>/Action Port'
       */
      /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
       *
       * Block description for '<S4>/Position_Estimator':
       *  Функция выполняет обработку счетчика переключений холлов для получения информации
       *   о положении вала электродвигателя.

       */
      /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
       *  DataStoreRead: '<S181>/Data Store Read'
       *  Inport: '<Root>/inSignals'
       *  Merge: '<S11>/Merge'
       *  Product: '<S181>/Product'
       */
      PositionObserver.SensorAngle_raw = SystemParameters.PosSensorResolution *
        (real32_T)inSignals.EncCnt;

      /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
      /* End of Outputs for SubSystem: '<S178>/If Action Subsystem1' */
    }

    /* End of If: '<S178>/If' */
    /* End of Outputs for SubSystem: '<S11>/EncoderSensor' */
    break;

   default:
    /* Outputs for IfAction SubSystem: '<S11>/DebugAngle' incorporates:
     *  ActionPort: '<S174>/Action Port'
     */
    /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
     *
     * Block description for '<S4>/Position_Estimator':
     *  Функция выполняет обработку счетчика переключений холлов для получения информации
     *   о положении вала электродвигателя.

     */
    /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
     *  DataStoreRead: '<S11>/Data Store Read'
     *  DataTypeConversion: '<S174>/Data Type Conversion'
     *  Merge: '<S11>/Merge'
     */
    PositionObserver.SensorAngle_raw = TestRefSignals.ThetaElTest;

    /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */
    /* End of Outputs for SubSystem: '<S11>/DebugAngle' */
    break;
  }

  /* End of SwitchCase: '<S11>/Switch Case' */
  /* End of Outputs for SubSystem: '<S4>/RawAngleCalc' */

  /* Outputs for Atomic SubSystem: '<S4>/Position_Estimator'
   *
   * Block description for '<S4>/Position_Estimator':
   *  Функция выполняет обработку счетчика переключений холлов для получения информации
   *   о положении вала электродвигателя.

   */
  /* BusAssignment: '<S10>/Bus Assignment1' incorporates:
   *  DataStoreRead: '<S10>/Data Store Read'
   */
  PositionObserver.L0 = SystemParameters.Observer_LO;

  /* Outputs for Atomic SubSystem: '<S10>/Position_observer' */
  Position_Observer(&PositionObserver, &PositionObserver);

  /* End of Outputs for SubSystem: '<S10>/Position_observer' */
  /* End of Outputs for SubSystem: '<S4>/Position_Estimator' */

  /* Outputs for Atomic SubSystem: '<S4>/PositionDataUpdate' */
  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  /* BusAssignment: '<S9>/Bus Assignment1' incorporates:
   *  BusAssignment: '<S14>/Bus Assignment'
   *  BusAssignment: '<S15>/Bus Assignment'
   *  BusAssignment: '<S16>/Bus Assignment'
   *  BusAssignment: '<S18>/Bus Assignment'
   *  BusAssignment: '<S198>/Bus Assignment'
   *  BusAssignment: '<S199>/Bus Assignment'
   *  BusAssignment: '<S200>/Bus Assignment'
   *  BusAssignment: '<S24>/Bus Assignment'
   *  BusAssignment: '<S75>/Bus Assignment'
   *  BusAssignment: '<S78>/Bus Assignment'
   *  BusAssignment: '<S8>/Bus Assignment'
   *  BusCreator generated from: '<S75>/Bus Assignment'
   *  Constant: '<S74>/Constant'
   *  DataStoreRead: '<S206>/TestRefSignals'
   *  DataStoreRead: '<S236>/Data Store Read'
   *  DataStoreWrite: '<S4>/Data Store Write'
   *  Gain: '<S236>/Gain'
   *  Inport: '<Root>/inSignals'
   *  Logic: '<S115>/OR'
   *  Product: '<S141>/Product'
   *  Product: '<S141>/Product1'
   *  Product: '<S142>/Product'
   *  Product: '<S142>/Product1'
   *  Product: '<S236>/Product'
   *  Product: '<S236>/Product1'
   *  Product: '<S236>/Product2'
   *  RelationalOperator: '<S74>/Compare'
   *  Sqrt: '<S141>/Sqrt'
   *  Sqrt: '<S142>/Sqrt'
   *  Sqrt: '<S20>/Sqrt'
   *  Sum: '<S141>/Add'
   *  Sum: '<S142>/Add'
   *  Sum: '<S20>/Add'
   *  Sum: '<S236>/Add'
   *  UnitDelay: '<S105>/Unit Delay'
   *  UnitDelay: '<S209>/Unit Delay'
   */
  Control.command.modOn = internalRef.onSignal;
  Control.command.clrFault = internalRef.clrErrSignal;
  Control.command.fixedAngle = fixedAngle;
  Control.command.voltageControl = voltageControl;
  Control.stat.testDone = TestRefSignals.testDone;
  Control.stat.testActive = TestRefSignals.testActive;

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.stat.ready = Control_lb_stat_ready;

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/cmdONlogic' */
  Control.stat.mod_Active = RelationalOperator1_a;

  /* End of Outputs for SubSystem: '<S12>/cmdONlogic' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  Control.stat.globalFault = Errors.GlobalError;

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.stat.driveMode = Control_lb_stat_driveMode;
  Control.stat.saturationActive = (RelationalOperator_o || RelationalOperator_al
    || RelationalOperator_h || RelationalOperator1 || NOT);
  Control.stat.fwActive = (PID_FW.Out <= -2.0F);

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */
  Control.errors = Errors;

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.actLimits.udcLimitFlg = RelationalOperator_o;
  Control.actLimits.igbtTlimFlg = RelationalOperator_al;
  Control.actLimits.motorTlimFlg = RelationalOperator_h;
  Control.actLimits.speedLimFlg = RelationalOperator1;
  Control.actLimits.idcLimitFlg = NOT;

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  Control.Ia = Merge_k5_idx_0;
  Control.Ib = Merge_k5_idx_1;
  Control.Ic = Merge_k5_idx_2;

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  Control.Ibeta = Control_f5_Ibeta;
  Control.Ialpha = Control_f5_Ialpha;

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentCalculations' */
  Control.Iampl = platform_sqrt(Product_g3 + Product1_f);

  /* End of Outputs for SubSystem: '<S7>/CurrentCalculations' */

  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.ImaxRef = DW_l.UnitDelay_DSTATE_e;

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  Control.IqRefMTPA = DW_l.UnitDelay_DSTATE;

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.IqRefSat = limitCoefHigh_g;

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */

  /* Outputs for Atomic SubSystem: '<S7>/VoltageLimiter' */
  Control.IqRefReg = BusAssignment_o_IqRefReg;

  /* End of Outputs for SubSystem: '<S7>/VoltageLimiter' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  Control.IdRefMTPA = Dref;

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.IdRefSat = limitCoefHigh_a;
  Control.IdRefReg = Product1_ck;
  Control.Iq = Control_lb_Iq;

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */

  /* Outputs for Atomic SubSystem: '<S7>/CurrentCalculations' */
  Control.Id = Product1_il;

  /* End of Outputs for SubSystem: '<S7>/CurrentCalculations' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  Control.Udc = inSignals.Vdc;

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  Control.UdcFiltered = Control_f5_UdcFiltered;

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/VoltageLimiter' */
  Control.UmodActual = platform_sqrt(Product_c_tmp + (Product_ce * Product_ce));

  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.UmodRef = platform_sqrt((Udz * Udz) + (Uqz * Uqz));

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */
  /* End of Outputs for SubSystem: '<S7>/VoltageLimiter' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */
  Control.UdRef = Control_f5_UdRef;

  /* Outputs for Atomic SubSystem: '<S4>/FOC_control'
   *
   * Block description for '<S4>/FOC_control':
   *  Алгоритм векторного управления
   */
  /* Outputs for Atomic SubSystem: '<S7>/VoltageLimiter' */
  Control.UqRef = Product_ce;

  /* End of Outputs for SubSystem: '<S7>/VoltageLimiter' */

  /* Outputs for Atomic SubSystem: '<S7>/CurrentRegulators' */
  Control.Ud = Udz;
  Control.Uq = Uqz;
  Control.ThetaElectrCorr = Switch1_e;

  /* End of Outputs for SubSystem: '<S7>/CurrentRegulators' */
  /* End of Outputs for SubSystem: '<S4>/FOC_control' */

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  Control.Idc = -((SVPWM.Ta * Merge_k5_idx_0) + (SVPWM.Tb * Merge_k5_idx_1) +
                  (SVPWM.Tc * Merge_k5_idx_2));

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  Control.WelectricalFilt = Qref;

  /* Outputs for Atomic SubSystem: '<S4>/inDataProcess' */
  /* Outputs for Atomic SubSystem: '<S12>/sensorDataProcess' */
  Control.Tmotor = inSignals.Tmotor;
  Control.TheatSink = inSignals.TheatSink;

  /* End of Outputs for SubSystem: '<S12>/sensorDataProcess' */

  /* Outputs for Atomic SubSystem: '<S12>/refCommndsProcess' */
  Control.Tigbt = Control_j_Tigbt;

  /* End of Outputs for SubSystem: '<S12>/refCommndsProcess' */
  /* End of Outputs for SubSystem: '<S4>/inDataProcess' */
  Control.ThetaElectrical = PositionObserver.ElectricalAngle_est;
  Control.Welectrical = PositionObserver.ElectricalSpeed_est;
  Control.Wmechanical = PositionObserver.MechanicalSpeed_est;

  /* End of Outputs for SubSystem: '<S4>/PositionDataUpdate' */

  /* Outputs for Atomic SubSystem: '<S1>/PWMcalc'
   *
   * Block description for '<S1>/PWMcalc':
   *  Функция формирует задания для
   *  компараторов ШИМ по алгоритму
   *  Векторного ШИМ.
   */
  /* DataStoreWrite: '<S4>/Data Store Write' */
  PWMcalc(Control.Ualpha, Control.Ubeta, Control.Udc, Control.stat.mod_Active,
          &Vqff, &Add, &Product_ce);

  /* End of Outputs for SubSystem: '<S1>/PWMcalc' */

  /* BusCreator generated from: '<S1>/outSignals1_BusCreator' incorporates:
   *  DataStoreWrite: '<S4>/Data Store Write'
   *  Outport: '<Root>/outSignals'
   */
  outSignals.clrFault = Control.command.clrFault;
  outSignals.globalFault = Control.stat.globalFault;
  outSignals.enable = Control.stat.mod_Active;

  /* If: '<S6>/If' incorporates:
   *  BusCreator generated from: '<S1>/outSignals1_BusCreator'
   *  DataStoreRead: '<S6>/Data Store Read'
   *  Outport: '<Root>/outSignals'
   *  SignalConversion generated from: '<S283>/In1'
   *  SignalConversion generated from: '<S284>/In1'
   */
  if (SystemParameters.phase_inverse > 0U) {
    /* Outputs for IfAction SubSystem: '<S6>/If Action Subsystem' incorporates:
     *  ActionPort: '<S283>/Action Port'
     */
    outSignals.refPwmA = Product_ce;

    /* End of Outputs for SubSystem: '<S6>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S6>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S284>/Action Port'
     */
    outSignals.refPwmA = Vqff;

    /* End of Outputs for SubSystem: '<S6>/If Action Subsystem1' */
  }

  /* Outputs for IfAction SubSystem: '<S6>/If Action Subsystem' incorporates:
   *  ActionPort: '<S283>/Action Port'
   */
  outSignals.refPwmB = Add;

  /* End of Outputs for SubSystem: '<S6>/If Action Subsystem' */
  if (SystemParameters.phase_inverse > 0U) {
    /* Outputs for IfAction SubSystem: '<S6>/If Action Subsystem' incorporates:
     *  ActionPort: '<S283>/Action Port'
     */
    outSignals.refPwmC = Vqff;

    /* End of Outputs for SubSystem: '<S6>/If Action Subsystem' */
  } else {
    /* Outputs for IfAction SubSystem: '<S6>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S284>/Action Port'
     */
    outSignals.refPwmC = Product_ce;

    /* End of Outputs for SubSystem: '<S6>/If Action Subsystem1' */
  }

  /* End of If: '<S6>/If' */

  /* Outport: '<Root>/Out1' incorporates:
   *  SignalConversion generated from: '<S1>/Out1'
   */
  Out1 = 0.0F;

  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/isrADC' */
}

/* Model step function */
void isrTIM(void)
{
  TestSignals_st Merge_c;

  /* RootInportFunctionCallGenerator generated from: '<Root>/isrTIM' incorporates:
   *  SubSystem: '<Root>/TIMisr'
   */
  /* Chart: '<S3>/Logic' incorporates:
   *  Inport: '<Root>/extRef'
   *  Outport: '<Root>/outSignals'
   */
  Logic(outSignals.globalFault, extRef.cmd, &DW_l.sf_Logic);

  /* Outputs for Atomic SubSystem: '<S3>/Identification' */
  Identification(DW_l.sf_Logic.onSignal_e, DW_l.sf_Logic.workMode, &Merge_c);

  /* End of Outputs for SubSystem: '<S3>/Identification' */

  /* DataStoreWrite: '<S3>/Data Store Write1' */
  TestRefSignals = Merge_c;

  /* Outputs for Atomic SubSystem: '<S3>/workModeChoise' */
  /* BusCreator: '<S3>/Bus Creator' incorporates:
   *  Inport: '<Root>/extRef'
   *  SignalConversion generated from: '<S3>/Bus Selector'
   */
  workModeChoise(extRef.refValue, extRef.limitHigh, extRef.limitLow,
                 DW_l.sf_Logic.regMode, DW_l.sf_Logic.clearErrSignal_p,
                 DW_l.sf_Logic.onSignal_e, &internalRef.ImRef,
                 &internalRef.onSignal, &internalRef.clrErrSignal,
                 &DW_l.workModeChoise_p);

  /* End of Outputs for SubSystem: '<S3>/workModeChoise' */
  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/isrTIM' */
}

/* Model initialize function */
void ControlSystem_v2_initialize(void)
{
  {
    real32_T Merge_cr;

    /* Start for DataStoreMemory: '<Root>/Data Store Memory' */
    TestRefSignals = ConstP_d.DataStoreMemory_InitialValue;

    /* Start for DataStoreMemory: '<Root>/Data Store Memory1' */
    SystemParameters = ConstP_d.pooled2;

    /* Start for DataStoreMemory: '<Root>/Data Store Memory2' */
    TripLevels = ConstP_d.pooled7;

    /* SystemInitialize for RootInportFunctionCallGenerator generated from: '<Root>/isrTIM' incorporates:
     *  SubSystem: '<Root>/TIMisr'
     */
    /* SystemInitialize for Atomic SubSystem: '<S3>/Identification' */
    Identification_Init();

    /* End of SystemInitialize for SubSystem: '<S3>/Identification' */
    /* End of SystemInitialize for RootInportFunctionCallGenerator generated from: '<Root>/isrTIM' */

    /* Outputs for Atomic SubSystem: '<Root>/Init' */
    /* If: '<S288>/If' incorporates:
     *  Constant: '<S288>/Constant7'
     *  RelationalOperator: '<S288>/Relational Operator'
     */
    if (SystemSetActual->version == 1UL) {
      /* Outputs for IfAction SubSystem: '<S288>/If Action Subsystem' incorporates:
       *  ActionPort: '<S296>/Action Port'
       */
      /* DataStoreWrite: '<S2>/Data Store Write1' incorporates:
       *  SignalConversion generated from: '<S296>/In1'
       */
      SystemParameters = *SystemSetActual;

      /* End of Outputs for SubSystem: '<S288>/If Action Subsystem' */
    } else {
      /* Outputs for IfAction SubSystem: '<S288>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S297>/Action Port'
       */
      /* DataStoreWrite: '<S2>/Data Store Write1' incorporates:
       *  Constant: '<S288>/Constant6'
       *  SignalConversion generated from: '<S297>/In1'
       */
      SystemParameters = ConstP_d.pooled2;

      /* End of Outputs for SubSystem: '<S288>/If Action Subsystem1' */
    }

    /* End of If: '<S288>/If' */

    /* BusAssignment: '<S289>/Bus Assignment5' incorporates:
     *  Constant: '<S289>/Constant'
     *  DataStoreRead: '<S289>/Data Store Read1'
     *  DataStoreWrite: '<S289>/Data Store Write'
     *  Gain: '<S289>/Gain'
     *  Gain: '<S289>/toH'
     *  Gain: '<S289>/toH2'
     *  Product: '<S289>/Divide'
     *  Product: '<S289>/Divide1'
     */
    (void)memset(&Control, 0, sizeof(Control_st));
    Control.Tpwm = 1.0F / SystemParameters.PwmBaseFrq;
    Control.motorParams.motorRs = SystemParameters.MotorParams.motorRs;
    Control.motorParams.motorLd = 1.0E-6F * SystemParameters.MotorParams.motorLd;
    Control.motorParams.motorLq = 1.0E-6F * SystemParameters.MotorParams.motorLq;
    Control.motorParams.motorPoles = SystemParameters.MotorParams.motorPoles;
    Control.motorParams.motorEmf = 0.577350259F *
      (SystemParameters.MotorParams.motorEmf / (real32_T)
       SystemParameters.MotorParams.motorPoles);
    Control.motorParams.motorKt = SystemParameters.MotorParams.motorEmf;
    Control.motorParams.motorType = SystemParameters.MotorParams.motorType;

    /* Math: '<S290>/Exp' incorporates:
     *  DataStoreRead: '<S290>/Data Store Read'
     *  DataStoreWrite: '<S289>/Data Store Write'
     *  DataStoreWrite: '<S290>/Data Store Write'
     *  Gain: '<S290>/Gain'
     *  Product: '<S290>/Divide'
     *
     * About '<S290>/Exp':
     *  Operator: exp
     */
    lpfUdc.k1 = platform_exp(-Control.Tpwm / SystemParameters.UdcFiltTp);

    /* Sum: '<S290>/Add1' incorporates:
     *  Constant: '<S290>/Constant2'
     *  DataStoreWrite: '<S290>/Data Store Write'
     */
    lpfUdc.k2 = 1.0F - lpfUdc.k1;

    /* DataStoreWrite: '<S290>/Data Store Write' incorporates:
     *  Constant: '<S290>/Constant2'
     */
    lpfUdc.filteredVal = 1.0F;

    /* Math: '<S290>/Exp1' incorporates:
     *  Constant: '<S290>/Constant'
     *  Constant: '<S290>/Constant3'
     *  DataStoreWrite: '<S290>/Data Store Write1'
     *  DataTypeConversion: '<S290>/Data Type Conversion6'
     *  Gain: '<S290>/Gain1'
     *  Product: '<S290>/Divide1'
     *
     * About '<S290>/Exp1':
     *  Operator: exp
     */
    lpfFluxErrorAngle.k1 = 0.980198681F;

    /* Sum: '<S290>/Add2' incorporates:
     *  Constant: '<S290>/Constant4'
     *  DataStoreWrite: '<S290>/Data Store Write1'
     */
    lpfFluxErrorAngle.k2 = 1.0F - lpfFluxErrorAngle.k1;

    /* DataStoreWrite: '<S290>/Data Store Write1' incorporates:
     *  Constant: '<S290>/Constant4'
     */
    lpfFluxErrorAngle.filteredVal = 1.0F;

    /* DataStoreWrite: '<S289>/Data Store Write1' incorporates:
     *  Constant: '<S289>/Constant1'
     */
    TripLevels = ConstP_d.pooled7;

    /* BusAssignment: '<S285>/Bus Assignment6' incorporates:
     *  Constant: '<S285>/PRD_VAL_MAX'
     *  Constant: '<S285>/Ualpha'
     *  Constant: '<S285>/Ubeta'
     *  DataStoreWrite: '<S285>/Data Store Write'
     */
    (void)memset(&SVPWM, 0, sizeof(PWM_st));
    SVPWM.PRD_VAL = 1.0F;
    SVPWM.Ualpha = 0.0F;
    SVPWM.Ubeta = 0.0F;

    /* End of Outputs for SubSystem: '<Root>/Init' */

    /* BusAssignment: '<S35>/Bus Assignment' */
    (void)memset(&PID_ID, 0, sizeof(PID_st));

    /* Outputs for Atomic SubSystem: '<Root>/Init' */
    /* BusAssignment: '<S286>/Bus Assignment3' incorporates:
     *  DataStoreRead: '<S286>/Data Store Read'
     *  DataStoreWrite: '<S289>/Data Store Write'
     *  Product: '<S286>/Product2'
     *  StateWriter: '<S286>/State Writer3'
     */
    PID_ID.Kp = Control.motorParams.motorLd * SystemParameters.BW_reg_Inv;

    /* Product: '<S286>/Product' incorporates:
     *  DataStoreRead: '<S286>/Data Store Read'
     *  DataStoreWrite: '<S289>/Data Store Write'
     *  Product: '<S286>/Product1'
     *  Product: '<S286>/Product3'
     */
    Merge_cr = (Control.motorParams.motorRs * SystemParameters.BW_reg_Inv) *
      Control.Tpwm;

    /* BusAssignment: '<S286>/Bus Assignment3' incorporates:
     *  Constant: '<S286>/Kc1'
     *  Product: '<S286>/Product'
     *  StateWriter: '<S286>/State Writer3'
     */
    PID_ID.Ki = Merge_cr;
    PID_ID.Kc = 0.9F;

    /* End of Outputs for SubSystem: '<Root>/Init' */

    /* BusAssignment: '<S47>/Bus Assignment' */
    (void)memset(&PID_IQ, 0, sizeof(PID_st));

    /* Outputs for Atomic SubSystem: '<Root>/Init' */
    /* BusAssignment: '<S286>/Bus Assignment4' incorporates:
     *  Constant: '<S286>/Kc2'
     *  DataStoreRead: '<S286>/Data Store Read'
     *  DataStoreWrite: '<S289>/Data Store Write'
     *  Product: '<S286>/Product4'
     *  StateWriter: '<S286>/State Writer4'
     */
    PID_IQ.Kp = Control.motorParams.motorLq * SystemParameters.BW_reg_Inv;
    PID_IQ.Ki = Merge_cr;
    PID_IQ.Kc = 0.9F;

    /* StateWriter: '<S286>/State Writer1' */
    (void)memset(&PID_Trq, 0, sizeof(PID_st));

    /* BusAssignment: '<S286>/Bus Assignment1' incorporates:
     *  Constant: '<S286>/Constant'
     *  Constant: '<S286>/Constant2'
     *  Constant: '<S286>/Constant3'
     *  DataTypeConversion: '<S286>/Data Type Conversion'
     *  DataTypeConversion: '<S286>/Data Type Conversion1'
     *  DataTypeConversion: '<S286>/Data Type Conversion2'
     *  StateWriter: '<S286>/State Writer1'
     */
    PID_Trq.Kp = 0.4F;
    PID_Trq.Ki = 0.2F;
    PID_Trq.Kc = 0.9F;

    /* End of Outputs for SubSystem: '<Root>/Init' */

    /* BusAssignment: '<S61>/Bus Assignment' */
    (void)memset(&PID_FW, 0, sizeof(PID_st));

    /* Outputs for Atomic SubSystem: '<Root>/Init' */
    /* BusAssignment: '<S286>/Bus Assignment2' incorporates:
     *  Constant: '<S286>/Kc'
     *  Constant: '<S286>/Ki'
     *  Constant: '<S286>/Kp'
     *  StateWriter: '<S286>/State Writer2'
     */
    PID_FW.Kp = 0.25F;
    PID_FW.Ki = 0.01F;
    PID_FW.Kc = 0.9F;

    /* StateWriter: '<S286>/State Writer5' */
    (void)memset(&PID_spd, 0, sizeof(PID_st));

    /* BusAssignment: '<S286>/Bus Assignment5' incorporates:
     *  Constant: '<S286>/Constant10'
     *  DataStoreRead: '<S286>/Data Store Read1'
     *  DataTypeConversion: '<S286>/Data Type Conversion5'
     *  StateWriter: '<S286>/State Writer5'
     */
    PID_spd.Kp = SystemParameters.SpeedKp;
    PID_spd.Ki = SystemParameters.SpeedKi;
    PID_spd.Kc = 0.9F;

    /* End of Outputs for SubSystem: '<Root>/Init' */

    /* BusAssignment: '<S120>/Bus Assignment' */
    (void)memset(&PID_IDC, 0, sizeof(PID_st));

    /* Outputs for Atomic SubSystem: '<Root>/Init' */
    /* BusAssignment: '<S286>/Bus Assignment6' incorporates:
     *  Constant: '<S286>/Constant11'
     *  Constant: '<S286>/Constant12'
     *  Constant: '<S286>/Constant7'
     *  DataTypeConversion: '<S286>/Data Type Conversion6'
     *  DataTypeConversion: '<S286>/Data Type Conversion7'
     *  DataTypeConversion: '<S286>/Data Type Conversion8'
     *  StateWriter: '<S286>/State Writer'
     */
    PID_IDC.Kp = 0.001F;
    PID_IDC.Ki = 2.0E-5F;
    PID_IDC.Kc = 0.99F;

    /* SwitchCase: '<S291>/Switch Case' incorporates:
     *  DataStoreRead: '<S287>/Data Store Read'
     */
    switch (SystemParameters.ActivePosSensor) {
     case ResolverType:
      /* Outputs for IfAction SubSystem: '<S291>/Switch Case Action Subsystem' incorporates:
       *  ActionPort: '<S292>/Action Port'
       */
      /* SignalConversion generated from: '<S292>/In1' incorporates:
       *  Constant: '<S291>/Constant'
       *  DataStoreRead: '<S291>/Data Store Read1'
       *  Product: '<S291>/Divide'
       */
      Merge_cr = 1.0F / SystemParameters.ResolverPoles;

      /* End of Outputs for SubSystem: '<S291>/Switch Case Action Subsystem' */
      break;

     case HallType:
      /* Outputs for IfAction SubSystem: '<S291>/Switch Case Action Subsystem1' incorporates:
       *  ActionPort: '<S293>/Action Port'
       */
      /* SignalConversion generated from: '<S293>/In1' incorporates:
       *  Constant: '<S291>/Constant'
       *  DataStoreRead: '<S291>/Data Store Read1'
       *  Product: '<S291>/Divide1'
       */
      Merge_cr = 1.0F / (real32_T)SystemParameters.MotorParams.motorPoles;

      /* End of Outputs for SubSystem: '<S291>/Switch Case Action Subsystem1' */
      break;

     case EncoderType:
      /* Outputs for IfAction SubSystem: '<S291>/Switch Case Action Subsystem3' incorporates:
       *  ActionPort: '<S295>/Action Port'
       */
      /* SignalConversion generated from: '<S295>/In1' incorporates:
       *  Constant: '<S291>/Constant'
       */
      Merge_cr = 1.0F;

      /* End of Outputs for SubSystem: '<S291>/Switch Case Action Subsystem3' */
      break;

     default:
      /* Outputs for IfAction SubSystem: '<S291>/Switch Case Action Subsystem2' incorporates:
       *  ActionPort: '<S294>/Action Port'
       */
      /* SignalConversion generated from: '<S294>/In1' incorporates:
       *  Constant: '<S291>/Constant'
       *  DataStoreRead: '<S291>/Data Store Read1'
       *  Product: '<S291>/Divide2'
       */
      Merge_cr = 1.0F / (real32_T)SystemParameters.MotorParams.motorPoles;

      /* End of Outputs for SubSystem: '<S291>/Switch Case Action Subsystem2' */
      break;
    }

    /* End of SwitchCase: '<S291>/Switch Case' */
    /* End of Outputs for SubSystem: '<Root>/Init' */

    /* BusAssignment: '<S161>/Bus Assignment' */
    (void)memset(&PositionObserver, 0, sizeof(Observer_st));

    /* Outputs for Atomic SubSystem: '<Root>/Init' */
    /* BusAssignment: '<S287>/Bus Assignment5' incorporates:
     *  Constant: '<S287>/L1_w'
     *  Constant: '<S287>/L2_w'
     *  DataStoreRead: '<S287>/Data Store Read'
     *  DataStoreRead: '<S287>/Data Store Read1'
     *  DataStoreWrite: '<S289>/Data Store Write'
     *  DataTypeConversion: '<S287>/Data Type Conversion'
     *  StateWriter: '<S287>/State Writer5'
     */
    PositionObserver.L0 = SystemParameters.Observer_LO;
    PositionObserver.L1_w = -0.04F;
    PositionObserver.L2_w = -13.0F;
    PositionObserver.Ts = Control.Tpwm;
    PositionObserver.SensorToMechanical = Merge_cr;
    PositionObserver.MechanicalToElectrical = (real32_T)
      SystemParameters.MotorParams.motorPoles;

    /* End of Outputs for SubSystem: '<Root>/Init' */
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
