/*
 * File: utils.c
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
#include "utils.h"
#include "ControlSystem_v2_private.h"

/*
 * Output and update for atomic system:
 *    '<S201>/LPF_Udc'
 *    '<S201>/LPF_Udc1'
 *    '<S361>/LPF_FluxErr'
 *    '<S361>/LPF_FluxErr1'
 */
void LPF(real32_T InVal, real32_T K1, real32_T K2, real32_T lastVal, real32_T
         *FiltVal)
{
  /* Sum: '<S237>/Add' incorporates:
   *  Product: '<S237>/Product'
   *  Product: '<S237>/Product1'
   */
  *FiltVal = (InVal * K2) + (K1 * lastVal);
}

/*
 * Output and update for atomic system:
 *    '<S343>/RateLimiter'
 *    '<S339>/RateLimiter'
 */
void RateLimiter(real32_T rated, real32_T ref, real32_T down, real32_T up,
                 real32_T *out)
{
  real32_T tmp;

  /* CFunction: '<S357>/C RateLimiter' */
  *out = rated;
  tmp = *out + up;
  if (ref > tmp) {
    *out = tmp;
  } else {
    tmp = *out - down;
    if (ref < tmp) {
      *out = tmp;
    } else {
      *out = ref;
    }
  }

  /* End of CFunction: '<S357>/C RateLimiter' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
