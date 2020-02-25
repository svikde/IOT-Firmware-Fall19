/***
 * @File	flash.h
 * @Brief	Contains function declarations pertaining to persistent memory
 * 			on-board the blue gecko.
 * @Author	Souvik De
 * @Date	12/05/2019
 * @Version	1.0
 ***/

#ifndef SRC_FLASH_H_
#define SRC_FLASH_H_

#include "stdint.h"
#include "native_gecko.h"

#define ROOM_OCCUPENCY_COUNT_ID    		(0x01)
#define LUMENS_RESIDING_VALUE_ID   		(0x02)

#define COUNT_ADDR       				(0x4001)
#define LUMENS_ADDR  					(0x4002)

#define COUNT_LENGTH 			        (1)
#define LUMEN_LENGTH        			(1)

/***
 * @Brief	Read Data from Flash Storage
 *
 * @Param	Flash-ID to indicate the type of data being read
 *
 * @Return	Stored Data
 ***/
uint8_t Flash_FetchData(uint8_t FlashID);

/***
 * @Brief	Store data to Flash
 *
 * @Param	(1) Data Type identifier, (2) Data to be saved
 *
 * @Return	void
 ***/
void Flash_SaveData(uint8_t FlashID, uint8_t *data);

#endif /* SRC_FLASH_H_ */
