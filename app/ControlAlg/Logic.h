/*
 * File: Logic.h
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

#ifndef RTW_HEADER_Logic_h_
#define RTW_HEADER_Logic_h_
#ifndef ControlSystem_v2_COMMON_INCLUDES_
#define ControlSystem_v2_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ControlSystem_v2_COMMON_INCLUDES_ */

#include "systemDefinations.h"

/* Block signals and states (default storage) for system '<S3>/Logic' */
typedef struct {
  WorkModeType workMode;               /* '<S3>/Logic' */
  regModeType regMode;                 /* '<S3>/Logic' */
  uint16_T delayCnt;                   /* '<S3>/Logic' */
  uint8_T is_ProcCMD;                  /* '<S3>/Logic' */
  uint8_T is_active_c3_ControlSystem_v2;/* '<S3>/Logic' */
  boolean_T onSignal_e;                /* '<S3>/Logic' */
  boolean_T clearErrSignal_p;          /* '<S3>/Logic' */
} DW_Logic;

extern void Logic(boolean_T GlobErr, uint16_T cmd, DW_Logic *localDW);

#endif                                 /* RTW_HEADER_Logic_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
