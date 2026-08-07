/*
 * adcInit.c
 *
 *  Created on: 2023 Mar 31
 *      Author: on4ip
 */
#include "adcInit.h"
#include "at32f403a_407_board.h"

/* adc data ready flags*/
static uint16_t adc1OK = 0, adc2OK = 0, adc3OK = 0; 
static void(*pAdclocalCallBack)(void) = NULL;
void setAdcCallBack(void *pCallBack)
{
	pAdclocalCallBack = pCallBack;
}

typedef struct
{
	uint8_t chNum;
	uint8_t chSequence;
	uint8_t chAdcSample;
} chSettings_st;
chSettings_st chPreemptSettings[MAX_PREEMPT_CH];
chSettings_st chOrdinaySettings[MAX_ORDINARY_CH];
static void setChannelsSettings(void)
{
	/*set preemt channels*/

	/* ADC 1*/
	chPreemptSettings[IA_CH].chNum = ADC_CHANNEL_1;
	chPreemptSettings[IA_CH].chSequence = ADC_PREEMPT_CHANNEL_1;
	chPreemptSettings[IA_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	
	chPreemptSettings[UDC_CH].chNum = ADC_CHANNEL_15;
	chPreemptSettings[UDC_CH].chSequence = ADC_PREEMPT_CHANNEL_2;
	chPreemptSettings[UDC_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	
	chPreemptSettings[RES_SIN_CH].chNum = ADC_CHANNEL_7;
	chPreemptSettings[RES_SIN_CH].chSequence = ADC_PREEMPT_CHANNEL_3;
	chPreemptSettings[RES_SIN_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	/* ADC 2*/
	chPreemptSettings[IB_CH].chNum = ADC_CHANNEL_2;
	chPreemptSettings[IB_CH].chSequence = ADC_PREEMPT_CHANNEL_1;
	chPreemptSettings[IB_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	
	chPreemptSettings[RES_COS_CH].chNum = ADC_CHANNEL_14;
	chPreemptSettings[RES_COS_CH].chSequence = ADC_PREEMPT_CHANNEL_2;
	chPreemptSettings[RES_COS_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	/* ADC 3*/
	chPreemptSettings[IC_CH].chNum = ADC_CHANNEL_3;
	chPreemptSettings[IC_CH].chSequence = ADC_PREEMPT_CHANNEL_1;
	chPreemptSettings[IC_CH].chAdcSample = ADC_SAMPLETIME_7_5;
	
	
	
	/* set ordinary channels 	 */
	chOrdinaySettings[BRAKE_CH].chNum = ADC_CHANNEL_6;
	chOrdinaySettings[BRAKE_CH].chSequence = BRAKE_CH + 1;
	chOrdinaySettings[BRAKE_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	chOrdinaySettings[THROTTLE_CH].chNum = ADC_CHANNEL_13;
	chOrdinaySettings[THROTTLE_CH].chSequence = THROTTLE_CH + 1;
	chOrdinaySettings[THROTTLE_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	chOrdinaySettings[MOTOR_TEMPR_CH].chNum = ADC_CHANNEL_2;
	chOrdinaySettings[MOTOR_TEMPR_CH].chSequence = MOTOR_TEMPR_CH + 1;
	chOrdinaySettings[MOTOR_TEMPR_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	/*IGBT tempeareture signals*/
	
	chOrdinaySettings[IGBT_1_CH].chNum = ADC_CHANNEL_10;
	chOrdinaySettings[IGBT_1_CH].chSequence = IGBT_1_CH + 1;
	chOrdinaySettings[IGBT_1_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	
	chOrdinaySettings[IGBT_2_CH].chNum = ADC_CHANNEL_11;
	chOrdinaySettings[IGBT_2_CH].chSequence = IGBT_2_CH + 1;
	chOrdinaySettings[IGBT_2_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	
	chOrdinaySettings[IGBT_3_CH].chNum = ADC_CHANNEL_12;
	chOrdinaySettings[IGBT_3_CH].chSequence = IGBT_3_CH + 1;
	chOrdinaySettings[IGBT_3_CH].chAdcSample = ADC_SAMPLETIME_13_5;
	/**/
	chOrdinaySettings[HEATSINK_CH].chNum = ADC_CHANNEL_6;
	chOrdinaySettings[HEATSINK_CH].chSequence = HEATSINK_CH + 1;
	chOrdinaySettings[HEATSINK_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	chOrdinaySettings[BACK_SINK_CH].chNum = ADC_CHANNEL_13;
	chOrdinaySettings[BACK_SINK_CH].chSequence = BACK_SINK_CH + 1;
	chOrdinaySettings[BACK_SINK_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	chOrdinaySettings[UINT_ERR_CH].chNum = ADC_CHANNEL_5;
	chOrdinaySettings[UINT_ERR_CH].chSequence = UINT_ERR_CH + 1;
	chOrdinaySettings[UINT_ERR_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	chOrdinaySettings[UINT_REF_CH].chNum = ADC_CHANNEL_17;
	chOrdinaySettings[UINT_REF_CH].chSequence = UINT_REF_CH + 1;
	chOrdinaySettings[UINT_REF_CH].chAdcSample = ADC_SAMPLETIME_13_5;

	chOrdinaySettings[TEMPR_INT_CH].chNum = ADC_CHANNEL_16;
	chOrdinaySettings[TEMPR_INT_CH].chSequence = TEMPR_INT_CH + 1;
	chOrdinaySettings[TEMPR_INT_CH].chAdcSample = ADC_SAMPLETIME_13_5;
}
/**
 * @brief  gpio configuration.
 * @param  none
 * @retval none
 */
static void gpio_config(void)
{
	gpio_init_type gpio_initstructure;
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

	gpio_default_para_init(&gpio_initstructure);
	gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
	gpio_initstructure.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_3 | GPIO_PINS_4
			| GPIO_PINS_5 | GPIO_PINS_6 | GPIO_PINS_7;
	gpio_init(GPIOA, &gpio_initstructure);

	gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
	gpio_initstructure.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_3 |  GPIO_PINS_4
			| GPIO_PINS_5;
	gpio_init(GPIOC, &gpio_initstructure);
}

__IO uint32_t adc1_ordinary_valuetab[MAX_ORDINARY_CH];
/**
 * @brief  dma configuration.
 * @param  none
 * @retval none
 */
static void dma_config(void)
{
	dma_init_type dma_init_struct;
	crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
	nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);
	dma_reset(DMA1_CHANNEL1);
	dma_default_para_init(&dma_init_struct);
	dma_init_struct.buffer_size = MAX_ORDINARY_CH;
	dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
	dma_init_struct.memory_base_addr = (uint32_t) adc1_ordinary_valuetab;
	dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_WORD;
	dma_init_struct.memory_inc_enable = TRUE;
	dma_init_struct.peripheral_base_addr = (uint32_t) &(ADC1->odt);
	dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_WORD;
	dma_init_struct.peripheral_inc_enable = FALSE;
	dma_init_struct.priority = DMA_PRIORITY_HIGH;
	dma_init_struct.loop_mode_enable = TRUE;
	dma_init(DMA1_CHANNEL1, &dma_init_struct);

	dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, FALSE);
	dma_channel_enable(DMA1_CHANNEL1, TRUE);
}
void adcInit(bool isrEna)
{
	gpio_config();
	dma_config();
	setChannelsSettings();
	adc_base_config_type adc_base_struct;
	crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_ADC2_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_ADC3_PERIPH_CLOCK, TRUE);
	crm_adc_clock_div_set(CRM_ADC_DIV_4);

	/* select combine mode */
	adc_combine_mode_select(ADC_INDEPENDENT_MODE);
	
	adc_base_default_para_init(&adc_base_struct);
	adc_base_struct.sequence_mode = TRUE;
	adc_base_struct.repeat_mode = TRUE;
	adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
	adc_base_struct.ordinary_channel_length = MAX_ORDINARY_CH;
	adc_base_config(ADC1, &adc_base_struct);
	uint8_t i = 0;
	for (i = 0; i < MAX_ORDINARY_CH; i++)
	{
		adc_ordinary_channel_set(ADC1,
			chOrdinaySettings[i].chNum,
			chOrdinaySettings[i].chSequence,
			chOrdinaySettings[i].chAdcSample);
	}
	adc_dma_mode_enable(ADC1, TRUE);
	/* set converter start trigger */
	adc_ordinary_conversion_trigger_set(ADC1,
		ADC12_ORDINARY_TRIG_SOFTWARE,
		TRUE);
	/* enable internal temperature sensor and internal VrefMonitor*/
	adc_tempersensor_vintrv_enable(TRUE);


	/*config preempt mode*/
	adc_preempt_channel_length_set(ADC1, 3);
	
	adc_preempt_channel_set(ADC1,
		chPreemptSettings[IA_CH].chNum,
		chPreemptSettings[IA_CH].chSequence+1,
		chPreemptSettings[IA_CH].chAdcSample);
	adc_preempt_channel_set(ADC1,
		chPreemptSettings[UDC_CH].chNum,
		chPreemptSettings[UDC_CH].chSequence+1,
		chPreemptSettings[UDC_CH].chAdcSample);
	adc_preempt_channel_set(ADC1,
		chPreemptSettings[RES_SIN_CH].chNum,
		chPreemptSettings[RES_SIN_CH].chSequence+1,
		chPreemptSettings[RES_SIN_CH].chAdcSample);
	adc_preempt_conversion_trigger_set(ADC1,
		ADC12_PREEMPT_TRIG_TMR1TRGOUT,
		TRUE);
	adc_preempt_auto_mode_enable(ADC1, FALSE);
	
	/* config adc 2 */
	adc_base_default_para_init(&adc_base_struct);
	adc_base_struct.sequence_mode = TRUE;
	adc_base_struct.repeat_mode = FALSE;
	adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
	adc_base_config(ADC2, &adc_base_struct);

	adc_preempt_channel_length_set(ADC2, 4);
	adc_preempt_channel_set(ADC2,
		chPreemptSettings[IB_CH].chNum,
		chPreemptSettings[IB_CH].chSequence+1,
		chPreemptSettings[IB_CH].chAdcSample);
	adc_preempt_channel_set(ADC2,
		chPreemptSettings[RES_COS_CH].chNum,
		chPreemptSettings[RES_COS_CH].chSequence+1,
		chPreemptSettings[RES_COS_CH].chAdcSample);

	adc_preempt_conversion_trigger_set(ADC2,
		ADC12_PREEMPT_TRIG_TMR1TRGOUT,
		TRUE);
	adc_preempt_auto_mode_enable(ADC2, FALSE);


	/* config adc 3*/
	adc_base_default_para_init(&adc_base_struct);
	adc_base_struct.sequence_mode = TRUE;
	adc_base_struct.repeat_mode = FALSE;
	adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
	adc_base_config(ADC3, &adc_base_struct);
	
	adc_preempt_channel_length_set(ADC3, 1);
	adc_preempt_channel_set(ADC3,
		chPreemptSettings[IC_CH].chNum,
		chPreemptSettings[IC_CH].chSequence+1,
		chPreemptSettings[IC_CH].chAdcSample);
	
	adc_preempt_conversion_trigger_set(ADC3,
		ADC3_PREEMPT_TRIG_TMR1TRGOUT,
		TRUE);
	adc_preempt_auto_mode_enable(ADC3, FALSE);
	
	/*enable ADC*/
	adc_enable(ADC1, TRUE);
	adc_enable(ADC2, TRUE);
	adc_enable(ADC3, TRUE);
	/*Wait ADC stab pwr time*/
	delay_ms(50);
	/*Calibration procedure for ADC*/
	adc_calibration_init(ADC1);
	while (adc_calibration_init_status_get(ADC1)) ;
	adc_calibration_start(ADC1);
	while (adc_calibration_status_get(ADC1)) ;
	
	adc_calibration_init(ADC2);
	while (adc_calibration_init_status_get(ADC2)) ;
	adc_calibration_start(ADC2);
	while (adc_calibration_status_get(ADC2)) ;
	
	adc_calibration_init(ADC3);
	while (adc_calibration_init_status_get(ADC3)) ;
	adc_calibration_start(ADC3);
	while (adc_calibration_status_get(ADC3)) ;

	if (isrEna)
	{
		adc_interrupt_enable(ADC1, ADC_PCCE_INT, FALSE);
		adc_interrupt_enable(ADC2, ADC_PCCE_INT, FALSE);
		adc_interrupt_enable(ADC3, ADC_PCCE_INT, FALSE);
		
	}
}
static uint16_t adc_preempt_conversion_data_get_local(adc_type *adc_x, adc_preempt_channel_type adc_preempt_channel)
{
	uint16_t preempt_conv_data_index = 0;
	switch (adc_preempt_channel)
	{
	case ADC_PREEMPT_CHANNEL_1:
		preempt_conv_data_index = (uint16_t)(adc_x->pdt1_bit.pdt1);
		break;
	case ADC_PREEMPT_CHANNEL_2:
		preempt_conv_data_index = (uint16_t)(adc_x->pdt2_bit.pdt2);
		break;
	case ADC_PREEMPT_CHANNEL_3:
		preempt_conv_data_index = (uint16_t)(adc_x->pdt3_bit.pdt3);
		break;
	case ADC_PREEMPT_CHANNEL_4:
		preempt_conv_data_index = (uint16_t)(adc_x->pdt4_bit.pdt4);
		break;
	default:
		break;
	}
	return preempt_conv_data_index;
}
static flag_status adc_flag_get_local(adc_type *adc_x, uint8_t adc_flag)
{
	flag_status status = RESET;

	if ((adc_x->sts & adc_flag) == RESET)
	{
		status = RESET;
	}
	else
	{
		status = SET;
	}
	return status;
}
static void adc_flag_clear_local(adc_type *adc_x, uint32_t adc_flag)
{
	adc_x->sts = ~adc_flag;
}
static uint8_t getAdcTrigState(void)
{
	uint8_t state = (adc_flag_get_local(ADC1, ADC_PCCE_FLAG) != RESET) & (adc_flag_get_local(ADC2, ADC_PCCE_FLAG) != RESET) & (adc_flag_get_local(ADC3, ADC_PCCE_FLAG) != RESET);
	return (state);
}
__IO uint32_t adc_preempt_value[MAX_PREEMPT_CH];

/**
  * @brief  this function handles ADC3 handler.
  * @param  none
  * @retval none
  */
void ADC1_2_IRQHandler(void)
{
	/*wait all ADC*/
	if (adc_flag_get_local(ADC1, ADC_PCCE_FLAG) == SET)
	{
		adc1OK = 1;
		/*read data*/
		adc_preempt_value[IA_CH] = adc_preempt_conversion_data_get_local(
		ADC1,
			chPreemptSettings[IB_CH].chSequence);
		
		adc_preempt_value[UDC_CH] = adc_preempt_conversion_data_get_local(
		ADC1,
			chPreemptSettings[UDC_CH].chSequence);
		
		//adc_flag_clear_local(ADC1, ADC_PCCE_FLAG);
	}

	if (adc_flag_get_local(ADC2, ADC_PCCE_FLAG) == SET)
	{
		adc2OK = 1;
		/*read data*/
		adc_preempt_value[IB_CH] = adc_preempt_conversion_data_get_local(
		ADC2,
			chPreemptSettings[IB_CH].chSequence);
		
		adc_preempt_value[RES_SIN_CH] = adc_preempt_conversion_data_get_local(
		ADC2,
			chPreemptSettings[RES_SIN_CH].chSequence);
		//adc_flag_clear_local(ADC2, ADC_PCCE_FLAG);
	}

	if (pAdclocalCallBack != NULL)	pAdclocalCallBack();

}
/**
  * @brief  this function handles ADC3 handler.
  * @param  none
  * @retval none
  */
void ADC3_IRQHandler(void)
{
	/* add user code begin ADC3_IRQ 0 */
	if (adc_flag_get_local(ADC3, ADC_PCCE_FLAG) == SET)
	{
		adc3OK = 1;
		/*read data*/
		adc_preempt_value[IC_CH] = adc_preempt_conversion_data_get_local(
		ADC3,
			chPreemptSettings[IC_CH].chSequence);

		adc_preempt_value[RES_COS_CH] = adc_preempt_conversion_data_get_local(
		ADC3,
			chPreemptSettings[RES_COS_CH].chSequence);
		//adc_flag_clear_local(ADC3, ADC_PCCE_FLAG);
		
	}
	/* add user code end ADC3_IRQ 0 */
	/* add user code begin ADC3_IRQ 1 */

	/* add user code end ADC3_IRQ 1 */
}
uint16_t getDataReadyState(void)
{
	uint16_t res = 0;
	if (getAdcTrigState())
	{
		/*read data*/
		adc_preempt_value[IA_CH] = adc_preempt_conversion_data_get_local(
		ADC1,
			chPreemptSettings[IA_CH].chSequence);
		
		adc_preempt_value[UDC_CH] = adc_preempt_conversion_data_get_local(
		ADC1,
			chPreemptSettings[UDC_CH].chSequence);
		
		adc_preempt_value[RES_SIN_CH] = adc_preempt_conversion_data_get_local(
		ADC1,
			chPreemptSettings[RES_SIN_CH].chSequence);
		/*read data*/
		adc_preempt_value[IB_CH] = adc_preempt_conversion_data_get_local(
		ADC2,
			chPreemptSettings[IB_CH].chSequence);
		adc_preempt_value[RES_COS_CH] = adc_preempt_conversion_data_get_local(
		ADC2,
			chPreemptSettings[RES_COS_CH].chSequence);
		
		/*read data*/
		adc_preempt_value[IC_CH] = adc_preempt_conversion_data_get_local(
		ADC3,
			chPreemptSettings[IC_CH].chSequence);

		
		
		adc_flag_clear_local(ADC1, ADC_PCCE_FLAG);
		adc_flag_clear_local(ADC2, ADC_PCCE_FLAG);
		adc_flag_clear_local(ADC3, ADC_PCCE_FLAG);
		
		res = 1;
		
	}
	//return (adc1OK*adc2OK*adc3OK);
	return res;
	
}
void clearDataReadyState(void)
{
	adc1OK = adc2OK = adc3OK = 0;
	adc_flag_clear_local(ADC1, ADC_PCCE_FLAG);
	adc_flag_clear_local(ADC2, ADC_PCCE_FLAG);
	adc_flag_clear_local(ADC3, ADC_PCCE_FLAG);
}
void getADCfastData(void)
{
	/*read data*/
	adc_preempt_value[IA_CH] = adc_preempt_conversion_data_get_local(
	ADC1,
		chPreemptSettings[IB_CH].chSequence);
		
	adc_preempt_value[UDC_CH] = adc_preempt_conversion_data_get_local(
	ADC1,
		chPreemptSettings[UDC_CH].chSequence);
	/*read data*/
	adc_preempt_value[IB_CH] = adc_preempt_conversion_data_get_local(
	ADC2,
		chPreemptSettings[IB_CH].chSequence);
		
	adc_preempt_value[RES_SIN_CH] = adc_preempt_conversion_data_get_local(
	ADC2,
		chPreemptSettings[RES_SIN_CH].chSequence);
	/*read data*/
	adc_preempt_value[IC_CH] = adc_preempt_conversion_data_get_local(
	ADC3,
		chPreemptSettings[IC_CH].chSequence);

	adc_preempt_value[RES_COS_CH] = adc_preempt_conversion_data_get_local(
	ADC3,
		chPreemptSettings[RES_COS_CH].chSequence);
}