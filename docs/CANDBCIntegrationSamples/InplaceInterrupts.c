//..
#include "CANReceiveDispatcher.h"
#include "CANSendDispatcher.h"

extern T_SEND_DISPATCHER SendDispatcher;
extern T_RECEIVE_DISPATCHER ReceiveDispatcher;
//..

//..
/**
  * @brief  this function handles USB High Priority or CAN1 TX handler.
  * @param  none
  * @retval none
  */
void USBFS_H_CAN1_TX_IRQHandler(void)
{
    CANTXInterruptHandler(&SendDispatcher);
}

/**
  * @brief  this function handles USB Low  Priority or CAN1 RX0 handler.
  * @param  none
  * @retval none
  */
void USBFS_L_CAN1_RX0_IRQHandler(void)
{
    CANRXInterruptHandler(&ReceiveDispatcher);
}
//..