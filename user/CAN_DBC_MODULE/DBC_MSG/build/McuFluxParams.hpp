
#pragma once
#include "CANdbcDriver.h"
#include "IMsg.hpp"
#include "optional"
extern "C" {
#include "McuFluxParams.h"
}

class McuFluxParams : public IMsgConv
{

public:
    float Zvflux;
    float Zvthetha;

    LocalErrorStats errStats = {};
    static const int cycleTime = 500;
    static const int cycleTimeReceiveGap = 10;

    int cycleTimeRemain = cycleTime;
	void pack(dbc_can_tx_message_type* transmitPacket);
	static std::optional<McuFluxParams> try_unpack(dbc_can_rx_message_type* receivedPacket);
    
	c_McuFluxParams toc_McuFluxParams();
	McuFluxParams() = default;
	McuFluxParams(c_McuFluxParams* self);
	McuFluxParams(dbc_can_rx_message_type* receivedPacket, LocalErrorStats errStats = {});
	
    virtual void msgPrepare(dbc_can_tx_message_type* transmitPacket);
	virtual bool tryMsgPrepare(dbc_can_tx_message_type* transmitPacket);
    virtual void msgParse(dbc_can_rx_message_type* receivedPacket);
	virtual bool tryMsgParse(dbc_can_rx_message_type* receivedPacket);



private:
    static const int ownCanId = 127;


	void saturateAdj();
	void rawPack(dbc_can_tx_message_type* transmitPacket);
    void UpdateGlobalState();
    void ReadGlobalState();
};
