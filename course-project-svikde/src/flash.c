/***
 * @File	flash.c
 * @Brief	Contains function definitions pertaining to persistent memory
 * 			on-board the blue gecko.
 * @Author	Souvik De
 * @Date	12/05/2019
 * @Version	1.0
 ***/

#include "flash.h"
#include "log.h"

uint8_t Flash_FetchData(uint8_t FlashID)
{
	uint8_t data = 0;
    uint16_t Response = 0;
    struct gecko_msg_flash_ps_load_rsp_t* FlashResponse;

    switch (FlashID)
    {
        case ROOM_OCCUPENCY_COUNT_ID:
        	FlashResponse = gecko_cmd_flash_ps_load(COUNT_ADDR);
        	data = FlashResponse->value.data[0];
            break;

        case LUMENS_RESIDING_VALUE_ID:
        	FlashResponse = gecko_cmd_flash_ps_load(LUMENS_ADDR);
            data = FlashResponse->value.data[0];
            break;
    }

    Response = FlashResponse->result;
    if(Response)
    {
        LOG_INFO("\nFailed to fetch value from Flash: %x\n", Response);
    }
    else
    {
        LOG_INFO("\nFlash Load Success\n");
    }

    return data;
}

void Flash_SaveData(uint8_t FlashID, uint8_t *data)
{
    uint16_t Response = 0;

    switch (FlashID)
    {
        case ROOM_OCCUPENCY_COUNT_ID:
        	Response = gecko_cmd_flash_ps_save(COUNT_ADDR, COUNT_LENGTH, data)->result;
            break;

        case LUMENS_RESIDING_VALUE_ID:
        	Response = gecko_cmd_flash_ps_save(LUMENS_ADDR, LUMEN_LENGTH, data)->result;
            break;
    }

    if (Response)
    {
        LOG_INFO("\nFailed to save data to Flash: %x\n", Response);
    }
    else
    {
        LOG_INFO("\nFlash Store Success\n");
    }
}

