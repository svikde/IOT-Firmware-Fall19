/***
 * @File	i2c.h
 * @Brief	Contains macros and function declarations
 * 			pertaining to I2C. The following functions
 * 			are used to read temperature from Si7021.
 * @Author	Souvik De
 * @Date	09/18/2019
 * @Version	1.0
 ***/

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include <i2cspm.h>
#include "letimer.h"
#include "log.h"

/* Command to Read Temperature (No Hold Master Mode) */
#define MEASURE_TEMP_CMD	(0xF3)

/* Data-sheet define calculation for temperature in Celsius */
#define CONVERT2CELS(data)	((((175.72)*(data))/(65536))-(46.85))

/* Si7021 Temperature Sensor Device Address */
#define Si7021_DEVICE_ADDR	(0x40)

/***
 * @Brief	Initializes the I2C Bus.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void I2C_init(void);

/***
 * @Brief	Used to write or read data to/from the I2C Bus.
 *
 * @Param	(1) Read or Write Flag
 * 			(2)	Write & Read through this variable
 * 			(3) Length of data to write or read
 *
 * @Return	Execution Status
 ***/
I2C_TransferReturn_TypeDef I2C_transfer(uint16_t TransactionType, uint16_t *data, uint8_t length);

/***
 * @Brief	Read Temperature form the Si7021 Temperature sensor.
 *
 * @Param	void
 *
 * @Return	Temperature in Celsius
 ***/
float ReadTemperature(void);

#endif /* SRC_I2C_H_ */
