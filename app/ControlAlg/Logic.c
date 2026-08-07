/*
 * File: Logic.c
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
#include "Logic.h"
#include "systemDefinations.h"
#include "ControlSystem_v2_private.h"

/* Named constants for Chart: '<S3>/Logic' */
#define IN_ACTDISCHARGE                ((uint8_T)1U)
#define IN_CONTROL                     ((uint8_T)2U)
#define IN_FAULT                       ((uint8_T)3U)
#define IN_INITIALISATION              ((uint8_T)4U)
#define IN_READY                       ((uint8_T)5U)
#define IN_SLEEP                       ((uint8_T)6U)
#define IN_STANDBY                     ((uint8_T)7U)

/* Forward declaration for local functions */
static int16_T safe_cast_to_regModeType(int16_T input);
static int16_T safe_cast_to_WorkModeType(int16_T input);
static int16_T safe_cast_to_regModeType(int16_T input)
{
  int16_T output;

  /* Initialize output value to default value for regModeType (READY) */
  output = 0;
  if ((input >= 0) && (input <= 4)) {
    /* Set output value to input value if it is a member of regModeType */
    output = input;
  }

  return output;
}

static int16_T safe_cast_to_WorkModeType(int16_T input)
{
  int16_T output;

  /* Initialize output value to default value for WorkModeType (DEFAULT) */
  output = 0;
  if ((input == 0) || ((input >= 2) && (input <= 7))) {
    /* Set output value to input value if it is a member of WorkModeType */
    output = input;
  }

  return output;
}

/* Output and update for atomic system: '<S3>/Logic' */
void Logic(boolean_T GlobErr, uint16_T cmd, DW_Logic *localDW)
{
  /* Chart: '<S3>/Logic' */
  if ((uint16_T)localDW->is_active_c3_ControlSystem_v2 == 0U) {
    localDW->is_active_c3_ControlSystem_v2 = 1U;
    localDW->is_ProcCMD = IN_INITIALISATION;
    localDW->delayCnt = 0U;
    localDW->clearErrSignal_p = false;
    localDW->onSignal_e = false;
  } else {
    regModeType RequestedRegMode;
    RequestedRegMode = (regModeType)safe_cast_to_regModeType((int16_T)((uint16_T)
      (cmd & (uint16_T)0xF)));
    localDW->workMode = (WorkModeType)safe_cast_to_WorkModeType((int16_T)
      ((uint16_T)((uint16_T)(cmd >> 4U) & (uint16_T)0xF)));
    switch (localDW->is_ProcCMD) {
     case IN_ACTDISCHARGE:
      localDW->is_ProcCMD = IN_STANDBY;
      localDW->delayCnt = 0U;
      localDW->clearErrSignal_p = true;
      break;

     case IN_CONTROL:
      if (GlobErr) {
        localDW->is_ProcCMD = IN_FAULT;
        localDW->onSignal_e = false;
        localDW->delayCnt = 0U;
      } else if (RequestedRegMode == READY) {
        localDW->is_ProcCMD = IN_READY;
        localDW->delayCnt = 0U;
        localDW->clearErrSignal_p = false;
        localDW->onSignal_e = false;
      } else {
        localDW->onSignal_e = true;
        localDW->regMode = RequestedRegMode;

        /* modeProc(workLoc); */
      }
      break;

     case IN_FAULT:
      if (RequestedRegMode == READY) {
        localDW->is_ProcCMD = IN_STANDBY;
        localDW->delayCnt = 0U;
        localDW->clearErrSignal_p = true;
      } else {
        localDW->delayCnt++;
      }
      break;

     case IN_INITIALISATION:
      if (localDW->delayCnt > 50U) {
        localDW->is_ProcCMD = IN_STANDBY;
        localDW->delayCnt = 0U;
        localDW->clearErrSignal_p = true;
      } else {
        localDW->delayCnt++;

        /* Initialisation */
      }
      break;

     case IN_READY:
      if ((RequestedRegMode != READY) && (localDW->delayCnt > 5U)) {
        localDW->is_ProcCMD = IN_CONTROL;
      } else {
        localDW->delayCnt++;
      }
      break;

     case IN_SLEEP:
      switch (RequestedRegMode) {
       case READY:
        localDW->is_ProcCMD = IN_STANDBY;
        localDW->delayCnt = 0U;
        localDW->clearErrSignal_p = true;
        break;

       case Discharge:
        localDW->is_ProcCMD = IN_ACTDISCHARGE;
        break;

       default:
        /* no actions */
        break;
      }
      break;

     default:
      /* case IN_STANDBY: */
      if (localDW->delayCnt > 20U) {
        localDW->is_ProcCMD = IN_READY;
        localDW->delayCnt = 0U;
        localDW->clearErrSignal_p = false;
        localDW->onSignal_e = false;
      } else {
        localDW->delayCnt++;
      }
      break;
    }
  }

  /* End of Chart: '<S3>/Logic' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
