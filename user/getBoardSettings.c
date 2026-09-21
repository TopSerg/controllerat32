#include "getBoardSettings.h"
#define MOTOR_LEFT  0
#define MOTOR_RIGHT 1
#define MOTOR_QS138 2

#define MOTOR_RESOLVER MOTOR_QS138

uint8_t getBoardSettings(boardAnalogSet_st * brd)
{
	uint16_t res = 0;

	switch(brd->boardCPU_ID_low)
	{
	case 0x587702b2:
		{
			/* плата инвертора в офисе*/
			brd->Iph_CH_K1 = 0.5496f;
			brd->Iph_CH_ZEROV = -1125.1f;
			brd->Udc_CH_K1 = 0.1862f;
			brd->Udc_CH_K2 = -4.4986f;
			res = 1;
			break;
		}
	case 0xdafbda2a:
		{
			/* плата инвертора в Троицке С права*/
			brd->Iph_CH_K1 = 0.5496f;
			brd->Iph_CH_ZEROV = -1125.1f;
			brd->Udc_CH_K1 = 0.1852f;
			brd->Udc_CH_K2 = -2.22f;
			res = 1;
			break;
		}
	case 0xe8499cee:
		{
			/* плата инвертора в Троицке слева*/
			brd->Iph_CH_K1 = 0.5496f;
			brd->Iph_CH_ZEROV = -1125.1f;
			brd->Udc_CH_K1 = 0.1894f;
			brd->Udc_CH_K2 = -4.927f;
			res = 1;
			break;
		}
	default:
		{
			/* плата инвертора в офисе*/
			brd->Iph_CH_K1 = 0.5496f;
			brd->Iph_CH_ZEROV = -1125.1f;
			brd->Udc_CH_K1 = 0.1901f;
			brd->Udc_CH_K2 = 6.844f;
			res = 0;
			break;
		}
	}
	
	/*Motor resolver settings */
#if MOTOR_RESOLVER == MOTOR_LEFT
	brd->resolverBase = 12;
	brd->resolverSwap = 1;
	brd->resolverShift = -1.08;
	
#endif

#if MOTOR_RESOLVER == MOTOR_RIGHT
	brd->resolverBase = 12;
	brd->resolverSwap = 1;
	brd->resolverShift = 0.50f;
	
#endif
	
#if MOTOR_RESOLVER == MOTOR_QS138
	/* PROVISIONAL QS138 SIN/COS front-end timing and electrical zero. The
	 * shift can be changed safely at runtime through calibration CAN 0x301. */
	brd->resolverBase = 1200;
	brd->resolverSwap = 0;
	brd->resolverShift = 2.7;
	
#endif	
	return res;
}
