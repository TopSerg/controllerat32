/*
 * canInit.h
 *
 *  Created on: 2023 Apr 12
 *      Author: on4ip
 */

#ifndef USER_CAN2INIT_H_
#define USER_CAN2INIT_H_
#include "at32f403a_407_conf.h"
#include "at32f403a_407_int.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

enum CAN2_BUAD_RATES
{
	CAN2_BAUD_125k  = 0,
	CAN2_BAUD_250k,
	CAN2_BAUD_500k,
	CAN2_BAUD_1000k

};
void can2Init(uint16_t baud, bool isrEna);

void setCan2RxCallBack(void *);




#endif /* USER_CANINIT_H_ */