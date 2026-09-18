#ifndef MCU_RESOLVERCALIBRATION_H
#define MCU_RESOLVERCALIBRATION_H

#include <stdint.h>
#include "CANdbcDriver.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct unpack_c_McuResolverCalibration_res unpack_c_McuResolverCalibration_res;
typedef struct c_McuResolverCalibration c_McuResolverCalibration;

typedef void (*pack_McuResolverCalibration_callback)(c_McuResolverCalibration*, dbc_can_tx_message_type*);
typedef unpack_c_McuResolverCalibration_res (*unpack_McuResolverCalibration_callback)(dbc_can_rx_message_type*);

typedef struct c_McuResolverCalibration {
    pack_McuResolverCalibration_callback pack;
    unpack_McuResolverCalibration_callback unpack;

    float ResolverSine;
    float ResolverCosine;
    float ResolverTheta;
    float ResolverThetaCorr;
} c_McuResolverCalibration;

typedef struct unpack_c_McuResolverCalibration_res {
    c_McuResolverCalibration val;
    bool is_valid;
} unpack_c_McuResolverCalibration_res;

void c_McuResolverCalibration_pack(c_McuResolverCalibration* self, dbc_can_tx_message_type* transmitPacket);
unpack_c_McuResolverCalibration_res c_McuResolverCalibration_unpack(dbc_can_rx_message_type* receivedPacket);
c_McuResolverCalibration c_McuResolverCalibration_new();

extern c_McuResolverCalibration* cpT_McuResolverCalibration_gstate;

#ifdef __cplusplus
}
#endif
#endif
