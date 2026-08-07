#include "tempSpiInit.h"
#include "at32f403a_407_board.h"
#include <string.h>


static const uint8_t ad7785_reg_size[] = {
	[AD7785_REG_COMM] = 1,
	[AD7785_REG_MODE] = 2,
	[AD7785_REG_CONF] = 2,
	[AD7785_REG_DATA] = 3,
	[AD7785_REG_ID] = 1,
	[AD7785_REG_IO] = 1,
	[AD7785_REG_OFFSET] = 3,
	[AD7785_REG_FULLSALE] = 3
};

volatile uint16_t resp = 0;
static uint8_t sendReciveByteSpi(uint8_t byte)
{
	while (spi_i2s_flag_get(SPI4, SPI_I2S_TDBE_FLAG) == RESET) ;
	spi_i2s_data_transmit(SPI4, byte);
	while (spi_i2s_flag_get(SPI4, SPI_I2S_RDBF_FLAG) == RESET) ;
	return (spi_i2s_data_receive(SPI4));
}

static int8_t  no_os_spi_write_and_read(uint8_t *buff, uint8_t size)
{
	gpio_bits_reset(GPIOE, GPIO_PINS_4);
	//delay_us(2);
	uint8_t i = 0;
	for (i = 0; i < size; i++)
	{
		buff[i] = sendReciveByteSpi(buff[i]);
		
	}
	//delay_us(2);
	gpio_bits_set(GPIOE, GPIO_PINS_4);
	return 0;
}
/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param device - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7785_read(uint8_t reg_addr, uint32_t *reg_data)
{
	int32_t ret;
	uint8_t i;
	uint8_t buff[4];
	uint8_t buff_size;

	*reg_data = 0;

	buff_size = ad7785_reg_size[reg_addr];

	buff[0] = AD7785_COMM_READ | AD7785_COMM_ADDR(reg_addr);

	memset((buff + 1), 0, buff_size + 1);

	ret = no_os_spi_write_and_read(buff, buff_size + 1);
	if (ret)
		return -1;

	for (i = 1; i < buff_size + 1; i++)
		*reg_data = (*reg_data << 8) | buff[i];

	return 0;
}

/**
 * @brief Write device register.
 * @param device - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7785_write(uint8_t reg_addr, uint32_t reg_data)
{
	int32_t ret;
	uint8_t i;
	uint8_t buff[4];
	uint8_t buff_size;

	buff_size = ad7785_reg_size[reg_addr];
	buff[0] = AD7785_COMM_WRITE | AD7785_COMM_ADDR(reg_addr);

	for (i = 1; i < buff_size + 1; i++)
		buff[i] = reg_data >> ((buff_size - i) * 8);

	ret = no_os_spi_write_and_read(buff, buff_size + 1);
	if (ret)
		return -1;

	return ret;
}

/**
 * @brief Software reset of the device.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7785_reset(void)
{
	uint8_t reset_data[4] = {
		AD7799_RESET_DATA,
		AD7799_RESET_DATA,
		AD7799_RESET_DATA,
		AD7799_RESET_DATA
	};

	return no_os_spi_write_and_read(reset_data, 4);
}

uint32_t regdata = 0; 
uint32_t datareg = 0;
uint32_t temperADC = 0;
static uint16_t setADCchannel(uint8_t ch)
{
	/*read conf and set desired*/
	resp = ad7785_read(AD7785_REG_CONF, &regdata);
	/*set channel*/
	regdata &= ~AD7785_CONF_CHAN_MASK;
	regdata |= AD7785_CONF_CHAN(ch);
	resp = ad7785_write(AD7785_REG_CONF, regdata);
}
static uint16_t initADCsettings(void)
{
	/*read mode and set desired*/
	resp = ad7785_read(AD7785_REG_MODE, &regdata);
	/*set mode*/
	resp &= ~AD7785_MODE_SEL_MASK;
	regdata |= AD7785_MODE_SEL(AD7785_MODE_CONT);
	/*set clock source*/
	regdata |= AD7785_MODE_CLKSRC(AD7785_CLK_INT);
	/*set update rate*/
	regdata &= ~AD7785_MODE_RATE_MASK;
	regdata |= AD7785_MODE_RATE(1);
	resp = ad7785_write(AD7785_REG_MODE, regdata);
	//delay_us(5000);
	/*read conf and set desired*/
	resp = ad7785_read(AD7785_REG_CONF, &regdata);
	/*clear settings*/
	regdata = 0;
	/*set gain*/
	regdata |= AD7785_CONF_GAIN(0);
	/*set ref select*/
	regdata |= AD7785_CONF_REFSEL(0);
	/*set bias*/
	regdata |= AD7785_CONF_VBIAS(0);
	/*set UNI polar mode*/
	regdata |= AD7785_CONF_UNIPOLAR;
	resp = ad7785_write(AD7785_REG_CONF, regdata);
	/*set channel*/
	setADCchannel(AD7785_CH_AIN1P_AIN1M);
	//delay_us(5000);
	resp = ad7785_read(AD7785_REG_STAT, &regdata);
			
	return 1;
	
}

enum ALG_STATE
{
	RESET_STATE = 0,
	WAIT_STATE,
	INIT_STATE,
	WORK_STATE
   
};
static uint8_t readChannel(uint8_t ch)
{
	
	ad7785_read(AD7785_REG_STAT, &regdata);
	if ((regdata & AD7785_STAT_RDY) == 0) setADCchannel(ch);
	return regdata;
}
enum ALG_STATE actState = RESET_STATE;
uint32_t readRawTempr(void)
{
	uint32_t rawData = 0;
	static uint16_t delayCNT_ms = 50;
	static uint16_t okData = 0;
	switch (actState)
	{
	case RESET_STATE:
		/*reset comm interface*/
		ad7785_reset();
		delayCNT_ms = 50;
		actState = WAIT_STATE;
		break;
	case WAIT_STATE:
		if (delayCNT_ms-- < 1) actState = INIT_STATE;
		break;
	case INIT_STATE:
		initADCsettings();
		actState = WORK_STATE;
		delayCNT_ms = 50;
		break;
	case WORK_STATE:
		okData = 0;
		resp = readChannel(AD7785_CH_AIN1P_AIN1M);
		if ((resp & AD7785_STAT_RDY) == 0)
		{
			okData = 1;
			ad7785_read(AD7785_REG_DATA, &rawData);
			temperADC = (rawData >> 4) * 1;
		}
		break;
	default:
		break;
	}

	return temperADC;
	
}


static void spi_gpio_config(void)
{
	gpio_init_type gpio_init_struct;
	
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);

	gpio_default_para_init(&gpio_init_struct);
	/* spi SCLK DI DO pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pins = GPIO_PINS_2 | GPIO_PINS_5 | GPIO_PINS_6;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOE, &gpio_init_struct);
	/* spi CS pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_pins = GPIO_PINS_4;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(GPIOE, &gpio_init_struct);
	
	gpio_bits_set(GPIOE, GPIO_PINS_4);
}

void motorTemprInit(void)
{
	spi_gpio_config();
	crm_periph_clock_enable(CRM_SPI4_PERIPH_CLOCK, TRUE);
	
	spi_init_type spi_init_struct;
	spi_default_para_init(&spi_init_struct);
	spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
	spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
	spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_64;
	spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
	spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
	spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
	spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
	spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
	spi_init(SPI4, &spi_init_struct);

	spi_enable(SPI4, TRUE);
	
}