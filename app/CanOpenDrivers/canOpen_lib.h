#pragma once
#include <stdio.h>
#include <stdint-gcc.h>

#include "canInit.h"

#include "CANopen.h"
#include "OD.h"
#include "CO_storageBlank.h"

/*Task function for freertos*/
void canopen_task(void *argument);

/*Idle fucntion for freertos*/
void canopen_loop(void);

int canopen_app_init();
void canopen_1ms(void);

uint8_t canSend(uint16_t id, uint8_t dlc, uint8_t *data);
uint8_t canOpen_rx_interrupt(can_rx_message_type*);
uint8_t can_tx_interrupt(void);

void CO_CANinterruptTx(CO_CANmodule_t *CANmodule);

void CO_CANinterruptRx(CO_CANmodule_t *CANmodule, CO_CANrxMsg_t *rcvMsg);
