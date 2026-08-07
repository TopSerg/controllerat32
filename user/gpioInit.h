#pragma once
#include "at32f403a_407_conf.h"
#include "at32f403a_407_int.h"
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"


#define CRASH_GPIO_PIN                  GPIO_PINS_3
#define CRASH_GPIO_PORT                 GPIOE
#define CRASH_GPIO_CRM_CLK              CRM_GPIOE_PERIPH_CLOCK 

#define HALL2_GPIO_PIN                  GPIO_PINS_7
#define HALL2_GPIO_PORT                 GPIOC
#define HALL2_GPIO_CRM_CLK              CRM_GPIOC_PERIPH_CLOCK

#define HALL3_GPIO_PIN                  GPIO_PINS_8
#define HALL3_GPIO_PORT                 GPIOC
#define HALL3_GPIO_CRM_CLK              CRM_GPIOC_PERIPH_CLOCK
 

/*Power domain signals*/
#define PWR_ON_GPIO_PIN                  GPIO_PINS_1
#define PWR_ON_GPIO_PORT                 GPIOD
#define PWR_ON_GPIO_CRM_CLK              CRM_GPIOD_PERIPH_CLOCK 


/*Driver fault pin*/
#define FAULT_CLEAR_GPIO_PIN			GPIO_PINS_0
#define FAULE_CLEAR_GPIO_PORT			GPIOD
#define FAULT_CLEAR_GPIO_CRM_CLK        CRM_GPIOD_PERIPH_CLOCK 
void gpioInit(void);
void motorTempGpioUp(bool state);
uint16_t getCrashState(void);
void setFaultPinState(bool state);
