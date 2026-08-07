//..
#include "CAN\CANReceiveDispatcher.h"
#include "CAN\CANSendDispatcher.h"

extern T_SEND_DISPATCHER SendDispatcher;
extern T_RECEIVE_DISPATCHER ReceiveDispatcher;
//..

//..
LoopHandler()
{
    //.. 
    ProcessReceivedPackets(&ReceiveDispatcher);
    SendPackets(&SendDispatcher);
    //..
}