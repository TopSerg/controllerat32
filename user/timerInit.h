/*
 * timerInit.h
 *
 *  Created on: 2023 Apr 4
 *      Author: on4ip
 */

#ifndef USER_TIMERINIT_H_
#define USER_TIMERINIT_H_

#include "at32f403a_407_conf.h"
#include "at32f403a_407_int.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
void timerInit(uint16_t freq,bool isrEna);
void timerProfilerinit(void);
void setTimerCallBack(void *);
extern float baseTickTime;
#define GET_ACTUAL_TIMECNT() (SysTick->VAL)
#define CONVERT_toUs(val) (((int32_t)(val)<0)?((float)(val+SysTick->LOAD)*baseTickTime):((float)val*baseTickTime))

#endif /* USER_TIMERINIT_H_ */
