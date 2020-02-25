/*
 * i2c.h
 *
 *  Created on: Nov 19, 2019
 *      Author: souvi
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include <i2cspm.h>
#include "letimer.h"
#include "mesh.h"
#include "log.h"

#define APDS_9301_DEV_ID	(0x39) //Light sensor address

I2C_TransferSeq_TypeDef I2C_Nonblocking_PacketCongif;

void I2C_init(void);

void I2C_transfer_nonblocking(uint16_t TransactionType, uint8_t *data, uint16_t length);

void I2C0_IRQHandler(void);

#endif /* SRC_I2C_H_ */
