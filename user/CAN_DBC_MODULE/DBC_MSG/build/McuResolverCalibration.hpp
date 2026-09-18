#pragma once
#include "CANdbcDriver.h"
#include "IMsg.hpp"
#include "optional"
extern "C" {
#include "McuResolverCalibration.h"
}

class McuResolverCalibration : public IMsgConv
{
public:
    float ResolverSine;
    float ResolverCosine;
    float ResolverTheta;
    float ResolverThetaCorr;

    LocalErrorStats errStats = {};
    static const int cycleTime = 20;
    static const int cycleTimeReceiveGap = 10;

    int cycleTimeRemain = cycleTime;
    void pack(dbc_can_tx_message_type* transmitPacket);
    static std::optional<McuResolverCalibration> try_unpack(dbc_can_rx_message_type* receivedPacket);

    c_McuResolverCalibration toc_McuResolverCalibration();
    McuResolverCalibration() = default;
    McuResolverCalibration(c_McuResolverCalibration* self);
    McuResolverCalibration(dbc_can_rx_message_type* receivedPacket, LocalErrorStats errStats = {});

    virtual void msgPrepare(dbc_can_tx_message_type* transmitPacket);
    virtual bool tryMsgPrepare(dbc_can_tx_message_type* transmitPacket);
    virtual void msgParse(dbc_can_rx_message_type* receivedPacket);
    virtual bool tryMsgParse(dbc_can_rx_message_type* receivedPacket);

private:
    static const int ownCanId = 129;

    void saturateAdj();
    void rawPack(dbc_can_tx_message_type* transmitPacket);
    void UpdateGlobalState();
    void ReadGlobalState();
};
