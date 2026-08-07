/*
 * File: systemDefinations.h
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

#ifndef RTW_HEADER_systemDefinations_h_
#define RTW_HEADER_systemDefinations_h_
#include "rtwtypes.h"

/* Типа моторов
   ACIM- асинронный мотор
   IPSM - синхронный мотор с магнитави В роторе
   SMPSM - синхронный мотор с магнитами НА роторе */
typedef enum {
  ACIM = 0,                            /* Default value */
  IPSM,
  SMPSM
} motorTypeType;

typedef struct {
  /* Уставка по высокому напряжению питания ( используется в генераторном режиме) */
  int16_T Udc_high;

  /* Диспазон работы ограничения от 1-0 в вольтах */
  int16_T Udc_high_diap;
  int16_T Udc_low;
  int16_T Udc_low_diap;
  int16_T Igbt_Thigh;
  int16_T Igbt_Thigh_diap;
  int16_T Motor_Thigh;
  int16_T Motor_Thigh_diap;
  int16_T Speed_fwd;
  int16_T Speed_fwd_diap;

  /* Начало ограничения скорости назад */
  uint16_T Speed_rev;

  /* Диспазон ограничения скорости назад */
  uint16_T Speed_rev_diap;
} FuncLimits_st;

/* Motor torque constant NM per Amper */
typedef struct {
  motorTypeType motorType;

  /* Омическое сопротивление фазы мотора */
  real32_T motorRs;

  /* Индуктивность по оси D базовое значение */
  real32_T motorLd;

  /* Индуктивность по оси Q */
  real32_T motorLq;

  /* Коэффциент  полюсности */
  real32_T motorSilence;

  /* Эдс мотора */
  real32_T motorEmf;
  real32_T motorKt;

  /* Количество пар полюсов */
  uint16_T motorPoles;

  /* максимальная рабочая температура мотора */
  int16_T motorTmax;

  /* Минимальная рабочая ( отрицательная ) температура мотора */
  int16_T motorTmin;

  /* Максимальная скороть мотора в радианах в секунду */
  uint16_T motorWmax;
} Motor_st;

/* Поддерживаемые типы датчиков положения */
typedef enum {
  NoSensor = 0,                        /* Default value */
  ResolverType,
  HallType,
  EncoderType,
  SensorLessType,
  HandModeSensor
} posSensorType;

typedef struct {
  uint32_T version;

  /* Источник управления приводом
     0-Аналоговый
     1-Цифровой */
  uint16_T controlSource;
  FuncLimits_st FuncLimits;
  Motor_st MotorParams;
  posSensorType ActivePosSensor;

  /* resolution of positon sensor in RAD to 1 mechanical revolution */
  real32_T PosSensorResolution;
  real32_T BW_reg_Inv;
  real32_T FieldWeakingCoef;
  real32_T LagCorrection;
  real32_T MaxCurrent;

  /* Максимальный ток Id(положительный) */
  real32_T MaxIdCurrent;

  /* Минимальный ток Id ( отрицательный) */
  real32_T MinIdCurrent;

  /* Максимальный ток заряда батареи (при торможении) */
  real32_T MaxCharge;

  /* Максимальный ток разряда батареи */
  real32_T MaxDischarge;

  /* Коэффициент пропорциональности регулятора момента */
  real32_T TrqKp;

  /* Коээфициент интгерирования регулятора момента */
  real32_T TrqKi;

  /* Коэффициент демпфирования колебаний момента */
  real32_T TrqDampingFactor;

  /* Бит разрешения демпфирования момента */
  uint16_T TrqDampingEnbale;

  /* Коэффициент усиления регулятора скорости */
  real32_T SpeedKp;

  /* Коэффициент интегрирования регулятора скорости */
  real32_T SpeedKi;
  real32_T Observer_LO;
  real32_T PwmBaseFrq;
  real32_T Rate_Down;
  real32_T Rate_Up;
  real32_T ResolverPoles;

  /* Бит используется для переключения местами SIn COs сигналов резольвера */
  uint16_T ResolverSignalSwap;
  real32_T UdcFiltTp;
  real32_T correctionTheta;
  real32_T deadTimeVoltLoss;
  uint16_T hallTableDirect[6];
  uint16_T hallTableInverse[6];
  uint16_T phase_inverse;
  uint16_T StaticInductionFlg;

  /* /_ #UINT16#decoupling mode control bit_/ */
  uint16_T DecouplingEnable;
} SystemParams_st;

typedef enum {
  DEFAULT = 0,                         /* Default value */
  CURRENT = 2,
  RS = 3,
  LS = 4,
  HALL_ID = 5,
  RESOLVER_ID = 6,
  ENCODER_ID = 7
} WorkModeType;

/* 0x0: Ready
   0x1: TrqCtrl
   0x2: Active Hearting
   0x3: Discharge
   0x4: SpeedCtrl */
typedef enum {
  READY = 0,                           /* Default value */
  TrqCtrl,
  ActHeating,
  Discharge,
  SpdCtrl
} regModeType;

#endif                                 /* RTW_HEADER_systemDefinations_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
