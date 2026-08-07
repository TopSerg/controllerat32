/*
 * File: Identification.h
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

#ifndef RTW_HEADER_Identification_h_
#define RTW_HEADER_Identification_h_
#ifndef ControlSystem_v2_COMMON_INCLUDES_
#define ControlSystem_v2_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ControlSystem_v2_COMMON_INCLUDES_ */

#include "systemDefinations.h"
#include "testSignals.h"

extern void Identification_Init(void);
extern void Identification(boolean_T OnSignal, WorkModeType workMode,
  TestSignals_st *Out1_c);

#endif                                 /* RTW_HEADER_Identification_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
