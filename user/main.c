/**
 **************************************************************************
 * @file     main.c
 * @brief    main program
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "at32f403a_407_int.h"
#include "at32f403a_407_crc.h"
#include <stdio.h>
#include <stdint.h>
/*Module includes*/
#include "nvicInit.h"
#include "pwmInit.h"
#include "tempSpiInit.h"
#include "resolverInit.h"
#include "timerInit.h"
#include "adcInit.h"
#include "canInit.h"
#include "can2Init.h"
#include "CommInterface.h"
#include "CompileTime.h"
#include "flash.h"
#include "gpioInit.h"
#include "canOpen_lib.h"
#include "canDBCtask.h"
#include "getBoardSettings.h"
#include "DbcDispatcher.h"

#include "build/McuTemperature1.h"
#include "build/McuTemperature2.h"
#include "build/McuStatus.h"
#include "build/VcuMCU02.h"
#include "build/VcuMCU01.h"
#include "build/McuVCU1.h"
#include "build/McuDeratingStatus.h"
#include "build/McuFailureCode.h"
#include "build/VcutoMCUCurrentCommand.h"
#include "build/McuCurrentVoltage.h"
#include "build/McuFluxParams.h"

#define PWM_FREQUENCY_HZ				10000
#define PWM_TIME						(1.f/PWM_FREQUENCY_HZ)
#define TIMER_FREQUENCY_HZ				1000
#define STATIC_ADC_REF

/*** FREERTOS ****/
#include "WorkTasks.h"
extern long start_deviceSettings;
extern long start_deviceFactory;
volatile SysTick_Type *pSysTick = SysTick;
/*Include control alg*/
#include "ControlSystem_v2.h"             /* Model header file */
SystemParams_st SystemParams;
SystemParams_st *SystemSetActual = &SystemParams;
#include "platform_math.h"

__attribute__((section(".firmware_crc"))) const uint32_t crc_firm				= 0xFAFBFCFD;
__attribute__((section(".tables_crc"))) const uint32_t crc_table				= 0xFAFBFCFD;
__attribute__((section(".dev_settings_crc"))) const uint32_t crc_dev_settings	= 0xFAFBFCFD;
#define FASTRUN __attribute__ ((long_call, section (".code_in_ram")))
#include "defaultStructures.h"

static dbc_can_rx_message_type* reformRxMessageType(can_rx_message_type* data)
{
	static dbc_can_rx_message_type out;
	out.message_id = data->standard_id;
	out.dlc = data->dlc;
	for (int i = 0; i < data->dlc; i++)
	{
		out.data[i] = data->data[i];
	}
	return &out;
}


static can_tx_message_type* reformTxMessageType(dbc_can_tx_message_type* data)
{
	static can_tx_message_type out;
	out.standard_id = data->message_id;
	out.dlc = data->dlc;
	out.id_type = CAN_ID_STANDARD;
	out.frame_type = CAN_TFT_DATA;
	for (int i = 0; i < data->dlc; i++)
	{
		out.data[i] = data->data[i];
	}
	return &out;
}


static void canRxCall(can_rx_message_type* recivedData)
{
	canOpen_rx_interrupt(recivedData);
	EnqueueReceivePacket(reformRxMessageType(recivedData));
	
}
static void canTxCall()
{
	dbc_transmitNextPacket(&SendDispatcher);
}

static uint8_t sendToDBCCAN1(dbc_can_tx_message_type *message)
{
	if (can_message_transmit(CAN1, reformTxMessageType(message)) != CAN_TX_STATUS_NO_EMPTY) return 1;
	return 0;
}
static uint8_t sendToCAN1(can_tx_message_type *message)
{
	if (can_message_transmit(CAN1, message) != CAN_TX_STATUS_NO_EMPTY) return 1;
	return 0;
}

float IaOffset = 0;
float IbOffset = 0;
float IcOffset = 0;
float resSinOffest = 0;
float resCosOffset = 0;
float actualVref = 0; /*Calculated value of ADC REF*/
float actualMCUTempr = 0; /*Calcuated value of Internal temperature*/
/*analog in data*/
#define THROTTLE_K1 (1.52f)
float throttleVoltage = 0;
float throttleNorm = 0;
float throttleZero = 0.85;
float throttleOne = 3.81;
#define THROTTLE_A  (1.f/(throttleOne-throttleZero))
#define THROTTLE_B  (-THROTTLE_A*throttleZero)

#define RT_K1  (200)
#define RT_K2	(-314)
float actualRT1 = 0; /*actaul tempearature of PCB board*/
float actualRT2 = 0; /*actual temperature of HeatSink*/
float actualIgbt1T = 0, actualIgbt2T = 0, actualIgbt3T = 0;
float actualRTM = 0; /*actual temperature of Motor*/
/**
 ** @brief function covert ADC tempeatures to Celcium
 ** @detatil function using polynomial aproximation 
 **		y = 1,6686E+01x3 - 6,4816E+01x2 + 1,2867E+02x - 1,8718E+01
 ***/
float k3 = 7.421E-09;
float k2 = -4.008E-05;
float k1 = 1.028E-01;
float a = -2.251E+01;
static void calcTigbt(void)
{
	actualIgbt1T = adc1_ordinary_valuetab[IGBT_1_CH];
	actualIgbt1T = k3*actualIgbt1T*actualIgbt1T*actualIgbt1T + k2*actualIgbt1T*actualIgbt1T + k1*actualIgbt1T + a;
	
	actualIgbt2T = adc1_ordinary_valuetab[IGBT_2_CH];
	actualIgbt2T = k3*actualIgbt2T*actualIgbt2T*actualIgbt2T + k2*actualIgbt2T*actualIgbt2T + k1*actualIgbt2T + a;
	
	actualIgbt3T = adc1_ordinary_valuetab[IGBT_3_CH];
	actualIgbt3T = k3*actualIgbt3T*actualIgbt3T*actualIgbt3T + k2*actualIgbt3T*actualIgbt3T + k1*actualIgbt3T + a;
}
/**
 ** @brief fuction convert ADC to Celcius
 ** @detail function using liner function to find T(Vin)
 **			y = 781,82x - 263,44
 **			adcBit to Vin scale is Vref(1.8853/20bit) = 1.79796e-6
 ***/
float adc_tmotor_scale = 1.79796e-6;
float k1_Tmotor = 781.82;
float k2_Tmotor = -263.44;
volatile uint32_t rawMotTempr = 0;
static void calcTmotor(void)
{
	rawMotTempr = readRawTempr();
	if ((rawMotTempr < 0xfffff) && (rawMotTempr > 200)) 
	{
		actualRTM = k1_Tmotor * rawMotTempr*adc_tmotor_scale + k2_Tmotor;
		if (actualRTM > 180) actualRTM = 180;
	}
}

WorkModeType workMode = CURRENT;
volatile uint32_t timer1msTicks = 0;
volatile uint16_t startUpDone = 0;
float timer1msTicksf = 0;
float TimIsrTime = 0;
uint16_t debug_tork;
float Rs;

#define NUM_POINTS    10
#define STEP_CALLS    100
#define NUM_REPEATS   1
#define OFFSET   0

uint8_T trqBenchmarkDone = 0;

uint32_T TrqCalls[NUM_POINTS];
float    TrqTimes[NUM_POINTS];

int point = 0;
int repeat = 0;
float32_t sum = 0;

/**
 * @brief  timer interrupt handler
 * @param  none
 * @retval none
 */
static void timerTickCall(void)
{
	static uint32_t startTick = 0;
	startTick = GET_ACTUAL_TIMECNT();
	timer1msTicks++;
	timer1msTicksf = timer1msTicks;
	
	if (!(timer1msTicks % 100))
	{
		at32_led_toggle(LED2_EXT);
		/*
		static can_tx_message_type tx_message_struct;
		tx_message_struct.frame_type = CAN_TFT_DATA;
		tx_message_struct.id_type = CAN_ID_STANDARD;
		tx_message_struct.dlc = 8;
		tx_message_struct.standard_id = 0x123;
		can_message_transmit(CAN2, &tx_message_struct); 
		*/
		//gpio_input_data_bit_read(GPIOB, GPIO_PINS_6) ? gpio_bits_reset(GPIOB, GPIO_PINS_6) : gpio_bits_set(GPIOB, GPIO_PINS_6);
		
	}
	
	/*Calculate motor temperature*/
	calcTmotor();
	/*calculate Internal signals */
#ifndef STATIC_ADC_REF
	actualVref = (ADC_VINT_REF * 4096.f)
			/ adc1_ordinary_valuetab[UINT_REF_CH];
#else
	actualVref = ADC_VREF;
#endif
	actualMCUTempr = (ADC_TEMP_BASE
			- adc1_ordinary_valuetab[TEMPR_INT_CH] * actualVref*ADC_RESOLUTION)
			/ ADC_TEMP_SLOPE + 25;
	/*calculate temperatures for IGBT*/
	calcTigbt();
	/*update DBC data*/
	cpT_McuVCU1_gstate->McuActualSpeed = Control.Wmechanical;
	cpT_McuVCU1_gstate->McuActualTorque = Control.motorTorque;
	cpT_McuVCU1_gstate->McuIsCurr = platform_sqrt(Control.Iq*Control.Iq + Control.Id*Control.Id);
	cpT_McuVCU1_gstate->McuUdcCurr = Control.UdcFiltered;
	/*Set Actual state */
	cpT_McuStatus_gstate->McuIsd = Control.Id;
	cpT_McuStatus_gstate->McuIsq = Control.Iq;
	cpT_McuStatus_gstate->McuOfsAl = Control.IdcFilt;
	cpT_McuCurrentVoltage_gstate->Id = Control.Id;
	cpT_McuCurrentVoltage_gstate->Iq = Control.Iq;
	cpT_McuCurrentVoltage_gstate->Ud = Control.Ud;
	cpT_McuCurrentVoltage_gstate->Uq = Control.Uq;
	(Control.stat.mod_Active) ? (cpT_McuStatus_gstate->McustGateDrv = McuStatusMCU_stGateDrvPWMrun) : (cpT_McuStatus_gstate->McustGateDrv = McuStatusMCU_stGateDrvFreeWheel);
	(Control.stat.mod_Active) ? (cpT_McuStatus_gstate->McuVCUWorkMode = McuStatusMCU_VCUWorkModeTorqueControl) : (cpT_McuStatus_gstate->McuVCUWorkMode = McuStatusMCU_VCUWorkModeStandby);
	/*Set Actual Derating bits*/
	cpT_McuDeratingStatus_gstate->McubDernOvrUdc = Control.actLimits.udcLimitFlg;
	cpT_McuDeratingStatus_gstate->McubDernUndrUdc = Control.actLimits.udcLimitFlg;
	cpT_McuDeratingStatus_gstate->McubDernTempIGBT = Control.actLimits.igbtTlimFlg;
	cpT_McuDeratingStatus_gstate->McubDernN = Control.actLimits.speedLimFlg;
	cpT_McuDeratingStatus_gstate->McubDernOvrIdc = Control.actLimits.idcLimitFlg;
	cpT_McuDeratingStatus_gstate->McubDernUndrIdc = Control.actLimits.idcLimitFlg;
	cpT_McuDeratingStatus_gstate->McubDernStrTemp = Control.actLimits.motorTlimFlg;
	
	Rs = Control.motorParams.motorRs;
	float flux = Control.motorEmfCalc / Control.Welectrical;
	float Ld = (Control.motorParams.motorRs * Control.Id - Control.Ud) / (Control.Wmechanical * Control.Iq);
	float Lq = (- Control.motorParams.motorRs * Control.Iq + Control.Uq - flux) / (Control.Wmechanical * Control.Iq);
	
	/*cpT_McuFluxParams_gstate->Emf = Control.motorEmfCalc;
	cpT_McuFluxParams_gstate->Motorrs = Control.motorParams.motorRs;
	cpT_McuFluxParams_gstate->Welectrical = Control.Welectrical;
	cpT_McuFluxParams_gstate->Wmechanical = Control.Wmechanical;*/
	
	/* Set ERRORS*/
	if (Control.errors.GlobalError)
	{
		cpT_McuFailureCode_gstate->McuFailCode1 = McuFailureCodeMCU_FailCode1FaultLevel1;
		cpT_McuFailureCode_gstate->McuSoftwaeFaultBit |= Control.errors.OverCurrent << 1;
		cpT_McuFailureCode_gstate->McuSoftwaeFaultBit |= Control.errors.OverVoltage << 2;
		cpT_McuFailureCode_gstate->McuSoftwaeFaultBit |= Control.errors.OverTemperature_motor << 7;
		cpT_McuFailureCode_gstate->McuSoftwaeFaultBit |= Control.errors.OverTemperature_power << 9;
		cpT_McuFailureCode_gstate->McuHardwareFault |= Control.errors.HardWareFault << 0;
	}
	else {
		cpT_McuFailureCode_gstate->McuFailCode1 = McuFailureCodeMCU_FailCode1NoFault;
		cpT_McuFailureCode_gstate->McuSoftwaeFaultBit = 0;
	}
	/*Check current offsets*/
	if ((platform_abs(IaOffset) > 90) || (platform_abs(IbOffset) > 90) || (platform_abs(IcOffset) > 90))
	{
		cpT_McuFailureCode_gstate->McuSensorFault = 1 << 1;
		
	}
	if (actualVref < 2.5f)
	{
		cpT_McuFailureCode_gstate->McuHardwareFault = 1 << 5;
	}
	
	cpT_McuTemperature1_gstate->McuIGBTTempU = inSignals.Tigbt1;
	cpT_McuTemperature1_gstate->McuIGBTTempV = inSignals.Tigbt2;
	cpT_McuTemperature1_gstate->McuIGBTTempW = inSignals.Tigbt3;
	
	cpT_McuTemperature2_gstate->McuTempCurrCool = inSignals.TheatSink;
	cpT_McuTemperature2_gstate->McuTempCurrStr = inSignals.Tmotor;
	
	bool isOverspeedFault = false;
	
	if (Control.Wmechanical > 6000*20*PI)
	{
		isOverspeedFault = true;
	}
	
	if (Control.Wmechanical <= 0)
	{
		isOverspeedFault = false;
	}
	
	TestRefSignals.testActive = cpT_VcutoMCUCurrentCommand_gstate->VcuCurrentCommandEnable;
	
	if (TestRefSignals.testActive && !isOverspeedFault)
	{
		TestRefSignals.IdTest = cpT_VcutoMCUCurrentCommand_gstate->VcuIdCommand;
		TestRefSignals.IqTest = cpT_VcutoMCUCurrentCommand_gstate->VcuIqCommand;
	}

	extRef.cmd = cpT_VcuMCU01_gstate->VcuMCURequestedState + (workMode << 4);
	extRef.limitHigh = cpT_VcuMCU02_gstate->VcuMaxTorqueLimit;
	extRef.limitLow = cpT_VcuMCU02_gstate->VcuMinTorqueLimit;
	
	debug_tork = cpT_VcuMCU01_gstate->VcuMCUDesiredTorque;
	
	if (cpT_VcuMCU01_gstate->VcuMCURequestedState == VcuMCU01VCU_MCURequestedStateTrqCtr && !isOverspeedFault)
	{
		SystemParameters.Rate_Down = cpT_VcuMCU02_gstate->VcuMaxTorqueGradient * 0.5f;	
		SystemParameters.Rate_Up = cpT_VcuMCU02_gstate->VcuMaxTorqueGradient;	
		extRef.refValue = cpT_VcuMCU01_gstate->VcuMCUDesiredTorque;

	}
	else if (!isOverspeedFault)/*speed Mode*/
	{
		SystemParameters.Rate_Down = (cpT_VcuMCU02_gstate->VcuMaxTorqueGradient > 150) ? 150 : cpT_VcuMCU02_gstate->VcuMaxTorqueGradient;
		SystemParameters.Rate_Up = (cpT_VcuMCU02_gstate->VcuMaxTorqueGradient > 150) ? 150 : cpT_VcuMCU02_gstate->VcuMaxTorqueGradient;
		extRef.refValue = cpT_VcuMCU01_gstate->VcuMCUDesiredTorque * 1.65f;
	}
	else
	{
		extRef.cmd = 0 + (workMode << 4);
		extRef.refValue = 0;
	}
	/* CAN communication control*/
	
	
	
#if 0
	if (ReceiveDispatcher.loopCounter > 300)
	{
		extRef.cmd = 0 + (workMode << 4); //Тут вместо 0 была какая-то константа равная 0, особого функционала я не понял, но оставил
		extRef.refValue = 0;
	}
#endif
	
	/*Used like Enable input*/
#if 0
	if (getCrashState())
	{
		extRef.refValue  = 0;
		extRef.limitHigh = 0;
		extRef.limitLow = 0;
			
	}
#endif
	/* Call to exported function */
	isrTIM();
	
	TimIsrTime = CONVERT_toUs((startTick - GET_ACTUAL_TIMECNT())) * 1e6;
	
#if 0
	DW_workModeChoise localDW = { 0 };
	point++;
	repeat++;
	int poin = point / 500;
	if (poin < NUM_POINTS)
	{
		int numCalls = (poin + 1) * STEP_CALLS + OFFSET;
		float sumTime = 0.0F;

		TrqCalls[poin] = (uint32_T)numCalls;
		
		startTick = GET_ACTUAL_TIMECNT();

		for (int rep = 0; rep < NUM_REPEATS; rep++)
		{

			for (int i = 0; i < numCalls; i++)
			{
				TrqCalc(Control.Id,
					Control.Iq,
					Control.ImaxRef,
					Control.motorParams.motorEmf,
					Control.motorParams.motorPoles,
					Control.motorParams.motorLq,
					Control.motorParams.motorLd,
					Control.Tmotor,
					&localDW.TrqCalc_m);
			}

		}
		if (repeat % 100 == 0)
		{
			sum += CONVERT_toUs(startTick - GET_ACTUAL_TIMECNT()) * 1e6;
		}
		if (repeat % 500 == 0)
		{
			sum = 0;
		}
		
		sumTime = sum/5;

		TrqTimes[poin] = sumTime;
	}
	else if (point  > 15000)
	{
		memset(TrqTimes, 0, sizeof(TrqTimes));
		point = 0;
		repeat = 0;
	}
#endif
}


float AdcIsrTime = 0;
uint32_t adcIsrtCnt = 0;
/**
 * @brief  adc interrupt handler
 * @param  none
 * @retval none
 */
PWM_st testPWM = { 0 };
#define SMALL_STEND 0

#if SMALL_STEND
#define UDC_CH_K1 (0.1887f)
#define UDC_CH_K2 (+7.84f)
#else
#define UDC_CH_K1 (0.1866f)
#define UDC_CH_K2 (-2.798f)
#endif
/*calculated usin Schematic*/
#define I_CH_K1 (0.5496f)
#define I_CH_ZEROV (-1125.1f)
boardAnalogSet_st boardAnalog = { UDC_CH_K1, UDC_CH_K2, I_CH_K1, I_CH_ZEROV, 0x0, 0x0 };

#define UID_ADDR_LOW 0x1FFFF7E8
#define UID_ADDR_HIGH 0x1FFFF7EC
float delta = 0;

/*sound test*/
float A = 0;
float F = 0;
#include "wave.h"
//const float angle = 0;
volatile uint16_t baseCval = 11;

void adcCall(void)
{
	//at32_led_off(LED1);
	adcIsrtCnt++;
	static uint16_t resActiv = 0;
	if (resActiv)
	{
		tmr_counter_enable(TMR8, TRUE);
		tmr_output_enable(TMR8, TRUE);
		
	}
	tmr_counter_value_set(TMR8, baseCval);	
	//tmr_counter_enable(TMR8, TRUE);
	static volatile  uint32_t startTick = 0, stopTick = 0;
	startTick = GET_ACTUAL_TIMECNT();

	/*Do some work befor ADC ready*/
	inSignals.TheatSink = actualRT1;
	inSignals.Tigbt1 = actualIgbt1T;
	inSignals.Tigbt2 = actualIgbt2T;
	inSignals.Tigbt3 = actualIgbt3T;
	inSignals.Tmotor = actualRTM;
	
	inSignals.Fault = pwmGetActiveBrkState();
	
	/*update actual PRD VALUE */
	//SVPWM.PRD_VAL = pwmGetPrdVal();		
	uint16_t actualPwmPRD = pwmGetPrdVal();
	
	if (!pwmGetActiveState())
	{
		IaOffset += inSignals.Ia * PWM_TIME * 10;
		IbOffset += inSignals.Ib * PWM_TIME * 10;
		IcOffset += inSignals.Ic * PWM_TIME * 10;
	}
	if (!resActiv)
	{
		resSinOffest += inSignals.ResolverSine * PWM_TIME * 10;
		resCosOffset += inSignals.ResolverCosine * PWM_TIME * 10;
		if (adcIsrtCnt > 10000) resActiv = 1;
	}
	/*wait adc ready */
	while (!getDataReadyState())
	{
		
	}
	
	
	/* calc something */
	inSignals.Vdc = adc_preempt_value[UDC_CH] * boardAnalog.Udc_CH_K1 + boardAnalog.Udc_CH_K2;
	inSignals.Ia  = (adc_preempt_value[IA_CH] * boardAnalog.Iph_CH_K1 + boardAnalog.Iph_CH_ZEROV) - IaOffset;
	inSignals.Ib  = (adc_preempt_value[IB_CH] * boardAnalog.Iph_CH_K1 + boardAnalog.Iph_CH_ZEROV) - IbOffset;
	inSignals.Ic  = (adc_preempt_value[IC_CH] * boardAnalog.Iph_CH_K1 + boardAnalog.Iph_CH_ZEROV) - IcOffset;
#if 1
	inSignals.ResolverCosine = ((int16_t) adc_preempt_value[RES_COS_CH] - 2048) - resCosOffset;
	inSignals.ResolverSine = ((int16_t) adc_preempt_value[RES_SIN_CH] - 2048) - resSinOffest;
#else
	inSignals.ResolverCosine = (int16_t) adc_preempt_value[RES_SIN_CH] - resSinOffest;
	inSignals.ResolverSine = (int16_t) adc_preempt_value[RES_COS_CH] - resSinOffest;
#endif

	/* Call to exported function */
	isrADC();
#if 0
	static uint32_t i = 0;
	VdPlay = waveData[i++];
	if (i > waveLen) i = 0;
#endif
	
	
	if (outSignals.enable)
	{
		pwmRefVal(outSignals.refPwmA*actualPwmPRD, outSignals.refPwmB *actualPwmPRD, outSignals.refPwmC*actualPwmPRD);
		pwmEnable();
		setFaultPinState(FALSE);
	}
	else
	{
		pwmRefVal(outSignals.refPwmA*actualPwmPRD, outSignals.refPwmB*actualPwmPRD, outSignals.refPwmC*actualPwmPRD);
		pwmDisable();
		setFaultPinState(FALSE);
	}
	if (outSignals.clrFault)
	{
		/*clear BRK flag and HardWare fault pin */
		pwmClearBRKFlag();
		/*reset Driver fault PIN*/
		setFaultPinState(TRUE);
	}
	stopTick = GET_ACTUAL_TIMECNT();
	AdcIsrTime = CONVERT_toUs((startTick - stopTick)) * 1e6;
	//at32_led_on(LED1);
}
/**
 * @brief  Pwm interrupt handler
 * @param  none
 * @retval none
 */
static void pwmCall(void)
{
	pwmIsrCnt++;
	at32_led_toggle(LED3);
	/*Time for other calculations*/
	
	at32_led_on(LED1);
	
	/*call adc Process function */
	adcCall();
	
	at32_led_off(LED1);

}
uint32_t AT_device_id;  

real32_T debug = 0;
/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void)
{

	(void*)crc_firm; 
	(void*)crc_table; 
	(void*)crc_dev_settings;
	
	const int iq_count = 50;
	const int id_count = 50;
	
	real32_T a1 = -3.085404536910588e-5F;
	real32_T b1 = 0.05724472636066932F;
	real32_T a2 = -2.3702624307921933e-13F;
	real32_T b2 = 3.6328865609212724e-7F;
	real32_T c2 = -2.4256518605057296e-4F;

	for (int iq_idx = 0; iq_idx < iq_count; iq_idx++)
	{
		real32_T iq = (real32_T)LookUpData.IqVect[iq_idx];

		for (int id_idx = 0; id_idx < id_count; id_idx++)
		{
			real32_T id = (real32_T)LookUpData.IdVect[id_idx];

			real32_T Iq2 = iq * iq;
			real32_T Iq4 = Iq2 * Iq2;

			real32_T torque_poly =
			    ((a1 * Iq2) + (b1 * iq));

			real32_T torque_id_part =
			    (((a2 * Iq4) +
			      (b2  * Iq2) +
			      (c2 * iq)) * id);

			real32_T Torque_calc = 58.59F * (torque_poly + torque_id_part);
			debug = Torque_calc;

			real32_T LdLq_Tables = 0.0F;

			if ((id != 0.0F) && (iq != 0.0F))
			{
				LdLq_Tables =
				    2.0F * Torque_calc /
				    (3.0F * 4 * id * iq)
				    - Control.motorEmfCalc / id;
			}
			
			debug = LdLq_Tables;

			LookUpDataGenerate.IdVect[id_idx] = id;
			LookUpDataGenerate.IqVect[iq_idx] = iq;
			LookUpDataGenerate.Ldmatr[iq_idx * iq_count + id_idx] = LdLq_Tables;
			LookUpDataGenerate.Lqmatr[iq_idx * iq_count + id_idx] = 0;
		}
	}
	
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x4000);
	compileTimeInit();
	system_clock_config();
	/* get actual clocks */
	crm_clocks_freq_type clocks_freq;
	crm_clocks_freq_get(&clocks_freq);
	at32_board_init();
	/*Disable global IRQ*/
	debug_periph_mode_set(DEBUG_TMR8_PAUSE, FALSE);
	debug_periph_mode_set(DEBUG_TMR1_PAUSE, TRUE);
#if 0
	uart_print_init(115200);
#endif
	gpioInit();
	motorTempGpioUp(false);

	/*Power ON sequence*/
	gpio_bits_set(PWR_ON_GPIO_PORT, PWR_ON_GPIO_PIN);
	/*Wait untill power UP*/
	delay_ms(500);
	
	init3phPWM(PWM_FREQUENCY_HZ, TRUE);
	setPwmCallBack(pwmCall);

	timerInit(TIMER_FREQUENCY_HZ, TRUE);
	setTimerCallBack(timerTickCall);

	timerProfilerinit();

	/*Init resolver TMR*/
	initResolver();
	
	adcInit(TRUE);
	//setAdcCallBack(adcCall);

	//initCommInterface();

	dbc_init(&ReceiveDispatcher, &SendDispatcher, &sendToDBCCAN1);
	
	canInit(CAN_BAUD_1000k, TRUE);
	setCanRxCallBack(canRxCall);
	setCanTxCallBack(canTxCall);
	
	can2Init(CAN2_BAUD_1000k, TRUE);
	setCan2RxCallBack(canRxCall);
	
	motorTemprInit();
	/*Enable SysTick timer*/
	SysTick->LOAD = 0xFFFFFF;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	/*read UID*/
	uint32_t uid[3] = { 0 };
	memcpy(&uid[0], (const void*)UID_ADDR_LOW, sizeof(uid));
	crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);
	crc_init_data_set(0xFFFFFFFF);
	crc_reverse_input_data_set(CRC_REVERSE_INPUT_NO_AFFECTE);
	crc_reverse_output_data_set(CRC_REVERSE_OUTPUT_NO_AFFECTE);
	crc_data_reset();
	/*make OWN uid with 32 bits*/
	AT_device_id = crc_block_calculate(&uid[0], 3);
	
	boardAnalog.boardCPU_ID_low = AT_device_id;
	boardAnalog.boardCPU_ID_High = *((uint32_t*)UID_ADDR_HIGH);
	if (!getBoardSettings(&boardAnalog)) cpT_McuFailureCode_gstate->McuHardwareFault = 1;
	
#if SMALL_STEND
	/*Update defaut settings to New*/
	SystemParams = ConstP_d.pooled2;
	SystemParams.PwmBaseFrq = PWM_FREQUENCY_HZ;
	SystemParams.LagCorrection = 1.0;
	SystemParams.MotorParams.motorLd = 47;
	SystemParams.MotorParams.motorLq = 47;
	SystemParams.StaticInductionFlg = true;
	SystemParams.MotorParams.motorRs = 0.02;
	SystemParams.MotorParams.motorPoles = 5;
	SystemParams.MotorParams.motorEmf = 0.053*SystemParams.MotorParams.motorPoles;
	SystemParams.ResolverPoles = 5;
	SystemParams.BW_reg_Inv = 4000;
	SystemParams.SpeedKp = 0.7;
	SystemParams.SpeedKi = 0.005;
	SystemParams.Rate_Down = 1500;
	SystemParams.Rate_Up = 1500;
	/*Для стендовой спарки - право */
	baseCval = boardAnalog.resolverBase; // Корректировка выборки для резольвера
	SystemParams.correctionTheta = boardAnalog.resolverShift;
	SystemParams.ResolverSignalSwap = boardAnalog.resolverSwap;
	
	/*FunctionalLimit settings*/
	SystemParams.MaxCurrent = 80;
	SystemParams.MinIdCurrent = 60;
	SystemParams.MaxCharge = 10;
	SystemParams.MaxDischarge = 10;
	SystemParams.FuncLimits.Udc_low = 40;
	SystemParams.FuncLimits.Udc_low_diap = 10;
	SystemParams.FuncLimits.Udc_high = 80;
	SystemParams.FuncLimits.Udc_high_diap = 5;
	SystemParams.FuncLimits.Speed_fwd = 6700;
	SystemParams.FuncLimits.Speed_fwd_diap = 500;
	SystemParams.FuncLimits.Speed_rev = 800;
	SystemParams.FuncLimits.Speed_rev_diap = 500;
	
	/*						  1  2  3  4  5  6		*/
	uint16_t const hallArray[6] = { 1, 3, 2, 5, 6, 4 };
	
	//uint16_t hallArray[6] = { 3, 1, 2, 5, 4, 6 };
	
	SystemParams.phase_inverse = 0;
	
	SystemParams.hallTableDirect[0] = hallArray[0];
	SystemParams.hallTableDirect[1] = hallArray[1];
	SystemParams.hallTableDirect[2] = hallArray[2];
	SystemParams.hallTableDirect[3] = hallArray[3];
	SystemParams.hallTableDirect[4] = hallArray[4];
	SystemParams.hallTableDirect[5] = hallArray[5];
	
	SystemParams.hallTableInverse[0] = hallArray[1];
	SystemParams.hallTableInverse[1] = hallArray[0];
	SystemParams.hallTableInverse[2] = hallArray[2];
	SystemParams.hallTableInverse[3] = hallArray[3];
	SystemParams.hallTableInverse[4] = hallArray[5];
	SystemParams.hallTableInverse[5] = hallArray[4];
	
	

	SystemParams.ActivePosSensor = ResolverType;
	SystemParams.Observer_LO = 0.985;
	
	TripLevels.OverCurrent_level = 110;
	TripLevels.OverVoltage_level = 90;
	
#else
	/*Update defaut settings to New*/
	SystemParams = ConstP_d.pooled2;
	SystemParams.PwmBaseFrq = PWM_FREQUENCY_HZ;
	SystemParams.LagCorrection = 1.0;
	SystemParams.MotorParams.motorLd = 47;
	SystemParams.MotorParams.motorLq = 47;
	SystemParams.StaticInductionFlg = false;
	SystemParams.MotorParams.motorRs = 0.015;
	SystemParams.MotorParams.motorPoles = 4;
	SystemParams.MotorParams.motorEmf = 0.095*SystemParams.MotorParams.motorPoles;
	SystemParams.ResolverPoles = 2;
	SystemParams.BW_reg_Inv = 6700;
	SystemParams.SpeedKp = 1.9;
	SystemParams.SpeedKi = 0.005;
	SystemParams.Rate_Down = 1500;
	SystemParams.Rate_Up = 1500;
	/*Для стендовой спарки - право */
	baseCval = boardAnalog.resolverBase; // Корректировка выборки для резольвера
	SystemParams.correctionTheta = boardAnalog.resolverShift;
	SystemParams.ResolverSignalSwap = boardAnalog.resolverSwap;
	/*FunctionalLimit settings*/
	SystemParams.MaxCurrent = 650;
	SystemParams.MinIdCurrent = 540;
	SystemParams.MaxCharge = 400;
	SystemParams.MaxDischarge = 400;
	SystemParams.FuncLimits.Udc_low = 100;
	SystemParams.FuncLimits.Udc_low_diap = 20;
	SystemParams.FuncLimits.Udc_high = 350;
	SystemParams.FuncLimits.Udc_high_diap = 20;
	SystemParams.FuncLimits.Speed_fwd = 6700;
	SystemParams.FuncLimits.Speed_fwd_diap = 500;
	SystemParams.FuncLimits.Speed_rev = 6500;
	SystemParams.FuncLimits.Speed_rev_diap = 500;
	SystemParams.FuncLimits.Motor_Thigh = 130;
	SystemParams.FuncLimits.Motor_Thigh_diap = 20;
	/*						  1  2  3  4  5  6		*/
	uint16_t const hallArray[6] = { 1, 3, 2, 5, 6, 4 };
	
	//uint16_t hallArray[6] = { 3, 1, 2, 5, 4, 6 };
	
	SystemParams.phase_inverse = 0;
	
	SystemParams.hallTableDirect[0] = hallArray[0];
	SystemParams.hallTableDirect[1] = hallArray[1];
	SystemParams.hallTableDirect[2] = hallArray[2];
	SystemParams.hallTableDirect[3] = hallArray[3];
	SystemParams.hallTableDirect[4] = hallArray[4];
	SystemParams.hallTableDirect[5] = hallArray[5];
	
	SystemParams.hallTableInverse[0] = hallArray[1];
	SystemParams.hallTableInverse[1] = hallArray[0];
	SystemParams.hallTableInverse[2] = hallArray[2];
	SystemParams.hallTableInverse[3] = hallArray[3];
	SystemParams.hallTableInverse[4] = hallArray[5];
	SystemParams.hallTableInverse[5] = hallArray[4];
	
	
	

	SystemParams.ActivePosSensor = ResolverType;
	SystemParams.Observer_LO = 0.985;
	
#endif
	/* Initialize model */
	ControlSystem_v2_initialize();
	

	TripLevels.OverCurrent_level = 750;
	TripLevels.OverVoltage_level = 390;
	TripLevels.OverTemp_power_level = 100;
	TripLevels.OverTemp_motor_level = 180;

	/* enable tmr8 */
	//tmr_counter_enable(TMR8, TRUE);
	//tmr_output_enable(TMR8, TRUE); 	
	pwmCounterEnable();
	
	
	adc_ordinary_software_trigger_enable(ADC1, TRUE);
	
#if ENABLE_FREERTOS
	/*Initialyze freertos tasks */
	initFreeRtos();
#else
	/*Enable global IRQ*/
	nvicInit();
	__enable_irq();
	while (1)
	{

		if ((pwmIsrCnt % 1600) == 1599)
		{
			//sendToUart(data, 6);

		}
		messageLoop();
	}
#endif
}

/**
 * @}
 */

/**
 * @}
 */
