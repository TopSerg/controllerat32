#include "resolverInit.h"
#include "at32f403a_407_board.h"
#include <math.h>
#define RES_EXT_FRQ 10000u
/*DMA Sin generator*/
#define DMA_SINE 0
#define TIMER_FEQ 1000000u
#define SIN_ARRAY_SIZE 100
#define SIN_FREQ 10000
#define M_PI 3.1415926535f
#define M_2PI 6.283185307f
volatile uint16_t sinArray[SIN_ARRAY_SIZE];
static void initSineTable()
{
	crm_clocks_freq_type crm_clocks_freq_struct = { 0 };
	//read actual clock of MCU
	crm_clocks_freq_get(&crm_clocks_freq_struct);
	/*Generate sinTable*/
	int i;
	// pwmAmpl = 240 ~= 8bit
	int pwmAmpl = crm_clocks_freq_struct.sclk_freq / SIN_ARRAY_SIZE / SIN_FREQ;
	float phase = 0;
	// Generate sin table
	for (i = 0; i < SIN_ARRAY_SIZE; i++)
	{
		sinArray[i] = (sinf(M_2PI * i / SIN_ARRAY_SIZE + phase)*0.4f+0.5f) * pwmAmpl;
	}
}
static void dma1_channel1_init(void);
void initResolver(void)
{
	gpio_init_type gpio_init_struct = { 0 };
	/* enable tmr1/gpioa/gpiob clock */
	//enable TIM base clock
	
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	
	crm_periph_clock_enable(CRM_TMR8_PERIPH_CLOCK, TRUE);
	
	
	

	/* timer8 output pin configuration */
	gpio_init_struct.gpio_pins = GPIO_PINS_0;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init(GPIOB, &gpio_init_struct);

	
	/*Act like GPIO for resolver signal path activation*/
	gpio_init_struct.gpio_pins = GPIO_PINS_7;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init(GPIOC, &gpio_init_struct);
	
	
	crm_clocks_freq_type crm_clocks_freq_struct = { 0 };
	//read actual clock of MCU
	crm_clocks_freq_get(&crm_clocks_freq_struct);
	
	//set max count value to overflow use /2 because UP_DOWN count mode
	//set divide for sysClk to ZERO (real div 1)
#if DMA_SINE
	uint32_t arrValue = (crm_clocks_freq_struct.sclk_freq / TIMER_FEQ) >> 0;
	tmr_base_init(TMR8, arrValue - 1, 0);
	tmr_cnt_dir_set(TMR8, TMR_COUNT_UP);
	tmr_clock_source_div_set(TMR8, TMR_CLOCK_DIV1);
#else
	uint32_t arrValue = (crm_clocks_freq_struct.sclk_freq / RES_EXT_FRQ) >> 0;
	tmr_base_init(TMR8, arrValue - 1, 0);
	tmr_cnt_dir_set(TMR8, TMR_COUNT_UP);
	tmr_clock_source_div_set(TMR8, TMR_CLOCK_DIV1);
#endif
	
	

	tmr_output_config_type tmr_output_struct = { 0 };
	/* channel Configuration in output mode */
	tmr_output_default_para_init(&tmr_output_struct);
	tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
	tmr_output_struct.oc_output_state = TRUE;
	tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_output_struct.oc_idle_state = FALSE;
	tmr_output_struct.occ_output_state = TRUE;
	tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_output_struct.occ_idle_state = FALSE;
	
	/* channel 2 */
	uint32_t actualArrValue = tmr_period_value_get(TMR8);
	/* compute ccr1 value to generate a duty cycle at 50% for channel 1 and 1n */
	uint16_t channel_pulse = (uint16_t)(((uint32_t) actualArrValue >> 1));
	tmr_output_channel_config(TMR8, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
	tmr_output_channel_buffer_enable(TMR8, TMR_SELECT_CHANNEL_2, TRUE);
	
	tmr_channel_value_set(TMR8, TMR_SELECT_CHANNEL_2, channel_pulse);
	
	/* channel 4 */
	tmr_output_default_para_init(&tmr_output_struct);
	tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_HIGH;
	tmr_output_struct.oc_output_state = FALSE;
	tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_output_struct.oc_idle_state = FALSE;
	tmr_output_struct.occ_output_state = FALSE;
	tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_output_struct.occ_idle_state = FALSE;
	
	actualArrValue = tmr_period_value_get(TMR8);
	/* compute ccr1 value to generate a duty cycle at 50% for channel 1 and 1n */
	channel_pulse = (uint16_t)(((uint32_t) actualArrValue - 1));
	tmr_output_channel_config(TMR8, TMR_SELECT_CHANNEL_4, &tmr_output_struct);
	tmr_output_channel_buffer_enable(TMR8, TMR_SELECT_CHANNEL_4, TRUE);
	
	tmr_channel_value_set(TMR8, TMR_SELECT_CHANNEL_4, channel_pulse);
	
#if DMA_SINE	
	/* configure dma overflow */
	initSineTable();
	tmr_channel_dma_select(TMR8, TMR_DMA_REQUEST_BY_CHANNEL);
	tmr_dma_request_enable(TMR8, TMR_C4_DMA_REQUEST, TRUE);
	tmr_repetition_counter_set(TMR8, arrValue);
	dma1_channel1_init();
#endif
	//tmr_counter_enable(TMR8, TRUE);
	tmr_output_enable(TMR8, FALSE);
	
	/* overflow interrupt enable */
	tmr_interrupt_enable(TMR8, TMR_OVF_INT, FALSE);
	//tmr_counter_enable(TMR8, TRUE);
}

void TMR8_OVF_TMR13_IRQHandler(void)
{
	if (tmr_flag_get(TMR8, TMR_OVF_FLAG) != RESET)
	{
		/* add user code... */
		//if (plocalCallBack != NULL) plocalCallBack();
		tmr_flag_clear(TMR8, TMR_OVF_FLAG);
	}
}
/**
  * @brief  config dma channel transfer parameter
  * @param  none
  * @retval none
  */
static void dma_channel_config(dma_channel_type* dmax_channely, uint32_t peripheral_base_addr, uint32_t memory_base_addr, uint16_t buffer_size)
{
	dmax_channely->dtcnt = buffer_size;
	dmax_channely->paddr = peripheral_base_addr;
	dmax_channely->maddr = memory_base_addr;
}

/**
  * @brief  init dma1 channel1 for "tmr8_overflow"
  * @param  none
  * @retval none
  */
static void dma1_channel1_init(void)
{
	dma_init_type dma_init_struct;
	/* enable dma1 periph clock */
	crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);

	dma_reset(DMA2_CHANNEL2);
	dma_default_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
	dma_init_struct.memory_inc_enable = TRUE;
	dma_init_struct.peripheral_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
	dma_init_struct.peripheral_inc_enable = FALSE;
	dma_init_struct.priority = DMA_PRIORITY_VERY_HIGH;
	dma_init_struct.loop_mode_enable = TRUE;
	dma_init(DMA2_CHANNEL2, &dma_init_struct);
	
	/* flexible function enable */
	dma_flexible_config(DMA2, FLEX_CHANNEL2, DMA_FLEXIBLE_TMR8_CH4);
	/**
	 * Users need to configure DMA1 interrupt functions according to the actual application.
	 * 1. Call the below function to enable the corresponding DMA1 interrupt.
	 *     --dma_interrupt_enable(...)
	 * 2. Add the user's interrupt handler code into the below function in the at32f403a_407_int.c file.
	 *     --void DMA1_Channel1_IRQHandler(void)
	 */
	dma_channel_config(DMA2_CHANNEL2, (uint32_t)&TMR8->c2dt, (uint32_t)sinArray, SIN_ARRAY_SIZE);
	dma_channel_enable(DMA2_CHANNEL2, TRUE);
}
