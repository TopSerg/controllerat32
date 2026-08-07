/*
 * File: ControlSystem_v2.h
 *
 * Code generated for Simulink model 'ControlSystem_v2'.
 *
 * Model version                  : 5.217
 * Simulink Coder version         : 9.8 (R2022b) 13-May-2022
 * C/C++ source code generated on : Mon Apr 21 10:09:35 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. MISRA C:2012 guidelines
 *    2. Execution efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_ControlSystem_v2_h_
#define RTW_HEADER_ControlSystem_v2_h_
#ifndef ControlSystem_v2_COMMON_INCLUDES_
#define ControlSystem_v2_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ControlSystem_v2_COMMON_INCLUDES_ */

#include "ControlSystem_v2_types.h"
#include "TrqCalc.h"
#include "Logic.h"
#include "systemDefinations.h"
#include "testSignals.h"

/* Includes for objects with custom storage classes */
#include "indTable.h"

/* Exported data define */

/* Definition for custom storage class: Define */
#define TwoPI                          6.28318501F               /* Referenced by:
                                                                  * '<S191>/Constant'
                                                                  * '<S182>/Constant'
                                                                  * '<S188>/Constant'
                                                                  * '<S215>/Bias'
                                                                  * '<S215>/Bias1'
                                                                  * '<S215>/Switch'
                                                                  * '<S30>/Bias'
                                                                  * '<S30>/Bias1'
                                                                  * '<S30>/Constant'
                                                                  */

/* user code (top of header file) */
#include "platform_math.h"

/* Block signals and states (default storage) for system '<S3>/workModeChoise' */
typedef struct {
  DW_TrqCalc TrqCalc_m;                /* '<S320>/TrqCalc' */
  real32_T UnitDelay_DSTATE;           /* '<S366>/Unit Delay' */
} DW_workModeChoise;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  DW_workModeChoise workModeChoise_p;  /* '<S3>/workModeChoise' */
  DW_Logic sf_Logic;                   /* '<S3>/Logic' */
  real32_T Uz;                         /* '<S311>/Chart' */
  real32_T Ls;                         /* '<S311>/Chart' */
  real32_T Imax;                       /* '<S311>/Chart' */
  real32_T Imin;                       /* '<S311>/Chart' */
  real32_T Uz_e;                       /* '<S315>/Chart' */
  real32_T thetaRef;                   /* '<S307>/Chart' */
  real32_T ThetaElTest;                /* '<S313>/Chart' */
  real32_T thetaRef_i;                 /* '<S309>/Chart' */
  real32_T UnitDelay_DSTATE;           /* '<S209>/Unit Delay' */
  real32_T UnitDelay_DSTATE_e;         /* '<S105>/Unit Delay' */
  real32_T Isumm;                      /* '<S311>/Chart' */
  real32_T Usumm;                      /* '<S311>/Chart' */
  real32_T Isumm_h;                    /* '<S315>/Chart' */
  real32_T Usumm_p;                    /* '<S315>/Chart' */
  real32_T Isumm2;                     /* '<S315>/Chart' */
  real32_T Usumm2;                     /* '<S315>/Chart' */
  real32_T WelRated;                   /* '<S313>/Chart' */
  real32_T tSample;                    /* '<S313>/Chart' */
  uint16_T treshold;                   /* '<S311>/Chart' */
  uint16_T delayCnt;                   /* '<S311>/Chart' */
  uint16_T delayCnt_b;                 /* '<S315>/Chart' */
  uint16_T treshold_g;                 /* '<S307>/Chart' */
  uint16_T delayCnt_p;                 /* '<S307>/Chart' */
  uint16_T stepsCnt;                   /* '<S307>/Chart' */
  uint16_T delayCnt_o;                 /* '<S313>/Chart' */
  uint16_T treshold_f;                 /* '<S309>/Chart' */
  uint16_T delayCnt_g;                 /* '<S309>/Chart' */
  uint16_T stepsCnt_e;                 /* '<S309>/Chart' */
  uint8_T is_c5_ControlSystem_v2;      /* '<S311>/Chart' */
  uint8_T is_active_c5_ControlSystem_v2;/* '<S311>/Chart' */
  uint8_T is_c1_ControlSystem_v2;      /* '<S315>/Chart' */
  uint8_T is_active_c1_ControlSystem_v2;/* '<S315>/Chart' */
  uint8_T is_c6_ControlSystem_v2;      /* '<S307>/Chart' */
  uint8_T is_active_c6_ControlSystem_v2;/* '<S307>/Chart' */
  uint8_T is_c4_ControlSystem_v2;      /* '<S313>/Chart' */
  uint8_T is_active_c4_ControlSystem_v2;/* '<S313>/Chart' */
  uint8_T is_c2_ControlSystem_v2;      /* '<S309>/Chart' */
  uint8_T is_active_c2_ControlSystem_v2;/* '<S309>/Chart' */
  boolean_T inProgress;                /* '<S311>/Chart' */
  boolean_T testDone;                  /* '<S311>/Chart' */
  boolean_T inProgress_g;              /* '<S315>/Chart' */
  boolean_T testDone_o;                /* '<S315>/Chart' */
  boolean_T inProgress_n;              /* '<S307>/Chart' */
  boolean_T inProgress_c;              /* '<S313>/Chart' */
  boolean_T testDone_d;                /* '<S313>/Chart' */
  boolean_T inProgress_a;              /* '<S309>/Chart' */
  boolean_T testDone_j;                /* '<S309>/Chart' */
} DW;

/* Constant parameters (default storage) */
typedef struct {
  /* Pooled Parameter (Expression: SystemParams_st_MATLABStruct)
   * Referenced by:
   *   '<Root>/Data Store Memory1'
   *   '<S288>/Constant6'
   */
  SystemParams_st pooled2;

  /* Expression: TestSignals_st_MATLABStruct
   * Referenced by: '<Root>/Data Store Memory'
   */
  TestSignals_st DataStoreMemory_InitialValue;

  /* Pooled Parameter (Expression: TripLevels_st_MATLABStruct)
   * Referenced by:
   *   '<Root>/Data Store Memory2'
   *   '<S289>/Constant1'
   */
  TripLevels_st pooled7;

  /* Pooled Parameter (Expression: )
   * Referenced by:
   *   '<S381>/Ld_lookUp'
   *   '<S381>/Lq_lookUp'
   */
  uint32_T pooled31[2];
} ConstP;

/* Block signals and states (default storage) */
extern DW DW_l;

/* Constant parameters (default storage) */
extern const ConstP ConstP_d;

/*
 * Exported Global Signals
 *
 * Note: Exported global signals are block signals with an exported global
 * storage class designation.  Code generation will declare the memory for
 * these signals and export their symbols.
 *
 */
extern inSignals_st inSignals;         /* '<Root>/inSignals' */
extern ExtRef_st extRef;               /* '<Root>/extRef' */
extern InternalRef_st internalRef;     /* '<S3>/Bus Creator' */

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern real32_T VdPlay;                /* Variable: VdPlay
                                        * Referenced by: '<S16>/Constant'
                                        */
extern real32_T VqPlay;                /* Variable: VqPlay
                                        * Referenced by: '<S16>/Constant1'
                                        */

/*
 * Exported States
 *
 * Note: Exported states are block states with an exported global
 * storage class designation.  Code generation will declare the memory for these
 * states and exports their symbols.
 *
 */
extern Control_st Control;             /* '<Root>/Data Store Memory3' */
extern SystemParams_st SystemParameters;/* '<Root>/Data Store Memory1' */
extern PID_st PID_IDC;                 /* '<S111>/Pid_Idc_Init'
                                        * Структура для регулятора тока Idc ( ограничение заряда разряда)
                                        */
extern PID_st PID_FW;                  /* '<S23>/Pid_FW_Init' */
extern PID_st PID_ID;                  /* '<S32>/Pid_Id_Init' */
extern PID_st PID_IQ;                  /* '<S33>/Pid_Iq_Init' */
extern Observer_st PositionObserver;   /* '<S10>/Observer_Init' */
extern TestedParams_st TestedParams;   /* '<Root>/Data Store Memory4'
                                        * Стуктура с парметрами полученными в результате тестирования системы
                                        */
extern TestSignals_st TestRefSignals;  /* '<Root>/Data Store Memory' */
extern PWM_st SVPWM;                   /* Simulink.Signal object 'SVPWM' */
extern LPF_st lpfUdc;                  /* Simulink.Signal object 'lpfUdc' */
extern LPF_st lpfFluxErrorAngle;   /* Simulink.Signal object 'lpfFluxErrorAngle'
                                    * Фильтр для расчета угла ошибки по потоку
                                    */
extern TripLevels_st TripLevels;       /* '<Root>/Data Store Memory2' */
extern real32_T debugAngle;            /* '<S29>/Unit Delay' */
extern PID_st PID_spd;                 /* '<S322>/Pid_SPD_Init'
                                        * Регултор скорости
                                        */
extern PID_st PID_Trq;                 /* '<S342>/Pid_TRQ_Init'
                                        * Структура PID регулятора для момента
                                        */

/* Model entry point functions */
extern void ControlSystem_v2_initialize(void);

/* Exported entry point function */
extern void isrADC(void);

/* Exported entry point function */
extern void isrTIM(void);

/* Exported data declaration */

/* Volatile memory section */
/* Declaration for custom storage class: Volatile */
extern volatile real32_T ResSignalAmpl;/* '<S194>/Add' */

/* Амплитуда сигнала резольвера */

/* Data with Exported storage */
extern real32_T DirectRef;             /* '<Root>/DirectRef' */
extern real32_T Out1;                  /* '<Root>/Out1' */
extern outSinals_st outSignals;        /* '<Root>/outSignals' */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'ControlSystem_v2'
 * '<S1>'   : 'ControlSystem_v2/ADCisr'
 * '<S2>'   : 'ControlSystem_v2/Init'
 * '<S3>'   : 'ControlSystem_v2/TIMisr'
 * '<S4>'   : 'ControlSystem_v2/ADCisr/ControlCalc'
 * '<S5>'   : 'ControlSystem_v2/ADCisr/PWMcalc'
 * '<S6>'   : 'ControlSystem_v2/ADCisr/PhaseInverse'
 * '<S7>'   : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control'
 * '<S8>'   : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect'
 * '<S9>'   : 'ControlSystem_v2/ADCisr/ControlCalc/PositionDataUpdate'
 * '<S10>'  : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator'
 * '<S11>'  : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc'
 * '<S12>'  : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess'
 * '<S13>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentCalculations'
 * '<S14>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators'
 * '<S15>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageControl'
 * '<S16>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter'
 * '<S17>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentCalculations/Clark '
 * '<S18>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentCalculations/Park'
 * '<S19>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentCalculations/Clark /Clark'
 * '<S20>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentCalculations/Clark /CurrentAmpCalc'
 * '<S21>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentCalculations/Park/Subsystem'
 * '<S22>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl'
 * '<S23>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW'
 * '<S24>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits '
 * '<S25>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem'
 * '<S26>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1'
 * '<S27>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem/VoltageControl'
 * '<S28>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem/VoltageControl/HandModeVoltage'
 * '<S29>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem/VoltageControl/HandModeVoltage/Roll2PiWithReset'
 * '<S30>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem/VoltageControl/HandModeVoltage/Roll2PiWithReset/roll_toPI1'
 * '<S31>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl'
 * '<S32>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id'
 * '<S33>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq'
 * '<S34>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/decupling'
 * '<S35>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id'
 * '<S36>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/AntiWindUp'
 * '<S37>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/Error Calculation'
 * '<S38>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/PIcalc'
 * '<S39>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/AntiWindUp/If Action Subsystem'
 * '<S40>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/AntiWindUp/If Action Subsystem1'
 * '<S41>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/AntiWindUp/If Action Subsystem2'
 * '<S42>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/PIcalc/Calc Int'
 * '<S43>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/PIcalc/Calc Prop'
 * '<S44>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/PIcalc/Calc Int/IntegralReset'
 * '<S45>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/PIcalc/Calc Int/IntegralReset/Ui=0'
 * '<S46>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Id/PID_Id/PIcalc/Calc Int/IntegralReset/Ui=1'
 * '<S47>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq'
 * '<S48>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/AntiWindUp'
 * '<S49>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/Error Calculation'
 * '<S50>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/PIcalc'
 * '<S51>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/AntiWindUp/If Action Subsystem'
 * '<S52>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/AntiWindUp/If Action Subsystem1'
 * '<S53>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/AntiWindUp/If Action Subsystem2'
 * '<S54>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/PIcalc/Calc Int'
 * '<S55>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/PIcalc/Calc Prop'
 * '<S56>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/PIcalc/Calc Int/IntegralReset'
 * '<S57>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/PIcalc/Calc Int/IntegralReset/Ui=0'
 * '<S58>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/Pid_Iq/PID_Iq/PIcalc/Calc Int/IntegralReset/Ui=1'
 * '<S59>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/decupling/DirectMothod'
 * '<S60>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/CurrentControl/If Action Subsystem1/CurrentControl/decupling/Saturation Dynamic'
 * '<S61>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw'
 * '<S62>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/AntiWindUp'
 * '<S63>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/Error Calculation'
 * '<S64>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/PIcalc'
 * '<S65>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/AntiWindUp/If Action Subsystem'
 * '<S66>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/AntiWindUp/If Action Subsystem1'
 * '<S67>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/AntiWindUp/If Action Subsystem2'
 * '<S68>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/PIcalc/Calc Int'
 * '<S69>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/PIcalc/Calc Prop'
 * '<S70>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/PIcalc/Calc Int/IntegralReset'
 * '<S71>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/PIcalc/Calc Int/IntegralReset/Ui=0'
 * '<S72>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/Pid_FW/PID_fw/PIcalc/Calc Int/IntegralReset/Ui=1'
 * '<S73>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits'
 * '<S74>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /Compare To Constant'
 * '<S75>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits'
 * '<S76>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /dynamicVariation'
 * '<S77>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/Current Circle Limit'
 * '<S78>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer'
 * '<S79>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/IIR Filter'
 * '<S80>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/MaxIqCalc'
 * '<S81>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/Saturation'
 * '<S82>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/satCoef'
 * '<S83>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/Current Circle Limit/Saturation Dynamic'
 * '<S84>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/Current Circle Limit/Ограничение Iq'
 * '<S85>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/Current Circle Limit/Ограничение Iq/CalcMaxIqAmp'
 * '<S86>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit'
 * '<S87>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter'
 * '<S88>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D-Q Equivalence'
 * '<S89>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority'
 * '<S90>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/Inport//Dialog Selection'
 * '<S91>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/Magnitude_calc'
 * '<S92>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D-Q Equivalence/Limiter'
 * '<S93>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D-Q Equivalence/Passthrough'
 * '<S94>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/Compare To Constant'
 * '<S95>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/Compare To Constant1'
 * '<S96>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/flipInputs'
 * '<S97>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/flipInputs1'
 * '<S98>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/limiter'
 * '<S99>'  : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/limiter/limitRef1'
 * '<S100>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/limiter/limitRef2'
 * '<S101>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/limiter/passThrough'
 * '<S102>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/CurrentFormer/RefVectorLimit/DQ Limiter/D//Q Axis Priority/limiter/limitRef2/Compare To Zero'
 * '<S103>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/IIR Filter/IIR Filter'
 * '<S104>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/IIR Filter/IIR Filter/Low-pass'
 * '<S105>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/IIR Filter/IIR Filter/Low-pass/IIR Low Pass Filter'
 * '<S106>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/MaxIqCalc/If Action Subsystem2'
 * '<S107>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/MaxIqCalc/If Action Subsystem2/Saturation Dynamic'
 * '<S108>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /2ndLimits/Saturation/Saturation Dynamic'
 * '<S109>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits'
 * '<S110>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/DynamicVoltageSaturation'
 * '<S111>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter '
 * '<S112>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IgbtTemprLimit'
 * '<S113>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/MotorTemprLimit'
 * '<S114>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/SpeedLimit'
 * '<S115>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/makeStateWord'
 * '<S116>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/DynamicVoltageSaturation/HighVoltLimit'
 * '<S117>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/DynamicVoltageSaturation/If Action Subsystem'
 * '<S118>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/DynamicVoltageSaturation/If Action Subsystem1'
 * '<S119>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/DynamicVoltageSaturation/LowVoltLimit'
 * '<S120>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc'
 * '<S121>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/AntiWindUp'
 * '<S122>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/Error Calculation'
 * '<S123>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/PIcalc'
 * '<S124>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/AntiWindUp/If Action Subsystem'
 * '<S125>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/AntiWindUp/If Action Subsystem1'
 * '<S126>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/AntiWindUp/If Action Subsystem2'
 * '<S127>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/PIcalc/Calc Int'
 * '<S128>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/PIcalc/Calc Prop'
 * '<S129>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/PIcalc/Calc Int/IntegralReset'
 * '<S130>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/PIcalc/Calc Int/IntegralReset/Ui=0'
 * '<S131>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IdcLimiter /PID_Idc/PIcalc/Calc Int/IntegralReset/Ui=1'
 * '<S132>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IgbtTemprLimit/HighBound'
 * '<S133>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/IgbtTemprLimit/LowBound'
 * '<S134>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/MotorTemprLimit/HighBound'
 * '<S135>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/MotorTemprLimit/LowBound'
 * '<S136>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/SpeedLimit/Backward'
 * '<S137>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/SpeedLimit/Compare To Constant'
 * '<S138>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/CurrentRegulators/SaturationLimits /RefCurrentsLimits/FuncLimits/SpeedLimit/Forward'
 * '<S139>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageControl/InversPark'
 * '<S140>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageControl/roll_toPI1'
 * '<S141>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/AmplCalc'
 * '<S142>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/AmplCalc1'
 * '<S143>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения'
 * '<S144>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Макс напряжение  оси Q'
 * '<S145>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D'
 * '<S146>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D2'
 * '<S147>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D/If Action Subsystem'
 * '<S148>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D/If Action Subsystem1'
 * '<S149>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D/If Action Subsystem2'
 * '<S150>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D2/If Action Subsystem'
 * '<S151>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D2/If Action Subsystem1'
 * '<S152>' : 'ControlSystem_v2/ADCisr/ControlCalc/FOC_control/VoltageLimiter/Ограничение компонент напряжения/Ограничение по  оси D2/If Action Subsystem2'
 * '<S153>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections'
 * '<S154>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections'
 * '<S155>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections/If Action Subsystem'
 * '<S156>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections/If Action Subsystem1'
 * '<S157>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections/Защита по напряжению'
 * '<S158>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections/Защита по температуре мотора'
 * '<S159>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections/Защита по температуре силовой части'
 * '<S160>' : 'ControlSystem_v2/ADCisr/ControlCalc/Fast_Protect/Protections/Protections/Защиты по мгновенному току'
 * '<S161>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer'
 * '<S162>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/ConvertToAngle1'
 * '<S163>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/ConvertToAngle2'
 * '<S164>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/ConvertToAngle3'
 * '<S165>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/Oberver_calc'
 * '<S166>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/Subsystem'
 * '<S167>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/Вычисление коэффциентов'
 * '<S168>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/ConvertToAngle1/roll_toPI1'
 * '<S169>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/ConvertToAngle2/roll_toPI1'
 * '<S170>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/ConvertToAngle3/roll_toPI1'
 * '<S171>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/Subsystem/roll_PI'
 * '<S172>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/Subsystem/roll_PI1'
 * '<S173>' : 'ControlSystem_v2/ADCisr/ControlCalc/Position_Estimator/Position_observer/Вычисление коэффциентов/activeL0'
 * '<S174>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/DebugAngle'
 * '<S175>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/EncoderSensor'
 * '<S176>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor'
 * '<S177>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor'
 * '<S178>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/EncoderSensor/RAW_to_CNT'
 * '<S179>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/EncoderSensor/RAW_to_CNT/If Action Subsystem1'
 * '<S180>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/EncoderSensor/RAW_to_CNT/If Action Subsystem2'
 * '<S181>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/EncoderSensor/RAW_to_CNT/If Action Subsystem1/CW_TABLE'
 * '<S182>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/EncoderSensor/RAW_to_CNT/If Action Subsystem2/CW_TABLE'
 * '<S183>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT'
 * '<S184>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT/If Action Subsystem1'
 * '<S185>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT/If Action Subsystem2'
 * '<S186>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT/If Action Subsystem1/CW_TABLE'
 * '<S187>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT/If Action Subsystem1/CW_TABLE/getHallIndex'
 * '<S188>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT/If Action Subsystem2/CW_TABLE'
 * '<S189>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/HallSensor/RAW_to_CNT/If Action Subsystem2/CW_TABLE/getHallIndex'
 * '<S190>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess'
 * '<S191>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/If Action Subsystem'
 * '<S192>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/If Action Subsystem1'
 * '<S193>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/LineSwap'
 * '<S194>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/maxAmpsearch'
 * '<S195>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/roll_toPI1'
 * '<S196>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/LineSwap/If Action Subsystem'
 * '<S197>' : 'ControlSystem_v2/ADCisr/ControlCalc/RawAngleCalc/ResolverSensor/InputsProcess/LineSwap/If Action Subsystem1'
 * '<S198>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/cmdONlogic'
 * '<S199>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess'
 * '<S200>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/sensorDataProcess'
 * '<S201>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation'
 * '<S202>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/cmdONlogic/CmdLockOverSpeed'
 * '<S203>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/cmdONlogic/CmdLockOverSpeed/Compare To Constant'
 * '<S204>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/cmdONlogic/CmdLockOverSpeed/Compare To Constant1'
 * '<S205>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/IIR Filter2'
 * '<S206>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications'
 * '<S207>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/IIR Filter2/IIR Filter'
 * '<S208>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/IIR Filter2/IIR Filter/Low-pass'
 * '<S209>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/IIR Filter2/IIR Filter/Low-pass/IIR Low Pass Filter'
 * '<S210>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem'
 * '<S211>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem1'
 * '<S212>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl'
 * '<S213>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents'
 * '<S214>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers'
 * '<S215>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/roll_toPI1'
 * '<S216>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers/If Action Subsystem'
 * '<S217>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers/If Action Subsystem1'
 * '<S218>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers/If Action Subsystem/ThetaCorrection'
 * '<S219>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers/If Action Subsystem/ThetaCorrection/fluxAngleCorrection'
 * '<S220>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers/If Action Subsystem1/ThetaCorrection'
 * '<S221>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/AnglePassControl/PhaseInvers/If Action Subsystem1/ThetaCorrection/fluxAngleCorrection'
 * '<S222>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq'
 * '<S223>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc'
 * '<S224>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/IPSM'
 * '<S225>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/IPSM1'
 * '<S226>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/Switch Case Action Subsystem'
 * '<S227>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/Switch Case Action Subsystem1'
 * '<S228>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/Switch Case Action Subsystem2'
 * '<S229>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/IPSM/CalcId'
 * '<S230>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/refCommndsProcess/Identifications/Switch Case Action Subsystem/CalcRefCurrents/TorqueToIdIq/refCurrentCalc/IPSM/CalcIq'
 * '<S231>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/sensorDataProcess/GetMaxTemperatureValues'
 * '<S232>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/sensorDataProcess/PhaseInverse'
 * '<S233>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/sensorDataProcess/PhaseInverse/If Action Subsystem'
 * '<S234>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/sensorDataProcess/PhaseInverse/If Action Subsystem1'
 * '<S235>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation/DriveModeDetect'
 * '<S236>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation/IdcCalc'
 * '<S237>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation/LPF_Udc'
 * '<S238>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation/LPF_Udc1'
 * '<S239>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation/DriveModeDetect/If Action Subsystem'
 * '<S240>' : 'ControlSystem_v2/ADCisr/ControlCalc/inDataProcess/signalfilteringAndCalulation/DriveModeDetect/If Action Subsystem1'
 * '<S241>' : 'ControlSystem_v2/ADCisr/PWMcalc/Ref Voltage Normalization1'
 * '<S242>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM'
 * '<S243>' : 'ControlSystem_v2/ADCisr/PWMcalc/Конвертация в CMP'
 * '<S244>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-1'
 * '<S245>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-2'
 * '<S246>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-3'
 * '<S247>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Sector_calc'
 * '<S248>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc'
 * '<S249>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-1/If Action Subsystem'
 * '<S250>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-1/If Action Subsystem1'
 * '<S251>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-1/If Action Subsystem2'
 * '<S252>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-2/If Action Subsystem'
 * '<S253>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-2/If Action Subsystem1'
 * '<S254>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-2/If Action Subsystem2'
 * '<S255>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-3/If Action Subsystem'
 * '<S256>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-3/If Action Subsystem1'
 * '<S257>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Limit0-3/If Action Subsystem2'
 * '<S258>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE1'
 * '<S259>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE2'
 * '<S260>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE3'
 * '<S261>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE4'
 * '<S262>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE5'
 * '<S263>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE6'
 * '<S264>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE7'
 * '<S265>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE1/Ta'
 * '<S266>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE1/Tb'
 * '<S267>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE1/Tc'
 * '<S268>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE2/Ta'
 * '<S269>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE2/Tb'
 * '<S270>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE2/Tc'
 * '<S271>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE3/Ta'
 * '<S272>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE3/Tb'
 * '<S273>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE3/Tb1'
 * '<S274>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE4/Ta'
 * '<S275>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE4/Tb'
 * '<S276>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE4/Tc'
 * '<S277>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE5/Ta'
 * '<S278>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE5/Tb'
 * '<S279>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE5/Tc'
 * '<S280>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE6/T1'
 * '<S281>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE6/Ta'
 * '<S282>' : 'ControlSystem_v2/ADCisr/PWMcalc/SVPWM/Tabc/CASE6/Tc'
 * '<S283>' : 'ControlSystem_v2/ADCisr/PhaseInverse/If Action Subsystem'
 * '<S284>' : 'ControlSystem_v2/ADCisr/PhaseInverse/If Action Subsystem1'
 * '<S285>' : 'ControlSystem_v2/Init/PWM_init'
 * '<S286>' : 'ControlSystem_v2/Init/PidInit'
 * '<S287>' : 'ControlSystem_v2/Init/PosObserverInit'
 * '<S288>' : 'ControlSystem_v2/Init/SystemParamInit'
 * '<S289>' : 'ControlSystem_v2/Init/controlInit'
 * '<S290>' : 'ControlSystem_v2/Init/filterInit'
 * '<S291>' : 'ControlSystem_v2/Init/PosObserverInit/SensorTypeInit'
 * '<S292>' : 'ControlSystem_v2/Init/PosObserverInit/SensorTypeInit/Switch Case Action Subsystem'
 * '<S293>' : 'ControlSystem_v2/Init/PosObserverInit/SensorTypeInit/Switch Case Action Subsystem1'
 * '<S294>' : 'ControlSystem_v2/Init/PosObserverInit/SensorTypeInit/Switch Case Action Subsystem2'
 * '<S295>' : 'ControlSystem_v2/Init/PosObserverInit/SensorTypeInit/Switch Case Action Subsystem3'
 * '<S296>' : 'ControlSystem_v2/Init/SystemParamInit/If Action Subsystem'
 * '<S297>' : 'ControlSystem_v2/Init/SystemParamInit/If Action Subsystem1'
 * '<S298>' : 'ControlSystem_v2/TIMisr/Identification'
 * '<S299>' : 'ControlSystem_v2/TIMisr/Logic'
 * '<S300>' : 'ControlSystem_v2/TIMisr/workModeChoise'
 * '<S301>' : 'ControlSystem_v2/TIMisr/Identification/ENCODER_Ident'
 * '<S302>' : 'ControlSystem_v2/TIMisr/Identification/HALL_Ident'
 * '<S303>' : 'ControlSystem_v2/TIMisr/Identification/LS_Ident'
 * '<S304>' : 'ControlSystem_v2/TIMisr/Identification/RESOLVER_Ident'
 * '<S305>' : 'ControlSystem_v2/TIMisr/Identification/RS_Ident'
 * '<S306>' : 'ControlSystem_v2/TIMisr/Identification/Switch Case Action Subsystem3'
 * '<S307>' : 'ControlSystem_v2/TIMisr/Identification/ENCODER_Ident/HallTableIdentification'
 * '<S308>' : 'ControlSystem_v2/TIMisr/Identification/ENCODER_Ident/HallTableIdentification/Chart'
 * '<S309>' : 'ControlSystem_v2/TIMisr/Identification/HALL_Ident/HallTableIdentification'
 * '<S310>' : 'ControlSystem_v2/TIMisr/Identification/HALL_Ident/HallTableIdentification/Chart'
 * '<S311>' : 'ControlSystem_v2/TIMisr/Identification/LS_Ident/LsIdentification'
 * '<S312>' : 'ControlSystem_v2/TIMisr/Identification/LS_Ident/LsIdentification/Chart'
 * '<S313>' : 'ControlSystem_v2/TIMisr/Identification/RESOLVER_Ident/ResolverIdentification'
 * '<S314>' : 'ControlSystem_v2/TIMisr/Identification/RESOLVER_Ident/ResolverIdentification/Chart'
 * '<S315>' : 'ControlSystem_v2/TIMisr/Identification/RS_Ident/RsIdentification'
 * '<S316>' : 'ControlSystem_v2/TIMisr/Identification/RS_Ident/RsIdentification/Chart'
 * '<S317>' : 'ControlSystem_v2/TIMisr/workModeChoise/CurrentRefGenerator'
 * '<S318>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator'
 * '<S319>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator'
 * '<S320>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations'
 * '<S321>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg'
 * '<S322>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd'
 * '<S323>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/TrqToIm'
 * '<S324>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/refRateLimiter'
 * '<S325>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd'
 * '<S326>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/Saturation Dynamic'
 * '<S327>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/Saturation Dynamic1'
 * '<S328>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/AntiWindUp'
 * '<S329>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/Error Calculation'
 * '<S330>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/PIcalc'
 * '<S331>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/AntiWindUp/If Action Subsystem'
 * '<S332>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/AntiWindUp/If Action Subsystem1'
 * '<S333>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/AntiWindUp/If Action Subsystem2'
 * '<S334>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/PIcalc/Calc Int'
 * '<S335>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/PIcalc/Calc Prop'
 * '<S336>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/PIcalc/Calc Int/IntegralReset'
 * '<S337>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/PIcalc/Calc Int/IntegralReset/Ui=0'
 * '<S338>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/PID_Spd/PID_spd/PIcalc/Calc Int/IntegralReset/Ui=1'
 * '<S339>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/refRateLimiter/RateLimiters'
 * '<S340>' : 'ControlSystem_v2/TIMisr/workModeChoise/SpeedRegulator/SpeedReg/refRateLimiter/RateLimiters/RateLimiter'
 * '<S341>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg'
 * '<S342>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq'
 * '<S343>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/RateLimiters'
 * '<S344>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/TrqLim'
 * '<S345>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq'
 * '<S346>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/AntiWindUp'
 * '<S347>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/Error Calculation'
 * '<S348>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/PIcalc'
 * '<S349>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/AntiWindUp/If Action Subsystem'
 * '<S350>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/AntiWindUp/If Action Subsystem1'
 * '<S351>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/AntiWindUp/If Action Subsystem2'
 * '<S352>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/PIcalc/Calc Int'
 * '<S353>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/PIcalc/Calc Prop'
 * '<S354>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/PIcalc/Calc Int/IntegralReset'
 * '<S355>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/PIcalc/Calc Int/IntegralReset/Ui=0'
 * '<S356>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/PID_Trq/PID_trq/PIcalc/Calc Int/IntegralReset/Ui=1'
 * '<S357>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/RateLimiters/RateLimiter'
 * '<S358>' : 'ControlSystem_v2/TIMisr/workModeChoise/TrqRegulator/TorqueReg/TrqLim/Saturation Dynamic'
 * '<S359>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/DC_Filt'
 * '<S360>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/GetMotorType'
 * '<S361>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc'
 * '<S362>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc'
 * '<S363>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/DC_Filt/IdcFilter'
 * '<S364>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/DC_Filt/IdcFilter/IIR Filter'
 * '<S365>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/DC_Filt/IdcFilter/IIR Filter/Low-pass'
 * '<S366>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/DC_Filt/IdcFilter/IIR Filter/Low-pass/IIR Low Pass Filter'
 * '<S367>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/GetMotorType/Compare To Constant'
 * '<S368>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/GetMotorType/If Action Subsystem'
 * '<S369>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/GetMotorType/If Action Subsystem1'
 * '<S370>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc/If Action Subsystem'
 * '<S371>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc/If Action Subsystem1'
 * '<S372>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc/If Action Subsystem2'
 * '<S373>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc/If Action Subsystem3'
 * '<S374>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc/LPF_FluxErr'
 * '<S375>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/ShiftAngleCalc/LPF_FluxErr1'
 * '<S376>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/GetInductions2'
 * '<S377>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/GetMotorType'
 * '<S378>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter1'
 * '<S379>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter2'
 * '<S380>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/WindingRcorrection'
 * '<S381>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/GetInductions2/LdLq_Tables'
 * '<S382>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/GetInductions2/Switch Case Action Subsystem'
 * '<S383>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/GetInductions2/Switch Case Action Subsystem1'
 * '<S384>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter1/IIR Filter'
 * '<S385>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter1/IIR Filter/Low-pass'
 * '<S386>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter1/IIR Filter/Low-pass/IIR Low Pass Filter'
 * '<S387>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter2/IIR Filter'
 * '<S388>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter2/IIR Filter/Low-pass'
 * '<S389>' : 'ControlSystem_v2/TIMisr/workModeChoise/calculations/TrqCalc/IIR Filter2/IIR Filter/Low-pass/IIR Low Pass Filter'
 */

/*-
 * Requirements for '<Root>': ControlSystem_v2


 */
#endif                                 /* RTW_HEADER_ControlSystem_v2_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
