#include <stdint.h>
#include "CANdbcDriver.h"
#include <optional>
#include "IMsg.hpp"
#include "DbcDispatcher.hpp"
#include "McuResolverCalibration.hpp"

extern "C" {
    #include "CANSendDispatcher.h"
    #include "McuResolverCalibration.h"
}

c_McuResolverCalibration c_McuResolverCalibration_gstate;
c_McuResolverCalibration* cpT_McuResolverCalibration_gstate = &c_McuResolverCalibration_gstate;

void McuResolverCalibration::saturateAdj() {
    this->ResolverSine = (float)SaturateSignalFloat(this->ResolverSine, -32768, 32767);
    this->ResolverCosine = (float)SaturateSignalFloat(this->ResolverCosine, -32768, 32767);
    this->ResolverTheta = (float)SaturateSignalFloat(this->ResolverTheta, 0, 6.5535);
    this->ResolverThetaCorr = (float)SaturateSignalFloat(this->ResolverThetaCorr, 0, 6.5535);
}

void McuResolverCalibration::rawPack(dbc_can_tx_message_type* transmitPacket) {
    transmitPacket->message_id = 129;
    ClearCANDataField(transmitPacket);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->ResolverSine + 32768.0f)), 7, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->ResolverCosine + 32768.0f)), 23, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)(this->ResolverTheta / 0.0001f), 39, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)(this->ResolverThetaCorr / 0.0001f), 55, 16);
}

void McuResolverCalibration::pack(dbc_can_tx_message_type* transmitPacket) {
    this->saturateAdj();
    this->rawPack(transmitPacket);
}

std::optional<McuResolverCalibration> McuResolverCalibration::try_unpack(dbc_can_rx_message_type* receivedPacket) {
    return McuResolverCalibration(receivedPacket);
}

void McuResolverCalibration::UpdateGlobalState() {
    cpT_McuResolverCalibration_gstate->ResolverSine = this->ResolverSine;
    cpT_McuResolverCalibration_gstate->ResolverCosine = this->ResolverCosine;
    cpT_McuResolverCalibration_gstate->ResolverTheta = this->ResolverTheta;
    cpT_McuResolverCalibration_gstate->ResolverThetaCorr = this->ResolverThetaCorr;
}

void McuResolverCalibration::ReadGlobalState() {
    this->ResolverSine = cpT_McuResolverCalibration_gstate->ResolverSine;
    this->ResolverCosine = cpT_McuResolverCalibration_gstate->ResolverCosine;
    this->ResolverTheta = cpT_McuResolverCalibration_gstate->ResolverTheta;
    this->ResolverThetaCorr = cpT_McuResolverCalibration_gstate->ResolverThetaCorr;
}

c_McuResolverCalibration McuResolverCalibration::toc_McuResolverCalibration() {
    c_McuResolverCalibration out;
    out.ResolverSine = this->ResolverSine;
    out.ResolverCosine = this->ResolverCosine;
    out.ResolverTheta = this->ResolverTheta;
    out.ResolverThetaCorr = this->ResolverThetaCorr;
    return out;
}

McuResolverCalibration::McuResolverCalibration(c_McuResolverCalibration* self)
    : ResolverSine(self->ResolverSine), ResolverCosine(self->ResolverCosine),
      ResolverTheta(self->ResolverTheta), ResolverThetaCorr(self->ResolverThetaCorr) {}

McuResolverCalibration::McuResolverCalibration(dbc_can_rx_message_type* receivedPacket, LocalErrorStats errStats) {
    this->ResolverSine = (float)UnpackSignalFromCANPacket(receivedPacket, 7, 16) - 32768.0f;
    this->ResolverCosine = (float)UnpackSignalFromCANPacket(receivedPacket, 23, 16) - 32768.0f;
    this->ResolverTheta = (float)UnpackSignalFromCANPacket(receivedPacket, 39, 16) * 0.0001f;
    this->ResolverThetaCorr = (float)UnpackSignalFromCANPacket(receivedPacket, 55, 16) * 0.0001f;
}

void McuResolverCalibration::msgPrepare(dbc_can_tx_message_type* transmitPacket) {
    InitPacket(transmitPacket);
    this->pack(transmitPacket);
}

bool McuResolverCalibration::tryMsgPrepare(dbc_can_tx_message_type* transmitPacket) {
    if (--cycleTimeRemain <= 0) {
        this->ReadGlobalState();
        cycleTimeRemain = cycleTime;
        msgPrepare(transmitPacket);
        return true;
    }
    return false;
}

void McuResolverCalibration::msgParse(dbc_can_rx_message_type* receivedPacket) {
    *this = McuResolverCalibration(receivedPacket);
}

bool McuResolverCalibration::tryMsgParse(dbc_can_rx_message_type* receivedPacket) {
    --cycleTimeRemain;
    if (receivedPacket->message_id == ownCanId) {
        cycleTimeRemain = cycleTime;
        *this = McuResolverCalibration(receivedPacket);
        this->UpdateGlobalState();
        return true;
    }
    if ((0 - cycleTimeReceiveGap) > cycleTimeRemain) {
        this->errStats.timeOut++;
    }
    return false;
}

void c_McuResolverCalibration_pack(c_McuResolverCalibration* self, dbc_can_tx_message_type* transmitPacket) {
    McuResolverCalibration _self(self);
    _self.pack(transmitPacket);
}

unpack_c_McuResolverCalibration_res c_McuResolverCalibration_unpack(dbc_can_rx_message_type* receivedPacket) {
    unpack_c_McuResolverCalibration_res result;
    result.is_valid = false;
    std::optional<McuResolverCalibration> parsed = McuResolverCalibration::try_unpack(receivedPacket);
    if (parsed) {
        result.is_valid = true;
        result.val = parsed.value().toc_McuResolverCalibration();
    }
    return result;
}

c_McuResolverCalibration c_McuResolverCalibration_new() {
    c_McuResolverCalibration out;
    out.pack = c_McuResolverCalibration_pack;
    out.unpack = c_McuResolverCalibration_unpack;
    return out;
}
