/*
 * File: indTable.h
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

#ifndef RTW_HEADER_indTable_h_
#define RTW_HEADER_indTable_h_
#include "rtwtypes.h"
#include "ControlSystem_v2_types.h"

typedef struct {
  int16_T IdVect[50];
  int16_T IqVect[50];
  uint16_T Ldmatr[2500];
  uint16_T Lqmatr[2500];
} LookUpTable;

typedef struct {
	int16_T IdVect[50];
	int16_T IqVect[50];
	real32_T Ldmatr[2500];
	real32_T Lqmatr[2500];
} LookUpTableGen;

/* Exported data declaration */

/* Const memory section */
/* Declaration for custom storage class: Const */
extern const LookUpTable LookUpData;   /* Referenced by:
                                        * '<S381>/Ld_lookUp'
                                        * '<S381>/Lq_lookUp'
                                        */

extern LookUpTableGen LookUpDataGenerate;

#endif                                 /* RTW_HEADER_indTable_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
