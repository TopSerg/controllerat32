

#include <stdint.h>
#include "CANdbcDriver.h"
#include <optional>
#include "IMsg.hpp"
#include "DbcDispatcher.hpp"
#include "McuCurrentVoltage.hpp"

extern "C" {
    #include "CANSendDispatcher.h"
    #include "McuCurrentVoltage.h"
}

c_McuCurrentVoltage c_McuCurrentVoltage_gstate;
c_McuCurrentVoltage* cpT_McuCurrentVoltage_gstate = &c_McuCurrentVoltage_gstate;

void McuCurrentVoltage::saturateAdj() {
    this->Ud = (float)SaturateSignalFloat(this->Ud, -3276, 3277);
    this->Uq = (float)SaturateSignalFloat(this->Uq, -3276, 3277);
    this->Id = (float)SaturateSignalFloat(this->Id, -3276, 3277);
    this->Iq = (float)SaturateSignalFloat(this->Iq, -3276, 3277);
};
void McuCurrentVoltage::rawPack(dbc_can_tx_message_type* transmitPacket) {

    transmitPacket->message_id = 126;

	ClearCANDataField(transmitPacket);

    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Ud - -3276) / 0.1), 7, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Uq - -3276) / 0.1), 23, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Id - -3276) / 0.1), 39, 16);
    PackSignalToCANPacket(transmitPacket, (uint32_t)((this->Iq - -3276) / 0.1), 55, 16);
};

void McuCurrentVoltage::pack(dbc_can_tx_message_type* transmitPacket){
	this->saturateAdj();
	this->rawPack(transmitPacket);
};
std::optional<McuCurrentVoltage> McuCurrentVoltage::try_unpack(dbc_can_rx_message_type* receivedPacket){ 
    if (true){  
        return McuCurrentVoltage(receivedPacket); 
    } 
    else { 
        return {}; 
    }
};

void McuCurrentVoltage::UpdateGlobalState()
{
        cpT_McuCurrentVoltage_gstate->Ud = this->Ud;
    cpT_McuCurrentVoltage_gstate->Uq = this->Uq;
    cpT_McuCurrentVoltage_gstate->Id = this->Id;
    cpT_McuCurrentVoltage_gstate->Iq = this->Iq;

}

void McuCurrentVoltage::ReadGlobalState()
{
        this->Ud = cpT_McuCurrentVoltage_gstate->Ud;
    this->Uq = cpT_McuCurrentVoltage_gstate->Uq;
    this->Id = cpT_McuCurrentVoltage_gstate->Id;
    this->Iq = cpT_McuCurrentVoltage_gstate->Iq;

}
    
c_McuCurrentVoltage McuCurrentVoltage::toc_McuCurrentVoltage(){
    c_McuCurrentVoltage out;
    out.Ud = this->Ud;
    out.Uq = this->Uq;
    out.Id = this->Id;
    out.Iq = this->Iq;
    return out;
};


McuCurrentVoltage::McuCurrentVoltage(c_McuCurrentVoltage* self):Ud(self->Ud), Uq(self->Uq), Id(self->Id), Iq(self->Iq){};
McuCurrentVoltage::McuCurrentVoltage(dbc_can_rx_message_type* receivedPacket, LocalErrorStats errStats){
    this->Ud = (float)(UnpackSignalFromCANPacket(receivedPacket, 7, 16) * 0.1 + -3276);
    this->Uq = (float)(UnpackSignalFromCANPacket(receivedPacket, 23, 16) * 0.1 + -3276);
    this->Id = (float)(UnpackSignalFromCANPacket(receivedPacket, 39, 16) * 0.1 + -3276);
    this->Iq = (float)(UnpackSignalFromCANPacket(receivedPacket, 55, 16) * 0.1 + -3276);
};

void McuCurrentVoltage::msgPrepare(dbc_can_tx_message_type* transmitPacket) {
		InitPacket(transmitPacket);
    	this->pack(transmitPacket);
	}
	;

bool McuCurrentVoltage::tryMsgPrepare(dbc_can_tx_message_type* transmitPacket)
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

void McuCurrentVoltage::msgParse(dbc_can_rx_message_type* receivedPacket){
    *this = McuCurrentVoltage(receivedPacket);
};
bool  McuCurrentVoltage::tryMsgParse(dbc_can_rx_message_type* receivedPacket){
    --cycleTimeRemain;
    if (receivedPacket->message_id == ownCanId){
        cycleTimeRemain = cycleTime;
        if (true){
            *this = McuCurrentVoltage(receivedPacket);
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

void c_McuCurrentVoltage_pack(c_McuCurrentVoltage* self, dbc_can_tx_message_type* transmitPacket){
    McuCurrentVoltage _self(self);
    _self.pack(transmitPacket);
}

unpack_c_McuCurrentVoltage_res c_McuCurrentVoltage_unpack(dbc_can_rx_message_type* receivedPacket){
    unpack_c_McuCurrentVoltage_res result;
	result.is_valid = false;
    std::optional<McuCurrentVoltage> parsed_struct = McuCurrentVoltage::try_unpack(receivedPacket);
    if (parsed_struct) {
		result.is_valid = true;
		result.val = parsed_struct.value().toc_McuCurrentVoltage();
	}
    return result;
}

c_McuCurrentVoltage c_McuCurrentVoltage_new(){
    c_McuCurrentVoltage out;
    out.pack = c_McuCurrentVoltage_pack;
    out.unpack = c_McuCurrentVoltage_unpack;
    return out;
}
