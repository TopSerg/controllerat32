/*
 * File: TrqCalc.h
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

#ifndef RTW_HEADER_TrqCalc_h_
#define RTW_HEADER_TrqCalc_h_
#ifndef ControlSystem_v2_COMMON_INCLUDES_
#define ControlSystem_v2_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ControlSystem_v2_COMMON_INCLUDES_ */

/* Block signals and states (default storage) for system '<S376>/LdLq_Tables' */
typedef struct {
  uint32_T m_bpIndex[2];               /* '<S381>/Lq_lookUp' */
  uint32_T m_bpIndex_i[2];             /* '<S381>/Ld_lookUp' */
} DW_LdLq_Tables;

/* Block signals and states (default storage) for system '<S320>/TrqCalc' */
typedef struct {
  DW_LdLq_Tables LdLq_Tables_h;        /* '<S376>/LdLq_Tables' */
  real32_T Merge_e;                    /* '<S376>/Merge' */
  real32_T Merge1_i;                   /* '<S376>/Merge1' */
  real32_T Add_ct;                     /* '<S377>/Add' */
  real32_T Torque_calc;                /* '<S362>/Torque_el' */
  real32_T TeMAxCalcc;                 /* '<S362>/Torque_el1' */
  real32_T Product_ob;                 /* '<S380>/Product' */
  real32_T UnitDelay_DSTATE;           /* '<S386>/Unit Delay' */
  real32_T UnitDelay_DSTATE_n;         /* '<S389>/Unit Delay' */
} DW_TrqCalc;

extern void LdLq_Tables(real32_T Id, real32_T iq, real32_T *Lq_table, real32_T
  *Ld_table_c, DW_LdLq_Tables *localDW);
extern void TrqCalc(real32_T Id, real32_T Iq, real32_T ImaxRef, real32_T Emf,
                    uint16_T motorPoles, real32_T motorLq_k, real32_T motorLd_g,
                    real32_T tMotor, DW_TrqCalc *localDW);

#endif                                 /* RTW_HEADER_TrqCalc_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
