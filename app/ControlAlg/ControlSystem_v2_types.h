/*
 * File: ControlSystem_v2_types.h
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

#ifndef RTW_HEADER_ControlSystem_v2_types_h_
#define RTW_HEADER_ControlSystem_v2_types_h_
#include "rtwtypes.h"
#include "testSignals.h"
#include "systemDefinations.h"
#include "indTable.h"
#ifndef DEFINED_TYPEDEF_FOR_outSinals_st_
#define DEFINED_TYPEDEF_FOR_outSinals_st_

/* Глобальный сигнал ошибки
 */
typedef struct {
  /* Сигнал сброса ошибки для внешних цепей */
  boolean_T clrFault;
  boolean_T globalFault;

  /* Enable signal for PWM module */
  boolean_T enable;

  /* Normalized valued of PWM reference */
  real32_T refPwmA;

  /* Normalized valued of PWM reference */
  real32_T refPwmB;

  /* Normalized valued of PWM reference */
  real32_T refPwmC;
} outSinals_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_FuncLimitFlag_st_
#define DEFINED_TYPEDEF_FOR_FuncLimitFlag_st_

typedef struct {
  /* флаг активности лимита по напряжению питания */
  boolean_T udcLimitFlg;

  /* флаг активности лимита по температуре силового модуля */
  boolean_T igbtTlimFlg;

  /* флаг лимита по температуре мотора */
  boolean_T motorTlimFlg;

  /* флаг лимита по скорости ротора */
  boolean_T speedLimFlg;

  /* флаг лимита по току DC */
  boolean_T idcLimitFlg;
} FuncLimitFlag_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_inSignals_st_
#define DEFINED_TYPEDEF_FOR_inSignals_st_

typedef struct {
  /* Current of phase Ia */
  real32_T Ia;

  /* Current of phase Ib */
  real32_T Ib;

  /* Current of phase Ic */
  real32_T Ic;

  /* Current of dc bus */
  real32_T Idc;

  /* Voltage of DC bus */
  real32_T Vdc;

  /* Motor temperature in Celcius */
  real32_T Tmotor;

  /* Temperature of HeatSink */
  real32_T TheatSink;

  /* Temperature of IGBT 1 */
  real32_T Tigbt1;

  /* Temperature of IGBT 2 */
  real32_T Tigbt2;

  /* Temperature of IGBT 3 */
  real32_T Tigbt3;

  /* Sine signal voltage form Resolver */
  real32_T ResolverSine;

  /* Cosine signal voltage from resolver */
  real32_T ResolverCosine;
  uint16_T Fault;

  /* счетчик имульсов датчика положения */
  uint16_T HallCnt;

  /* Encoder AB position counter */
  uint16_T EncCnt;
} inSignals_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_ExtRef_st_
#define DEFINED_TYPEDEF_FOR_ExtRef_st_

typedef struct {
  /* Reference valuer
     In case of different Control modes act as Speed or Torque reference ( or Direct current ) */
  real32_T refValue;

  /* High allowed torque ( or current) */
  real32_T limitHigh;

  /* low allowed torque (or current) */
  real32_T limitLow;

  /* Control word with bitfiels */
  uint16_T cmd;
} ExtRef_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_TripLevels_st_
#define DEFINED_TYPEDEF_FOR_TripLevels_st_

typedef struct {
  /* Уровень защиты по мгновенному значенияю тока инвертора в амперах */
  uint16_T OverCurrent_level;

  /* Уровень защиты по максимальному напряжению DC шины в вольтах */
  uint16_T OverVoltage_level;

  /* Уровнь защиты по максимальной температуре силовой части в градусах */
  uint16_T OverTemp_power_level;

  /* Уровень уставки по максимальной температуре мотора в градусах */
  uint16_T OverTemp_motor_level;
} TripLevels_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Command_st_
#define DEFINED_TYPEDEF_FOR_Command_st_

/* Структура с питовыми полями команд алгоритма */
typedef struct {
  /* Бит разрешения работы преобразователя */
  boolean_T modOn;

  /* бит сброса ошибок преобразователя */
  boolean_T clrFault;

  /* бит управления моментом */
  boolean_T torqueMode;

  /* Команда режима управления скоростью */
  boolean_T speedMode;

  /* Бит режима торможения рекуперацией */
  boolean_T regenMode;

  /* режим виксированного угла для системы( замена угла PosObeserver) */
  boolean_T fixedAngle;

  /* Режим ручного управления напряжениями Vd Vq */
  boolean_T voltageControl;
} Command_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Status_st_
#define DEFINED_TYPEDEF_FOR_Status_st_

/* Структура с питовыми полями описаний статуса преобразователя */
typedef struct {
  /* Бит завершения режима тестирования */
  boolean_T testDone;

  /* бит активного режима тестирования */
  boolean_T testActive;

  /* Бит готовности преобразователя Нет ошибок */
  boolean_T ready;

  /* Бит активности( управление преобразователем). */
  boolean_T mod_Active;

  /* Бит глобальной ошибки */
  boolean_T globalFault;

  /* Бит активности режима управления моментом */
  boolean_T torqueMode;
  boolean_T speedMode;

  /* Активен режим торможения рекуперацией */
  boolean_T regenMode;

  /* активен режим работы в двигательном режиме */
  boolean_T driveMode;

  /* активен режим работы в генераторном режиме */
  boolean_T saturationActive;

  /* режим ослабления поля активен */
  boolean_T fwActive;
} Status_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Errors_st_
#define DEFINED_TYPEDEF_FOR_Errors_st_

typedef struct {
  /* Сброс защит */
  boolean_T ClearErrors;

  /* Суммарный флаг защиты */
  boolean_T GlobalError;

  /* Превышение максимально допустимого мгновенного тока */
  boolean_T OverCurrent;

  /* Превышение максимально допустимого значения напряжения Udc */
  boolean_T OverVoltage;

  /* Первышение максимально допустимой температуры силовой части */
  boolean_T OverTemperature_power;

  /* Первышение максимально допустимой температуры мотора */
  boolean_T OverTemperature_motor;

  /* Сигнал триггера аппаратной защиты */
  boolean_T HardWareFault;
} Errors_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Control_st_
#define DEFINED_TYPEDEF_FOR_Control_st_

/* Стурктура с сигналами которые используются в системе управления */
typedef struct {
  /* Структура команжных бит алгоритма */
  Command_st command;

  /* структура статусных бит алгоритма */
  Status_st stat;

  /* стуктура с ошибками привода */
  Errors_st errors;

  /* активные лимиты */
  FuncLimitFlag_st actLimits;

  /* Преобразованное занчение тока фазы А */
  real32_T Ia;

  /* Преобразованное занчение тока фазы B */
  real32_T Ib;

  /* Преобразованное занчение тока фазы C */
  real32_T Ic;

  /* Ток оси Бета в преобразовании Кларк */
  real32_T Ibeta;

  /* Ток оси альфа в преобразовании Кларк */
  real32_T Ialpha;

  /* амплитуда тока инвертора */
  real32_T Iampl;

  /* Максимальная амплитуда тока задания */
  real32_T ImaxRef;

  /* Задание по фазному току  в амперах аплитудное заначение */
  real32_T IqRefMTPA;
  real32_T IqRefSat;

  /* Активный Ток задания на регулятор тока после всех охраничений */
  real32_T IqRefReg;

  /* Задание по фазному току  в А амплитудное значение */
  real32_T IdRefMTPA;

  /* Ток задания от регулятора ослабления */
  real32_T IdRefSat;

  /* Реактивный ток задания на регултор тока */
  real32_T IdRefReg;

  /* Ток Q в преобразовании Парка */
  real32_T Iq;

  /* Ток оси D  преобразовании Парка */
  real32_T Id;

  /* Напряжение питания DC шины */
  real32_T Udc;

  /* Фильтрованное значение напряжения питания */
  real32_T UdcFiltered;

  /* амплитуда вектора напряжения */
  real32_T UmodActual;
  real32_T UmodRef;

  /* Напрядение оси Алфа в преобразовании кларк */
  real32_T Ualpha;

  /* Напряжение оси Бета в преобразовании Кларк */
  real32_T Ubeta;

  /* Задание напряжения по оси D после ограничений */
  real32_T UdRef;

  /* Задагние напряженя по оси Q после ограничений */
  real32_T UqRef;

  /* Напряжение задания по оси D в преобразовании ПАРКА */
  real32_T Ud;

  /* Напряжение задания оси Q в преобразовании ПАрка */
  real32_T Uq;

  /* электрический угол в радианах */
  real32_T ThetaElectrical;

  /* Скорректированное по фазе значение угла ротора */
  real32_T ThetaElectrCorr;

  /* Расчетная электрическая мощность в ваттах */
  real32_T Pelectrical;

  /* Фильтрованная мощность DC */
  real32_T PelectricalFilt;

  /* ток звена постоянного тока после расчета */
  real32_T Idc;

  /* Фильтрованный ток DC */
  real32_T IdcFilt;

  /* Электрическая скорость в рад\сек */
  real32_T Welectrical;

  /* Фильтрованная скорость электрическая */
  real32_T WelectricalFilt;

  /* Механическая скорость вала двигателя */
  real32_T Wmechanical;

  /* Температура мотора ( выбирается максимальаня из датичков) */
  real32_T Tmotor;

  /* Температура охладителя - максимальная из датчиков */
  real32_T TheatSink;

  /* Температура iGBT -максимальная из датчиков */
  real32_T Tigbt;

  /* Расчетный момент на валу двигателя */
  real32_T motorTorque;

  /* Доступный момент двигателя */
  real32_T motorTorqueLimit;

  /* Рассчитанное системой управления значение EMF */
  real32_T motorEmfCalc;

  /* Расчитанное системой управление смещение реального потока */
  real32_T motorFluxPosError;

  /* Время такта ШИМ */
  real32_T Tpwm;
  Motor_st motorParams;
} Control_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_TestedParams_st_
#define DEFINED_TYPEDEF_FOR_TestedParams_st_

typedef struct {
  real32_T Rs;
  real32_T Ld;
  real32_T Lq;
  real32_T Emf;
  uint16_T resolverPoles;
  uint16_T resolverDir;
  real32_T resolverZero;
  uint16_T encoderSteps;
  uint16_T encoderDir;
  uint16_T hallTable[6];
} TestedParams_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_PID_st_
#define DEFINED_TYPEDEF_FOR_PID_st_

typedef struct {
  /* Ошибка на входе регулятора */
  real32_T err;

  /* Значение насыщаения Out-OutPreSat */
  real32_T SatErr;

  /* Референсное значение */
  real32_T Ref;

  /* Обратная связь */
  real32_T Fdb;

  /* Выход пропорциональной части */
  real32_T Up;

  /* Выход интегральной части */
  real32_T Ui;

  /* Выход регулятора */
  real32_T Out;

  /* Ограничние выхода сверху */
  real32_T OutMax;

  /* ограничение выхода снизу */
  real32_T OutMin;

  /* Суммарный выход Ui+Up до ограничения */
  real32_T OutPreSat;

  /* Пропорциональный коэфф */
  real32_T Kp;

  /* Интегральный коэффциент */
  real32_T Ki;

  /* Коэфф обратной связи AntiWindUp */
  real32_T Kc;

  /* Бит сброса интгеральной части */
  boolean_T defaultBit;
} PID_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Observer_st_
#define DEFINED_TYPEDEF_FOR_Observer_st_

typedef struct {
  /* Время выборки (sample time) */
  real32_T Ts;

  /* Коэфф наблюдателя 1 */
  real32_T L1_w;

  /* Коэффициент наблюдателя 2 */
  real32_T L2_w;

  /* Не обработанное значение угла с датчика */
  real32_T SensorAngle_raw;

  /* Угол вычисленный для сенсора */
  real32_T SensorAngle_est;

  /* Электрический урол посчитанный через коэффциент приведения механического угла */
  real32_T ElectricalAngle_est;

  /* Вычисленный угол после наблюдателя */
  real32_T MechanicalAngle_est;

  /* Вычисленная скорость сенсора в рад\сек после наблюдателя */
  real32_T SensorSpeed_est;

  /* Скорость сенсора приведенная к электрической сокрости rad\s */
  real32_T ElectricalSpeed_est;

  /* Скорость сенсора приведенная к механической скорости */
  real32_T MechanicalSpeed_est;

  /* Коэфф динамики наблюдателя */
  real32_T L0;

  /* Коэффциент пересчата скорости сенсора вмеханическую скорость */
  real32_T SensorToMechanical;

  /* Коэффциент пересчата механической в электрическую скорость */
  real32_T MechanicalToElectrical;
} Observer_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_LPF_st_
#define DEFINED_TYPEDEF_FOR_LPF_st_

typedef struct {
  /* Коэффциент фильтрации k1 */
  real32_T k1;

  /* коээфциент фильтации k2 */
  real32_T k2;

  /* фильтрованное значение параметра */
  real32_T filteredVal;
} LPF_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_PWM_st_
#define DEFINED_TYPEDEF_FOR_PWM_st_

typedef struct {
  real32_T Udc;
  real32_T Ualpha;
  real32_T Ubeta;
  uint16_T sector;
  real32_T PRD_VAL;
  real32_T Ta;
  real32_T Tb;
  real32_T Tc;
} PWM_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_InternalRef_st_
#define DEFINED_TYPEDEF_FOR_InternalRef_st_

typedef struct {
  /* Referenced Motor Current */
  real32_T ImRef;
  boolean_T onSignal;
  boolean_T clrErrSignal;
} InternalRef_st;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_ZtFeL2tyDGYHfXrSWSsEDH_
#define DEFINED_TYPEDEF_FOR_struct_ZtFeL2tyDGYHfXrSWSsEDH_

typedef struct {
  motorTypeType motorType;
  real_T motorRs;
  real_T motorLd;
  real_T motorLq;
  real32_T motorSilence;
  real_T motorEmf;
  real32_T motorKt;
  real_T motorPoles;
  int16_T motorTmax;
  int16_T motorTmin;
  uint16_T motorWmax;
} struct_ZtFeL2tyDGYHfXrSWSsEDH;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_ZZlc6hTk7WaPGAHB0XhClD_
#define DEFINED_TYPEDEF_FOR_struct_ZZlc6hTk7WaPGAHB0XhClD_

typedef struct {
  real_T version;
  real_T controlSource;
  FuncLimits_st FuncLimits;
  struct_ZtFeL2tyDGYHfXrSWSsEDH MotorParams;
  posSensorType ActivePosSensor;
  real_T PosSensorResolution;
  real_T BW_reg_Inv;
  real_T FieldWeakingCoef;
  real_T LagCorrection;
  real_T MaxCurrent;
  real_T MaxIdCurrent;
  real_T MinIdCurrent;
  real_T MaxCharge;
  real_T MaxDischarge;
  real_T TrqKp;
  real_T TrqKi;
  real32_T TrqDampingFactor;
  uint16_T TrqDampingEnbale;
  real_T SpeedKp;
  real_T SpeedKi;
  real_T Observer_LO;
  real_T PwmBaseFrq;
  real_T Rate_Down;
  real_T Rate_Up;
  real_T ResolverPoles;
  uint16_T ResolverSignalSwap;
  real_T UdcFiltTp;
  real_T correctionTheta;
  real_T deadTimeVoltLoss;
  uint16_T hallTableDirect[6];
  uint16_T hallTableInverse[6];
  real_T phase_inverse;
  uint16_T StaticInductionFlg;
  real_T DecouplingEnable;
} struct_ZZlc6hTk7WaPGAHB0XhClD;

#endif
#endif                                /* RTW_HEADER_ControlSystem_v2_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
