
#ifndef MCU_FLUXPARAMS_H
#define MCU_FLUXPARAMS_H

#include <stdint.h>
#include "CANdbcDriver.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif




typedef struct unpack_c_McuFluxParams_res unpack_c_McuFluxParams_res;
typedef struct c_McuFluxParams c_McuFluxParams;

typedef void (*pack_McuFluxParams_callback)(c_McuFluxParams*, dbc_can_tx_message_type*);
typedef unpack_c_McuFluxParams_res (*unpack_McuFluxParams_callback)(dbc_can_rx_message_type*);

typedef struct c_McuFluxParams{
    pack_McuFluxParams_callback pack;
    unpack_McuFluxParams_callback unpack;

    float Zvflux;
    float Zvthetha;

} c_McuFluxParams;

typedef struct unpack_c_McuFluxParams_res{ 
    c_McuFluxParams val;
    bool is_valid;
} unpack_c_McuFluxParams_res;

void c_McuFluxParams_pack(void* self, dbc_can_tx_message_type* transmitPacket);
unpack_c_McuFluxParams_res c_McuFluxParams_unpack(dbc_can_rx_message_type* receivedPacket);

c_McuFluxParams c_McuFluxParams_new();

	extern c_McuFluxParams* cpT_McuFluxParams_gstate;

#ifdef __cplusplus
}
#endif
#endif
