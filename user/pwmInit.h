/*
 * pwmInit.h
 *
 *  Created on: 2023 Mar 30
 *      Author: on4ip
 */

#ifndef USER_PWMINIT_H_
#define USER_PWMINIT_H_
#include "at32f403a_407_conf.h"
#include "at32f403a_407_int.h"
#include "stdint.h"
#include "stdbool.h"
/*!
 * @brief function initialize timer to 3ph mode with dt
 * @details function calculate base timer CNT value due refFreq, enable timer ISR
 * 			and set proper value for dead time
 * @param freq - Reference PWM frequence
 * @param isrEna- Generate isr or not
 */
void init3phPWM(uint16_t freq, bool isrEna);
static uint16_t pwmActive;
__STATIC_INLINE uint16_t pwmGetActiveState(void)
{
	return (pwmActive);
}
__STATIC_INLINE uint16_t pwmGetActiveBrkState(void)
{
	return (tmr_flag_get(TMR1, TMR_BRK_FLAG));
}
__STATIC_INLINE void pwmClearBRKFlag(void)
{
	tmr_flag_clear(TMR1, TMR_BRK_FLAG);
}
__STATIC_INLINE void pwmDisable(void) {
	tmr_output_enable(TMR1, FALSE);
	pwmActive = 0;
}
__STATIC_INLINE void pwmEnable(void) {
	if (!pwmGetActiveBrkState())
	{
		tmr_output_enable(TMR1, TRUE);
		pwmActive = 1;
	 }
}
__STATIC_INLINE void pwmCounterEnable(void) {
	/* enable tmr1 */
	tmr_counter_enable(TMR1, TRUE);
}

__STATIC_INLINE void pwmRefVal(uint16_t ch1, uint16_t ch2, uint16_t ch3) {
	tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, ch1);
	tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_2, ch2);
	tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_3, ch3);
}
__STATIC_INLINE uint32_t  pwmGetPrdVal(void)
{
	return tmr_period_value_get(TMR1);
}

void setPwmCallBack(void *pCallBack);
extern uint32_t pwmIsrCnt;

#endif /* USER_PWMINIT_H_ */
