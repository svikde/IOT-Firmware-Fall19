/***
 * @File	i2c.c
 * @Brief	Contains function definitions required
 * 			to facilitate I2C transfers
 * @Author	Souvik De
 * @Date	09/18/2019
 * @Version	2.0
 ***/

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

	NVIC_EnableIRQ(I2C0_IRQn);
}

void I2C_transfer_nonblocking(uint16_t TransactionType, uint8_t *data, uint16_t length)
{
	I2C_Nonblocking_PacketCongif.addr = (Si7021_DEVICE_ADDR << 1);
	I2C_Nonblocking_PacketCongif.flags = TransactionType;
	I2C_Nonblocking_PacketCongif.buf[0].data = data;
	I2C_Nonblocking_PacketCongif.buf[0].len = length;

	I2C_TransferInit(I2C0, &I2C_Nonblocking_PacketCongif);
}

void I2C0_IRQHandler(void)
{
	I2C_TransferReturn_TypeDef status = I2C_Transfer(I2C0);
	if(status != i2cTransferInProgress)
	{
		gecko_external_signal(I2C_TRANSFER_DONE);
	}
}

