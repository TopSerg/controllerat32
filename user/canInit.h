/*
 * canInit.h
 *
 *  Created on: 2023 Apr 12
 *      Author: on4ip
 */

#ifndef USER_CANINIT_H_
#define USER_CANINIT_H_
#include "at32f403a_407_conf.h"
#include "at32f403a_407_int.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

enum CAN_BUAR_RATES
{
	CAN_BAUD_125k=0,
	CAN_BAUD_250k,
	CAN_BAUD_500k,
	CAN_BAUD_1000k

};
void canInit(uint16_t baud,bool isrEna);

void setCanRxCallBack(void *);
void setCanTxCallBack(void *);




#endif /* USER_CANINIT_H_ */
