/*
 * File: ControlSystem_v2_private.h
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

#ifndef RTW_HEADER_ControlSystem_v2_private_h_
#define RTW_HEADER_ControlSystem_v2_private_h_
#include "rtwtypes.h"
#include "systemDefinations.h"
#include "ControlSystem_v2.h"
#include "ControlSystem_v2_types.h"
#ifndef PORTABLE_WORDSIZES
#ifndef UCHAR_MAX
#include <limits.h>
#endif

#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( UINT_MAX != (0xFFFFU) ) || ( INT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized uint/int. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( ULONG_MAX != (0xFFFFFFFFUL) ) || ( LONG_MAX != (0x7FFFFFFFL) )
#error Code was generated for compiler with different sized ulong/long. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif
#endif                                 /* PORTABLE_WORDSIZES */

/* Imported (extern) pointer block parameters */
extern SystemParams_st *SystemSetActual;/* Variable: SystemSetActual
                                         * Referenced by: '<S288>/Constant7'
                                         */
extern uint16_T look2_is16lu32n16tu16_pbinlcse(int16_T u0, int16_T u1, const
  int16_T bp0[], const int16_T bp1[], const uint16_T table[], uint32_T
  prevIndex[], const uint32_T maxIndex[], uint32_T stride);
extern void getHallIndex(const uint16_T HallArr[6], uint16_T posCnt, uint16_T
  *hallindex_p);
extern void SwitchCaseActionSubsystem(real32_T IdRef_MTPA, real32_T IqRef_MTPA,
  real32_T *Out1, real32_T *Out2);
extern void refCurrentCalc(real32_T SilencyCoef, real32_T motorEmf, real32_T
  ImRef_c, motorTypeType motorType, real32_T *IdRef_MTPA, real32_T *IqRef_MTPA);
extern void workModeChoise(real32_T refValue_m, real32_T limitHigh, real32_T
  limitLow, regModeType regMode, boolean_T clearErrSignal_g, boolean_T
  onSignal_a, real32_T *ImRef_m, boolean_T *OnSignal, boolean_T *ClearErrSignal,
  DW_workModeChoise *localDW);

#endif                              /* RTW_HEADER_ControlSystem_v2_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
