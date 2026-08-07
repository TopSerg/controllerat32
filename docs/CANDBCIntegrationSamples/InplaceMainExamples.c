// ...
# include "CAN\CANSendDispatcher.h"
# include "CAN\CANReceiveDispatcher.h"
#include "CAN\CANReceiveHandlers.h"
#include "CAN\CANSendHandlers.h"
#include "CAN\CANErrorsHandlers.h"
//...

//...
T_SEND_DISPATCHER SendDispatcher;
T_RECEIVE_DISPATCHER ReceiveDispatcher;
//...

//...
main()
{
//..
  crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);

  nvic_irq_enable(USBFS_H_CAN1_TX_IRQn, 0, 0);
  nvic_irq_enable(USBFS_L_CAN1_RX0_IRQn, 0, 0);
  nvic_irq_enable(CAN1_RX1_IRQn, 0, 0);
  nvic_irq_enable(CAN1_SE_IRQn, 0, 0);

  wk_can1_init();
  wk_crc_init();
//..
    InitSendDispatcher(&SendDispatcher);
    SendDispatcher.MCU_VCU_1_Callback = &Fill_MCU_VCU_1;
    SendDispatcher.MCU_Temperature1_Callback = &Fill_MCU_Temperature1;
    SendDispatcher.MCU_Temperature2_Callback = &Fill_MCU_Temperature2;
    SendDispatcher.MCU_Status_Callback = &Fill_MCU_Status;
    SendDispatcher.MCU_DeratingStatus_Callback = &Fill_MCU_DeratingStatus;
    SendDispatcher.MCU_FailureCode_Callback = &Fill_MCU_FailureCode;
    SendDispatcher.MCU_SoftwareNumber_Callback = &Fill_MCU_SoftwareNumber;
    SendDispatcher.MCU_HardwareNumber_Callback = &Fill_MCU_HardwareNumber;

    SendDispatcher.QueueOverflow_Callback = &SendQueueOverflowHandler;

    InitReceiveDispatcher(&ReceiveDispatcher);
    ReceiveDispatcher.VCU_MCU01_Callback = VCU_MCU01_Received;
    ReceiveDispatcher.VCU_MCU02_Callback = VCU_MCU02_Received;

    ReceiveDispatcher.QueueOverflow_Callback = &ReceiveQueueOverflowHandler;
    ReceiveDispatcher.CRCError_Callback = &CRCErrorHandler;
    ReceiveDispatcher.MessageCounter046WrongDelta_Callback = &MessageCounter046WrongDeltaHandler;
    ReceiveDispatcher.MessageCounter047WrongDelta_Callback = &MessageCounter047WrongDeltaHandler;
    ReceiveDispatcher.Message046Timeout_Callback = &Message046TimeoutHandler;
    ReceiveDispatcher.Message047Timeout_Callback = &Message047TimeoutHandler;

    can_interrupt_enable(CAN1, CAN_RF0MIEN_INT, TRUE);
    can_interrupt_enable(CAN1, CAN_TCIEN_INT, TRUE);
//..
}

//..
void wk_crc_init(void)
{
  /* add user code begin crc_init 0 */

  /* add user code end crc_init 0 */

  crc_init_data_set(0x00000000);
  crc_poly_size_set(CRC_POLY_SIZE_8B);
  crc_poly_value_set(0x1D);
  crc_reverse_input_data_set(CRC_REVERSE_INPUT_NO_AFFECTE);
  crc_reverse_output_data_set(CRC_REVERSE_OUTPUT_NO_AFFECTE);
  crc_data_reset();

  /* add user code begin crc_init 1 */

  /* add user code end crc_init 1 */
}

void wk_can1_init(void)
{
  /* add user code begin can1_init 0 */

  /* add user code end can1_init 0 */
  
  gpio_init_type gpio_init_struct;
  can_base_type can_base_struct;
  can_baudrate_type can_baudrate_struct;
  can_filter_init_type can_filter_init_struct;

  /* add user code begin can1_init 1 */

  /* add user code end can1_init 1 */
  
  /*gpio-----------------------------------------------------------------------------*/ 
  gpio_default_para_init(&gpio_init_struct);

  /* configure the CAN1 TX pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the CAN1 RX pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* GPIO PIN remap */
  gpio_pin_remap_config(CAN1_GMUX_0010, TRUE); 

  /*can_base_init--------------------------------------------------------------------*/ 
  can_default_para_init(&can_base_struct);
  can_base_struct.mode_selection = CAN_MODE_COMMUNICATE;
  can_base_struct.ttc_enable = FALSE;
  can_base_struct.aebo_enable = TRUE;
  can_base_struct.aed_enable = TRUE;
  can_base_struct.prsf_enable = FALSE;
  can_base_struct.mdrsel_selection = CAN_DISCARDING_FIRST_RECEIVED;
  can_base_struct.mmssr_selection = CAN_SENDING_BY_REQUEST;

  can_base_init(CAN1, &can_base_struct);

  /*can_baudrate_setting-------------------------------------------------------------*/ 
  /*set baudrate = pclk/(baudrate_div *(1 + bts1_size + bts2_size))------------------*/ 
  can_baudrate_struct.baudrate_div = 15;                       /*value: 1~0xFFF*/
  can_baudrate_struct.rsaw_size = CAN_RSAW_2TQ;                /*value: 1~4*/
  can_baudrate_struct.bts1_size = CAN_BTS1_5TQ;                /*value: 1~16*/
  can_baudrate_struct.bts2_size = CAN_BTS2_2TQ;                /*value: 1~8*/
  can_baudrate_set(CAN1, &can_baudrate_struct);

  /*can_filter_0_config--------------------------------------------------------------*/
  can_filter_init_struct.filter_activate_enable = TRUE;
  can_filter_init_struct.filter_number = 0;
  can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;
  can_filter_init_struct.filter_bit = CAN_FILTER_16BIT;  
  can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_LIST;
  /*Standard identifier + List Mode + Data/Remote frame: id/mask 11bit --------------*/
  can_filter_init_struct.filter_id_high = 0x047 << 5;
  can_filter_init_struct.filter_id_low = 0x046 << 5;
  can_filter_init_struct.filter_mask_high = 0x0 << 5;
  can_filter_init_struct.filter_mask_low = 0x0 << 5;

  can_filter_init(CAN1, &can_filter_init_struct);

  /**
   * Users need to configure CAN1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding CAN1 interrupt.
   *     --can_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f403a_407_int.c file.
   *     --void USBFS_H_CAN1_TX_IRQHandler(void)
   *     --void USBFS_L_CAN1_RX0_IRQHandler(void)
   *     --void CAN1_RX1_IRQHandler(void)
   *     --void CAN1_SE_IRQHandler(void)
   */

  /*can1 rx0 interrupt config--------------------------------------------------------*/ 
  //can_interrupt_enable(CAN1, CAN_RF0MIEN_INT, TRUE);

  /*can1 rx1 interrupt config--------------------------------------------------------*/ 
  //can_interrupt_enable(CAN1, CAN_RF1MIEN_INT, TRUE);

  /*can1 se interrupt config---------------------------------------------------------*/ 
  //can_interrupt_enable(CAN1, CAN_ETRIEN_INT, TRUE);
  //can_interrupt_enable(CAN1, CAN_EOIEN_INT, TRUE);

  /*can1 tx interrupt config---------------------------------------------------------*/ 
  //can_interrupt_enable(CAN1, CAN_TCIEN_INT, TRUE);

  /* add user code begin can1_init 2 */

  /* add user code end can1_init 2 */
}