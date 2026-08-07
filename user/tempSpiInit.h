#pragma once
#include "at32f403a_407_conf.h"
#include "at32f403a_407_int.h"
/* Registers */
#define AD7785_REG_COMM		0 /* Communications Register (WO, 8-bit) */
#define AD7785_REG_STAT		0 /* Status Register	     (RO, 8-bit) */
#define AD7785_REG_MODE		1 /* Mode Register	     (RW, 16-bit */
#define AD7785_REG_CONF		2 /* Configuration Register  (RW, 16-bit) */
#define AD7785_REG_DATA		3 /* Data Register	     (RO, 16-/24-bit) */
#define AD7785_REG_ID		4 /* ID Register	     (RO, 8-bit) */
#define AD7785_REG_IO		5 /* IO Register	     (RO, 8-bit) */
#define AD7785_REG_OFFSET	6 /* Offset Register	     (RW, 16-bit
				   * (AD7792)/24-bit (AD7785)) */
#define AD7785_REG_FULLSALE	7 /* Full-Scale Register
				   * (RW, 16-bit (AD7792)/24-bit (AD7785)) */

/* Communications Register Bit Designations (AD7785_REG_COMM) */
#define AD7785_COMM_WEN		(1 << 7) /* Write Enable */
#define AD7785_COMM_WRITE	(0 << 6) /* Write Operation */
#define AD7785_COMM_READ	(1 << 6) /* Read Operation */
#define AD7785_COMM_ADDR(x)	(((x) & 0x7) << 3) /* Register Address */
#define AD7785_COMM_CREAD	(1 << 2) /* Continuous Read of Data Register */

/* Status Register Bit Designations (AD7785_REG_STAT) */
#define AD7785_STAT_RDY		(1 << 7) /* Ready */
#define AD7785_STAT_ERR		(1 << 6) /* Error (Overrange, Underrange) */
#define AD7785_STAT_CH3		(1 << 2) /* Channel 3 */
#define AD7785_STAT_CH2		(1 << 1) /* Channel 2 */
#define AD7785_STAT_CH1		(1 << 0) /* Channel 1 */

/* Mode Register Bit Designations (AD7785_REG_MODE) */
#define AD7785_MODE_SEL(x)	(((x) & 0x7) << 13) /* Operation Mode Select */
#define AD7785_MODE_SEL_MASK	(0x7 << 13) /* Operation Mode Select mask */
#define AD7785_MODE_CLKSRC(x)	(((x) & 0x3) << 6) /* ADC Clock Source Select */
#define AD7785_MODE_RATE_MASK (0xF << 0) /*Filter Update rate Mask*/
#define AD7785_MODE_RATE(x)	((x) & 0xF) /* Filter Update Rate Select */

#define AD7785_MODE_CONT		0 /* Continuous Conversion Mode */
#define AD7785_MODE_SINGLE		1 /* Single Conversion Mode */
#define AD7785_MODE_IDLE		2 /* Idle Mode */
#define AD7785_MODE_PWRDN		3 /* Power-Down Mode */
#define AD7785_MODE_CAL_INT_ZERO	4 /* Internal Zero-Scale Calibration */
#define AD7785_MODE_CAL_INT_FULL	5 /* Internal Full-Scale Calibration */
#define AD7785_MODE_CAL_SYS_ZERO	6 /* System Zero-Scale Calibration */
#define AD7785_MODE_CAL_SYS_FULL	7 /* System Full-Scale Calibration */

#define AD7785_CLK_INT		0 /* Internal 64 kHz Clock not
				   * available at the CLK pin */
#define AD7785_CLK_INT_CO	1 /* Internal 64 kHz Clock available
				   * at the CLK pin */
#define AD7785_CLK_EXT		2 /* External 64 kHz Clock */
#define AD7785_CLK_EXT_DIV2	3 /* External Clock divided by 2 */

/* Configuration Register Bit Designations (AD7785_REG_CONF) */
#define AD7785_CONF_VBIAS(x)	(((x) & 0x3) << 14) /* Bias Voltage
						     * Generator Enable */
#define AD7785_CONF_BO_EN	(1 << 13) /* Burnout Current Enable */
#define AD7785_CONF_UNIPOLAR	(1 << 12) /* Unipolar/Bipolar Enable */
#define AD7785_CONF_BOOST	(1 << 11) /* Boost Enable */
#define AD7785_CONF_GAIN(x)	(((x) & 0x7) << 8) /* Gain Select */
#define AD7785_CONF_REFSEL(x)	((x) << 6) /* INT/EXT Reference Select */
#define AD7785_CONF_BUF		(1 << 4) /* Buffered Mode Enable */
#define AD7785_CONF_CHAN(x)	((x) & 0xf) /* Channel select */
#define AD7785_CONF_CHAN_MASK	0xf /* Channel select mask */

#define AD7785_CH_AIN1P_AIN1M	0 /* AIN1(+) - AIN1(-) */
#define AD7785_CH_AIN2P_AIN2M	1 /* AIN2(+) - AIN2(-) */
#define AD7785_CH_AIN3P_AIN3M	2 /* AIN3(+) - AIN3(-) */
#define AD7785_CH_AIN1M_AIN1M	3 /* AIN1(-) - AIN1(-) */
#define AD7785_CH_TEMP		6 /* Temp Sensor */
#define AD7785_CH_AVDD_MONITOR	7 /* AVDD Monitor */

/* ID Register Bit Designations (AD7785_REG_ID) */
#define AD7785_ID		0xB

#define AD7799_RESET_DATA	 0xFF
void motorTemprInit(void);
uint32_t readRawTempr(void);