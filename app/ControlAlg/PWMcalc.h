/*
 * File: PWMcalc.h
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

#ifndef RTW_HEADER_PWMcalc_h_
#define RTW_HEADER_PWMcalc_h_
#ifndef ControlSystem_v2_COMMON_INCLUDES_
#define ControlSystem_v2_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ControlSystem_v2_COMMON_INCLUDES_ */

extern void PWMcalc(real32_T Ualpha_n, real32_T Ubeta_c, real32_T Udc, boolean_T
                    modActive, real32_T *Ta, real32_T *Tb, real32_T *Tc);

#endif                                 /* RTW_HEADER_PWMcalc_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
