/*
 * i2c.c
 *
 *  Created on: Sep 18, 2019
 *      Author: souvi
 */

#include "i2c.h"

void I2C_init(void)
{
	I2CSPM_Init_TypeDef I2C_Config = {
			.port = I2C0,
			.sclPort = gpioPortC,
			.sclPin = 10,
			.sdaPort = gpioPortC,
			.sdaPin = 11,
			.portLocationScl = 14,
			.portLocationSda = 16,
			.i2cRefFreq = 0,
			.i2cMaxFreq = I2C_FREQ_STANDARD_MAX,
			.i2cClhr = i2cClockHLRStandard
	};

	I2CSPM_Init(&I2C_Config);
}

I2C_TransferReturn_TypeDef I2C_transfer(uint16_t TransactionType, uint16_t *data, uint8_t length)
{
	I2C_TransferSeq_TypeDef I2C_PacketCongif;

	I2C_PacketCongif.addr = (Si7021_DEVICE_ADDR << 1);
	I2C_PacketCongif.flags = TransactionType;
	I2C_PacketCongif.buf[0].data = data;
	I2C_PacketCongif.buf[0].len = length;

	return I2CSPM_Transfer(I2C0, &I2C_PacketCongif);
}

float ReadTemperature(void)
{
	I2C_TransferReturn_TypeDef status;
	uint16_t data;
	float tempC = 0;

	gpioI2CSensorEnSetOn();

	timerWaitUs(80000);

	data = MEASURE_TEMP_CMD;
	status = I2C_transfer(I2C_FLAG_WRITE, &data, sizeof(uint8_t));
	if(status == i2cTransferDone)
	{
		timerWaitUs(10000);

		data = 0;
		status = I2C_transfer(I2C_FLAG_READ, &data, sizeof(uint16_t));
		if(status == i2cTransferDone)
		{
			/* Swap the Most Significant Byte and the Least Significant Byte */
			tempC = CONVERT2CELS(((data & 0xFF00)>>8) | ((data & 0x00FF)<<8));
		}
		else
		{
			LOG_ERROR("Failed to read from the slave device, Return Code : %d\n", status);
		}
	}
	else
	{
		LOG_ERROR("Failed to write to slave device, Return Code : %d\n", status);
	}

	return tempC;
}


