/*
 * Tasks.c
 *
 *  Created on: 2023 Apr 13
 *      Author: on4ip
 */
#include "WorkTasks.h"

#include "at32f403a_407_board.h"
#include "nvicInit.h"
#include "CommInterface.h"
#include "canOpen_lib.h"
#include "canDBCtask.h"
TaskHandle_t led2_handler;
/* led2 task */
void led2_task_function(void *pvParameters);
/* canOpenTask*/
TaskHandle_t canOpen_handler;
TaskHandle_t canDBC_handler;

/* FreeRtos HOOK functions*/
uint16_t heapError = 0;
void vApplicationMallocFailedHook(void)
{
	heapError = 1;
}

void vApplicationDaemonTaskStartupHook(void)
{
	/*Enable global IRQ*/
	__disable_irq();
	nvicInit();
	__enable_irq();
}
/* IDLE loop low prio function */
void vApplicationIdleHook(void)
{
	//messageLoop();
	canopen_loop();
}


void initFreeRtos(void)
{
	/* enter critical */
#if ENABLE_FREERTOS
	taskENTER_CRITICAL();
	xTaskCreate((TaskFunction_t) led2_task_function,
		(const char*) "LED2_task",
		(uint16_t) 512,
		(void*) NULL,
		(UBaseType_t) 2,
		(TaskHandle_t*) &led2_handler);
	/* initialize CANopen */
#if 1
	if (!canopen_app_init())
	{
		

		xTaskCreate((TaskFunction_t) canopen_task,
			(const char*) "CANopen_task",
			(uint16_t) 512,
			(void*) NULL,
			(UBaseType_t) 2,
			(TaskHandle_t*) &canOpen_handler);

	}
	
	/*Make task for CAN dbc*/
	
	xTaskCreate((TaskFunction_t) canDBC_task,
		(const char*) "CanDBC_task",
		(uint16_t) 512,
		(void*) NULL,
		(UBaseType_t) 2,
		(TaskHandle_t*) &canDBC_handler);
#endif
	/* exit critical */
	taskEXIT_CRITICAL();

	/* start scheduler */
	vTaskStartScheduler();
#endif
}



/* led2 task function */
void led2_task_function(void *pvParameters)
{
	while (1)
	{
		//at32_led_toggle(LED3);
		vTaskDelay(1000);
	}
}


void canopen_task(void *argument)
{
	/* Infinite loop */
	for (;;) {
		canopen_1ms();
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	/* USER CODE END canopen_task */
}

void canDBC_task(void *argument)
{
	/* Infinite loop */
	for (;;) {
		canDBC_tick();
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	/* USER CODE END canopen_task */
}