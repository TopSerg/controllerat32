/*
 * canInit.c
 *
 *  Created on: 2023 Apr 12
 *      Author: on4ip
 */
#include "can2Init.h"
#include "CANReceiveDispatcher.h"
#include "CANSendDispatcher.h"

extern T_SEND_DISPATCHER SendDispatcher;
extern T_RECEIVE_DISPATCHER ReceiveDispatcher;

static void(*plocalCallBack)(can_rx_message_type*) = NULL;

void setCan2RxCallBack(void *pCallBack)
{
	plocalCallBack = pCallBack;
}
static void can_gpio_config(void)
{
	gpio_init_type gpio_init_struct;
	
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	
	gpio_pin_remap_config(CAN2_GMUX_0001, TRUE);
	
	gpio_default_para_init(&gpio_init_struct);
	/* can tx pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pins = GPIO_PINS_6;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOB, &gpio_init_struct);
	/* can rx pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = GPIO_PINS_5;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(GPIOB, &gpio_init_struct);
}
/**
 *  @brief  can configiguration.
 *  @param  none
 *  @retval none
 */
static void can_configuration(uint16_t baud)
{
	can_base_type can_base_struct;
	can_baudrate_type can_baudrate_struct;
	can_filter_init_type can_filter_init_struct;

	crm_periph_clock_enable(CRM_CAN2_PERIPH_CLOCK, TRUE);
	/* can base init */
	can_default_para_init(&can_base_struct);
	can_base_struct.mode_selection = CAN_MODE_COMMUNICATE;//| CAN_MODE_LOOPBACK;
	can_base_struct.ttc_enable = FALSE;
	can_base_struct.aebo_enable = TRUE;
	can_base_struct.aed_enable = TRUE;
	can_base_struct.prsf_enable = FALSE;
	can_base_struct.mdrsel_selection = CAN_DISCARDING_FIRST_RECEIVED;
	can_base_struct.mmssr_selection = CAN_SENDING_BY_ID;
	can_base_init(CAN2, &can_base_struct);

	/**
	 * @brief  set the baudrate of the can peripheral
	 * @param  can_x: select the can peripheral.
	 *         this parameter can be one of the following values:
	 *         CAN1,CAN2.
	 * @param  baudrate_div: baudrate division.
	 * @param  rsaw_size: resynchronization adjust width.
	 * @param  bts1_size: bit time segment 1.
	 * @param  bts2_size: bit time segment 2.
	 * @note   baudrate calculate method is:
	 *         baudrate = fpclk/(baudrate_div *(1 + bts1_size + bts2_size))
	 */
	switch (baud)
	{
	case CAN2_BAUD_125k:
		can_baudrate_default_para_init(&can_baudrate_struct);
		can_baudrate_struct.baudrate_div = 112;
		can_baudrate_struct.rsaw_size = CAN_RSAW_1TQ;
		can_baudrate_struct.bts1_size = CAN_BTS1_6TQ;
		can_baudrate_struct.bts2_size = CAN_BTS2_1TQ;
		can_baudrate_set(CAN2, &can_baudrate_struct);
		break;
	case CAN2_BAUD_250k:
		can_baudrate_default_para_init(&can_baudrate_struct);
		can_baudrate_struct.baudrate_div = 56;
		can_baudrate_struct.rsaw_size = CAN_RSAW_1TQ;
		can_baudrate_struct.bts1_size = CAN_BTS1_6TQ;
		can_baudrate_struct.bts2_size = CAN_BTS2_1TQ;
		can_baudrate_set(CAN2, &can_baudrate_struct);
		break;
	case CAN2_BAUD_500k:
		can_baudrate_default_para_init(&can_baudrate_struct);
		can_baudrate_struct.baudrate_div = 28;
		can_baudrate_struct.rsaw_size = CAN_RSAW_1TQ;
		can_baudrate_struct.bts1_size = CAN_BTS1_6TQ;
		can_baudrate_struct.bts2_size = CAN_BTS2_1TQ;
		can_baudrate_set(CAN2, &can_baudrate_struct);
		break;
	case CAN2_BAUD_1000k:
		can_baudrate_default_para_init(&can_baudrate_struct);
		can_baudrate_struct.baudrate_div = 14;
		can_baudrate_struct.rsaw_size = CAN_RSAW_1TQ;
		can_baudrate_struct.bts1_size = CAN_BTS1_6TQ;
		can_baudrate_struct.bts2_size = CAN_BTS2_1TQ;
		can_baudrate_set(CAN2, &can_baudrate_struct);
		break;
	default:

		break;

	} //end of baudrate selection

	/* can filter init */
	can_filter_init_struct.filter_activate_enable = TRUE;
	can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_MASK;
	can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;
	can_filter_init_struct.filter_number = 0;
	can_filter_init_struct.filter_bit = CAN_FILTER_32BIT;
	can_filter_init_struct.filter_id_high = 0;
	can_filter_init_struct.filter_id_low = 0;
	can_filter_init_struct.filter_mask_high = 0;
	can_filter_init_struct.filter_mask_low = 0;
	can_filter_init(CAN2, &can_filter_init_struct);

}
void can2Init(uint16_t baud, bool isrEna)
{
	can_gpio_config();
	can_configuration(baud);
	if (isrEna)
	{
		can_interrupt_enable(CAN2, CAN_RF0MIEN_INT, TRUE);
		can_interrupt_enable(CAN2, CAN_TCIEN_INT, TRUE);
		/* error interrupt enable */
		can_interrupt_enable(CAN2, CAN_ETRIEN_INT, TRUE);
		can_interrupt_enable(CAN2, CAN_EOIEN_INT, TRUE);
	}
}
void CAN2_TX_IRQHandler(void)
{
	dbc_transmitNextPacket(&SendDispatcher);

	if (can_flag_get(CAN2, CAN_TMEF_FLAG) != RESET)
	{
		can_flag_clear(CAN2, CAN_TMEF_FLAG);
		
	}
}

/**
 *  @brief  can1 interrupt function rx0
 *  @param  none
 *  @retval none
 */
void CAN2_RX0_IRQHandler(void)
{
	can_rx_message_type rx_message_struct;
	if (can_flag_get(CAN2, CAN_RF0MN_FLAG) != RESET)
	{
		do
		{
			can_message_receive(CAN2, CAN_RX_FIFO0, &rx_message_struct);
			if (plocalCallBack != NULL)
				plocalCallBack(&rx_message_struct);
		} while (can_receive_message_pending_get(CAN2, CAN_RX_FIFO0));
	}
}

/**
 *  @brief  can1 interrupt function se
 *  @param  none
 *  @retval none
 */
void CAN2_SE_IRQHandler(void)
{
	__IO uint32_t err_index = 0;
	if (can_flag_get(CAN2, CAN_ETR_FLAG) != RESET)
	{
		err_index = CAN2->ests & 0x70;
		can_flag_clear(CAN2, CAN_ETR_FLAG);
		/* error type is stuff error */
		if (err_index == 0x00000010)
		{
			/* when stuff error occur: in order to ensure communication normally,
			 user must restart can or send a frame of highest priority message here */
		}
	}
	if (can_flag_get(CAN2, CAN_EOIF_FLAG) != RESET)
	{
		
		can_flag_clear(CAN2, CAN_EOIF_FLAG);
	}
}
