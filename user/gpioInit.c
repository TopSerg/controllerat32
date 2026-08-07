#include "gpioInit.h"

void gpioInit(void)
{
	gpio_init_type gpio_init_struct;

	/* enable the button clock */
	crm_periph_clock_enable(CRASH_GPIO_CRM_CLK, TRUE);

	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure button pin as input with pull-up/pull-down */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = CRASH_GPIO_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init(CRASH_GPIO_PORT, &gpio_init_struct);
	
	/* enable the button clock */
	crm_periph_clock_enable(HALL2_GPIO_CRM_CLK, TRUE);

	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure button pin as input with pull-up/pull-down */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = HALL2_GPIO_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init(HALL2_GPIO_PORT, &gpio_init_struct);
	
	/* enable the button clock */
	crm_periph_clock_enable(HALL3_GPIO_CRM_CLK, TRUE);

	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure button pin as input with pull-up/pull-down */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = HALL3_GPIO_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init(HALL3_GPIO_PORT, &gpio_init_struct);
	
	/*Power Ref source activation PD1*/
	/* enable the button clock */
	crm_periph_clock_enable(PWR_ON_GPIO_CRM_CLK , TRUE);
	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure button pin as input with pull-up/pull-down */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_pins = PWR_ON_GPIO_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init(PWR_ON_GPIO_PORT, &gpio_init_struct);
	
	/*Set power to HIGH*/
	//gpio_bits_set(PWR_ON_GPIO_PORT, PWR_ON_GPIO_PIN);
	
	
	/*Fault Clear source activation PD0*/
	/* enable the button clock */
	crm_periph_clock_enable(FAULT_CLEAR_GPIO_CRM_CLK, TRUE);
	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure button pin as input with pull-up/pull-down */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_pins = FAULT_CLEAR_GPIO_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init(FAULE_CLEAR_GPIO_PORT, &gpio_init_struct);
}
__INLINE uint16_t getCrashState(void)
{
	return (!gpio_input_data_bit_read(CRASH_GPIO_PORT, CRASH_GPIO_PIN));
}

void motorTempGpioUp(bool state)
{
	gpio_init_type gpio_init_struct;

	/* enable the button clock */
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure button pin as input with pull-up/pull-down */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = GPIO_PINS_9;
	if (state) gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	else gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init(GPIOC, &gpio_init_struct);
}

void setFaultPinState(bool state)
{
	(state) ? (gpio_bits_set(FAULE_CLEAR_GPIO_PORT, FAULT_CLEAR_GPIO_PIN)) : gpio_bits_reset(FAULE_CLEAR_GPIO_PORT, FAULT_CLEAR_GPIO_PIN);
}