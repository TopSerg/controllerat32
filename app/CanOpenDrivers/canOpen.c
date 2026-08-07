#include "canOpen_lib.h"
/* Printf function of CanOpen app */
#if 0
#define log_printf(macropar_message, ...) printf(macropar_message, ##__VA_ARGS__)
#else
#define log_printf(macropar_message, ...) 
#endif

/* default values for CO_CANopenInit() */
#define NMT_CONTROL \
            CO_NMT_STARTUP_TO_OPERATIONAL \
          | CO_NMT_ERR_ON_ERR_REG \
          | CO_ERR_REG_GENERIC_ERR \
          | CO_ERR_REG_COMMUNICATION
	
#define FIRST_HB_TIME 500
#define SDO_SRV_TIMEOUT_TIME 1000
#define SDO_CLI_TIMEOUT_TIME 500
#define SDO_CLI_BLOCK false
#define OD_STATUS_BITS NULL
	
CO_t *CO = NULL; /* CANopen object */
CO_storage_t storage;
extern long start_deviceSettings;
extern long start_deviceFactory;
#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
//CO_storage_t storage;
CO_storage_entry_t storageEntries[] = { 
{
	.addr = &OD_PERSIST_COMM,
	.len = sizeof(OD_PERSIST_COMM),
	.subIndexOD = 2,
	.attr = CO_storage_cmd | CO_storage_restore,
	.addrNV = &start_deviceSettings 
} 
};
uint8_t storageEntriesCount = sizeof(storageEntries) / sizeof(storageEntries[0]);
uint32_t storageInitError = 0;
#endif
void *CANptr = NULL; /* CAN module address */
uint16_t baudrate = 0;
uint16_t desiredNodeID = 0x1;
uint16_t activeNodeID = 0x1;

// Global variables
uint32_t time_old = 0, time_current = 0;
CO_ReturnError_t err;

/* This function will basically setup the CANopen node */
static int canopen_app_resetCommunication() {
	/* CANopen communication reset - initialize CANopen objects *******************/
	log_printf("CANopenNode - Reset communication...\n");

	/* Wait rt_thread. */
	CO->CANmodule->CANnormal = false;

	/* Enter CAN configuration. */
	CO_CANsetConfigurationMode((void*)NULL);
	CO_CANmodule_disable(CO->CANmodule);

	/* initialize CANopen */
	err = CO_CANinit(CO, NULL, 0); // Bitrate set during baremeral init phase
	if (err != CO_ERROR_NO) {
		log_printf("Error: CAN initialization failed: %d\n", err);
		return 1;
	}

	CO_LSS_address_t lssAddress = {
		.identity = {
		.vendorID = OD_PERSIST_COMM.x1018_identity.vendor_ID,
		.productCode = OD_PERSIST_COMM.x1018_identity.productCode,
		.revisionNumber = OD_PERSIST_COMM.x1018_identity.revisionNumber,
		.serialNumber = OD_PERSIST_COMM.x1018_identity.serialNumber 
	} 
	};
	
	baudrate = OD_PERSIST_COMM.x2200_netConfig.canBaud;
	desiredNodeID = OD_PERSIST_COMM.x2200_netConfig.nodeId;
	
	err = CO_LSSinit(CO, &lssAddress, (uint8_t*)&desiredNodeID, &baudrate);
	if (err != CO_ERROR_NO) {
		log_printf("Error: LSS slave initialization failed: %d\n", err);
		return 2;
	}

	activeNodeID = desiredNodeID;
	uint32_t errInfo = 0;

	err = CO_CANopenInit(CO,                   /* CANopen object */
		NULL,                 /* alternate NMT */
		NULL,                 /* alternate em */
		OD,                   /* Object dictionary */
		OD_STATUS_BITS,       /* Optional OD_statusBits */
		NMT_CONTROL,          /* CO_NMT_control_t */
		FIRST_HB_TIME,        /* firstHBTime_ms */
		SDO_SRV_TIMEOUT_TIME, /* SDOserverTimeoutTime_ms */
		SDO_CLI_TIMEOUT_TIME, /* SDOclientTimeoutTime_ms */
		SDO_CLI_BLOCK,        /* SDOclientBlockTransfer */
		activeNodeID,
		&errInfo);
	if (err != CO_ERROR_NO && err != CO_ERROR_NODE_ID_UNCONFIGURED_LSS) {
		if (err == CO_ERROR_OD_PARAMETERS) {
			log_printf("Error: Object Dictionary entry 0x%X\n", errInfo);
		}
		else {
			log_printf("Error: CANopen initialization failed: %d\n", err);
		}
		return 3;
	}

	err = CO_CANopenInitPDO(CO, CO->em, OD, activeNodeID, &errInfo);
	if (err != CO_ERROR_NO) {
		if (err == CO_ERROR_OD_PARAMETERS) {
			log_printf("Error: Object Dictionary entry 0x%X\n", errInfo);
		}
		else {
			log_printf("Error: PDO initialization failed: %d\n", err);
		}
		return 4;
	}

	/* Configure Timer interrupt function for execution every 1 millisecond */
	//HAL_TIM_Base_Start_IT(canopenNodeSTM32->timerHandle); //1ms interrupt

	/* Configure CAN transmit and receive interrupt */

	/* Configure CANopen callbacks, etc */
	if (!CO->nodeIdUnconfigured) {

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
		if (storageInitError != 0) {
			CO_errorReport(CO->em, CO_EM_NON_VOLATILE_MEMORY, CO_EMC_HARDWARE, storageInitError);
		}
#endif
	}
	else {
		log_printf("CANopenNode - Node-id not initialized\n");
	}

	/* start CAN */
	CO_CANsetNormalMode(CO->CANmodule);

	log_printf("CANopenNode - Running...\n");
	fflush(stdout);
	time_old = time_current = 0;
	return 0;
}
/* This function will basically setup the CANopen node */
int canopen_app_init() {

	// Keep a copy global reference of canOpenSTM32 Object
	//canopenNodeSTM32 = _canopenNodeSTM32;

	/* Allocate memory */
	CO_config_t* config_ptr = NULL;

	uint32_t heapMemoryUsed;
	CO = CO_new(config_ptr, &heapMemoryUsed);
	if (CO == NULL) {
		log_printf("Error: Can't allocate memory\n");
		return 1;
	}
	else {
		log_printf("Allocated %u bytes for CANopen objects\n", heapMemoryUsed);
	}

	//canopenNodeSTM32->canOpenStack = CO;

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
	err = CO_storageBlank_init(&storage,
		CO->CANmodule,
		OD_ENTRY_H1010_storeParameters,
		OD_ENTRY_H1011_restoreDefaultParameters,
		storageEntries,
		storageEntriesCount,
		&storageInitError);

	if (err != CO_ERROR_NO && err != CO_ERROR_DATA_CORRUPT) {
		log_printf("Error: Storage %d\n", storageInitError);
		return 2;
	}
#endif

	return canopen_app_resetCommunication();
	
}

void canopen_loop() {
	/* loop for normal program execution ******************************************/
	/* get time difference since last function call */
	if ((time_current - time_old) > 0) {
		// Make sure more than 1ms elapsed
	    /* CANopen process */
		CO_NMT_reset_cmd_t reset_status;
		uint32_t timeDifference_us = (time_current - time_old) * 1000;
		time_old = time_current;
		reset_status = CO_process(CO, false, timeDifference_us, NULL);
		//canopenNodeSTM32->outStatusLEDRed = CO_LED_RED(CO->LEDs, CO_LED_CANopen);
		//canopenNodeSTM32->outStatusLEDGreen = CO_LED_GREEN(CO->LEDs, CO_LED_CANopen);

		if (reset_status == CO_RESET_COMM) {
			/* delete objects from memory */
			//CO_CANsetConfigurationMode((void*)canopenNodeSTM32);
			//CO_delete(CO);
			//log_printf("CANopenNode Reset Communication request\n");
			//canopen_app_resetCommunication(); // Reset Communication routine
		}
		else if (reset_status == CO_RESET_APP) {
			log_printf("CANopenNode Device Reset\n");
			nvic_system_reset(); // Reset the STM32 Microcontroller
		}
	}
}


void canopen_1ms(void) {
	
	time_current++;
	
	CO_LOCK_OD(CO->CANmodule);
	
	if (!CO->nodeIdUnconfigured && CO->CANmodule->CANnormal) {
		bool_t syncWas = false;
		/* get time difference since last function call */
		uint32_t timeDifference_us = 1000; // 1ms second

#if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
		syncWas = CO_process_SYNC(CO, timeDifference_us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_RPDO_ENABLE
		CO_process_RPDO(CO, syncWas, timeDifference_us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_TPDO_ENABLE
		CO_process_TPDO(CO, syncWas, timeDifference_us, NULL);
#endif

		/* Further I/O or nonblocking application code may go here. */
	}
	CO_UNLOCK_OD(CO->CANmodule);
}


/*Interruprs and send functions*/
uint8_t canSend(uint16_t id, uint8_t dlc, uint8_t *data)
{
	uint8_t success = 0;
	/*
	 * RTR flag is part of identifier value
	 * hence it needs to be properly decoded
	 */

	static can_tx_message_type tx_message_struct;
	tx_message_struct.frame_type = CAN_TFT_DATA;
	tx_message_struct.id_type = CAN_ID_STANDARD;
	tx_message_struct.dlc = dlc;
	tx_message_struct.standard_id = id;

	/*make data*/
	memcpy(tx_message_struct.data,
		data,
		sizeof(tx_message_struct.data));
	if (can_message_transmit(CAN1, &tx_message_struct) !=
	    CAN_TX_STATUS_NO_EMPTY)
		success = 1;
    
	return (success);
}

uint8_t can_tx_interrupt(void)
{
	CO_CANinterruptTx(CO->CANmodule);
}

uint8_t canOpen_rx_interrupt(can_rx_message_type* mesg)
{
	CO_CANrxMsg_t rcvmsg;
	rcvmsg.ident = mesg->standard_id;
	rcvmsg.dlc = mesg->dlc;
	memcpy(rcvmsg.data, mesg->data, sizeof(rcvmsg.data));
	CO_CANinterruptRx(CO->CANmodule, &rcvmsg);
}