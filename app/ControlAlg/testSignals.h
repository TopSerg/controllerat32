/*
 * File: testSignals.h
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

#ifndef RTW_HEADER_testSignals_h_
#define RTW_HEADER_testSignals_h_
#include "rtwtypes.h"

/* Структура с сигнлами испольемы при наладке и идентификации системы */
typedef struct {
  /* Тестовое задание тока Id */
  real32_T IdTest;

  /* Тестовое задание тока Шй */
  real32_T IqTest;

  /* Тестовое задание напяжения Vd в вольтах */
  real32_T VdTest;

  /* Тестовое задание напряжения Vq  в вольтах */
  real32_T VqTest;

  /* Тестововое задание частоты электрической в ras\s */
  real32_T WelTest;

  /* тестовый угол для работы системы */
  real32_T ThetaElTest;
  boolean_T fixedAngle;

  /* бит режима управления напряжением */
  boolean_T voltageControl;

  /* Бит активности тестового режима */
  boolean_T testActive;

  /* Бит окончания тестирования */
  boolean_T testDone;
} TestSignals_st;

#endif                                 /* RTW_HEADER_testSignals_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
