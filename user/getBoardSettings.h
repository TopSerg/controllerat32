#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	float Udc_CH_K1;
	float Udc_CH_K2;
	float Iph_CH_K1;
	float Iph_CH_ZEROV;
	uint16_t resolverBase;
	uint16_t resolverSwap;
	float resolverShift;
	uint32_t boardCPU_ID_low;
	uint32_t boardCPU_ID_High;
}boardAnalogSet_st;

uint8_t getBoardSettings(boardAnalogSet_st * brd);