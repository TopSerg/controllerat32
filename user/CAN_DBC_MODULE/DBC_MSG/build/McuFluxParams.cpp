

#include <stdint.h>
#include "CANdbcDriver.h"
#include <optional>
#include "IMsg.hpp"
#include "DbcDispatcher.hpp"
#include "McuFluxParams.hpp"

extern "C" {
    #include "CANSendDispatcher.h"
    #include "McuFluxParams.h"
}

c_McuFluxParams c_McuFluxParams_gstate;
c_McuFluxParams* cpT_McuFluxParams_gstate = &c_McuFluxParams_gstate;

void McuFluxParams::saturateAdj() {
    this->Zvflux = (float)SaturateSignalFloat(this->Zvflux, 0, 6.5535);
    this->Zvthetha = (float)SaturateSignalFloat(this->Zvthetha, 0, 6.5535);
    this->Zvelectricalspeed = (float)SaturateSignalFloat(this->Zvelectricalspeed, -3276.8, 3276.7);
    this->Zvcalibrationstatus = (uint8_t)SaturateSignalInteger(this->Zvcalibrationstatus, 0, 255);
    this->Zvcalibrationacksequence = (uint8_t)SaturateSignalInteger(this->Zvcalibrationacksequence, 0, 255);
};
void McuFluxParams::rawPack(dbc_can_tx_message_type* transmitPacket) {

    transmitPacket->message_id = 130;

	ClearCANDataField(transmitPacket);

    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Zvflux - 0) / 0.0001), 7, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Zvthetha - 0) / 0.0001), 23, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Zvelectricalspeed - -3276.8) / 0.1), 39, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)this->Zvcalibrationstatus, 55, 8);
    PackSignalToCANPacket(transmitPacket, (uint32_t)this->Zvcalibrationacksequence, 63, 8);
};

void McuFluxParams::pack(dbc_can_tx_message_type* transmitPacket){
	this->saturateAdj();
	this->rawPack(transmitPacket);
};
std::optional<McuFluxParams> McuFluxParams::try_unpack(dbc_can_rx_message_type* receivedPacket){ 
    if (true){  
        return McuFluxParams(receivedPacket); 
    } 
    else { 
        return {}; 
    }
};

void McuFluxParams::UpdateGlobalState()
{
        cpT_McuFluxParams_gstate->Zvflux = this->Zvflux;
    cpT_McuFluxParams_gstate->Zvthetha = this->Zvthetha;
    cpT_McuFluxParams_gstate->Zvelectricalspeed = this->Zvelectricalspeed;
    cpT_McuFluxParams_gstate->Zvcalibrationstatus = this->Zvcalibrationstatus;
    cpT_McuFluxParams_gstate->Zvcalibrationacksequence = this->Zvcalibrationacksequence;

}

void McuFluxParams::ReadGlobalState()
{
        this->Zvflux = cpT_McuFluxParams_gstate->Zvflux;
    this->Zvthetha = cpT_McuFluxParams_gstate->Zvthetha;
    this->Zvelectricalspeed = cpT_McuFluxParams_gstate->Zvelectricalspeed;
    this->Zvcalibrationstatus = cpT_McuFluxParams_gstate->Zvcalibrationstatus;
    this->Zvcalibrationacksequence = cpT_McuFluxParams_gstate->Zvcalibrationacksequence;

}
    
c_McuFluxParams McuFluxParams::toc_McuFluxParams(){
    c_McuFluxParams out;
    out.Zvflux = this->Zvflux;
    out.Zvthetha = this->Zvthetha;
    out.Zvelectricalspeed = this->Zvelectricalspeed;
    out.Zvcalibrationstatus = this->Zvcalibrationstatus;
    out.Zvcalibrationacksequence = this->Zvcalibrationacksequence;
    return out;
};


McuFluxParams::McuFluxParams(c_McuFluxParams* self)
    : Zvflux(self->Zvflux), Zvthetha(self->Zvthetha),
      Zvelectricalspeed(self->Zvelectricalspeed),
      Zvcalibrationstatus(self->Zvcalibrationstatus),
      Zvcalibrationacksequence(self->Zvcalibrationacksequence){};
McuFluxParams::McuFluxParams(dbc_can_rx_message_type* receivedPacket, LocalErrorStats errStats){
    this->Zvflux = (float)(UnpackSignalFromCANPacket(receivedPacket, 7, 16) * 0.0001 + 0);
    this->Zvthetha = (float)(UnpackSignalFromCANPacket(receivedPacket, 23, 16) * 0.0001 + 0);
    this->Zvelectricalspeed = (float)(UnpackSignalFromCANPacket(receivedPacket, 39, 16) * 0.1 - 3276.8);
    this->Zvcalibrationstatus = (uint8_t)UnpackSignalFromCANPacket(receivedPacket, 55, 8);
    this->Zvcalibrationacksequence = (uint8_t)UnpackSignalFromCANPacket(receivedPacket, 63, 8);
};

void McuFluxParams::msgPrepare(dbc_can_tx_message_type* transmitPacket) {
		InitPacket(transmitPacket);
    	this->pack(transmitPacket);
	}
	;

bool McuFluxParams::tryMsgPrepare(dbc_can_tx_message_type* transmitPacket)
{
	if (--cycleTimeRemain <= 0)
	{
        this->ReadGlobalState();
        cycleTimeRemain = cycleTime;
		msgPrepare(transmitPacket);
		return true;
	}
	return false;
}

void McuFluxParams::msgParse(dbc_can_rx_message_type* receivedPacket){
    *this = McuFluxParams(receivedPacket);
};
bool  McuFluxParams::tryMsgParse(dbc_can_rx_message_type* receivedPacket){
    --cycleTimeRemain;
    if (receivedPacket->message_id == ownCanId){
        cycleTimeRemain = cycleTime;
        if (true){
            *this = McuFluxParams(receivedPacket);
            this->UpdateGlobalState();
            return true;
        }  else {
            this->errStats.crcError++;
            return false;
        }
    } 
    else {
        if ((0 - cycleTimeReceiveGap) > cycleTimeRemain){ this->errStats.timeOut++; }
        return false;
    }

};

void c_McuFluxParams_pack(c_McuFluxParams* self, dbc_can_tx_message_type* transmitPacket){
    McuFluxParams _self(self);
    _self.pack(transmitPacket);
}

unpack_c_McuFluxParams_res c_McuFluxParams_unpack(dbc_can_rx_message_type* receivedPacket){
    unpack_c_McuFluxParams_res result;
	result.is_valid = false;
    std::optional<McuFluxParams> parsed_struct = McuFluxParams::try_unpack(receivedPacket);
    if (parsed_struct) {
		result.is_valid = true;
		result.val = parsed_struct.value().toc_McuFluxParams();
	}
    return result;
}

c_McuFluxParams c_McuFluxParams_new(){
    c_McuFluxParams out;
    out.pack = c_McuFluxParams_pack;
    out.unpack = c_McuFluxParams_unpack;
    return out;
}
