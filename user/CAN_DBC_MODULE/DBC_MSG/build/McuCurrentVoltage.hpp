
#pragma once
#include "CANdbcDriver.h"
#include "IMsg.hpp"
#include "optional"
extern "C" {
#include "McuCurrentVoltage.h"
}

class McuCurrentVoltage : public IMsgConv
{

public:
    float Ud;
    float Uq;
    float Id;
    float Iq;

    LocalErrorStats errStats = {};
    /* 50 Hz is fast enough to see commissioning transients without changing
     * the existing CAN message layout or the stand application. */
    static const int cycleTime = 20;
    static const int cycleTimeReceiveGap = 10;

    int cycleTimeRemain = cycleTime;
	void pack(dbc_can_tx_message_type* transmitPacket);
	static std::optional<McuCurrentVoltage> try_unpack(dbc_can_rx_message_type* receivedPacket);
    
	c_McuCurrentVoltage toc_McuCurrentVoltage();
	McuCurrentVoltage() = default;
	McuCurrentVoltage(c_McuCurrentVoltage* self);
	McuCurrentVoltage(dbc_can_rx_message_type* receivedPacket, LocalErrorStats errStats = {});
	
    virtual void msgPrepare(dbc_can_tx_message_type* transmitPacket);
	virtual bool tryMsgPrepare(dbc_can_tx_message_type* transmitPacket);
    virtual void msgParse(dbc_can_rx_message_type* receivedPacket);
	virtual bool tryMsgParse(dbc_can_rx_message_type* receivedPacket);



private:
    static const int ownCanId = 126;


	void saturateAdj();
	void rawPack(dbc_can_tx_message_type* transmitPacket);
    void UpdateGlobalState();
    void ReadGlobalState();
};
