/***
 * @File	ble.h
 * @Brief	Contains function declarations pertaining to BLE Operations.
 * @Author	Souvik De
 * @Date	10/04/2019
 * @Version	1.0
 ***/

#ifndef SRC_BLE_H_
#define SRC_BLE_H_

#include <native_gecko.h>
#include "infrastructure.h"
#include "gatt_db.h"
#include "math.h"
#include "event.h"
#include "display.h"
#include "gpio.h"
#include "ble_device_type.h"
#include "gecko_ble_errors.h"

#define ADV_INTVL_MIN	(400)
#define ADV_INTVL_MAX	(400)
#define CON_INTVL_MIN	(60)
#define CON_INTVL_MAX	(60)
#define SLAVE_LATENCY	(3)
#define TIMEOUT			(600)

/* RSSI Macro Definitions */
#define RSSI_NEG35DB		(-35)
#define RSSI_NEG45DB		(-45)
#define RSSI_NEG55DB		(-55)
#define RSSI_NEG65DB		(-65)
#define RSSI_NEG75DB		(-75)
#define RSSI_NEG85DB		(-85)

/* TX Power Macro Definitions */
#define TX_POWER_MIN		(-260)
#define TX_POWER_NEG20DB	(-200)
#define TX_POWER_NEG15DB	(-150)
#define TX_POWER_NEG5DB		(-50)
#define TX_POWER_0DB		(0)
#define TX_POWER_5DB		(50)
#define TX_POWER_MAX		(80)


#define BONDINGS_ALLOWED	(1)
#define ACCEPT_BOND_REQ		(1)

/* Configuration Flags */
#define MITM_FLAG			(0x01)
#define ENCRIPT_FLAG		(0x02)
#define SECURE_CON_FLAG		(0x04)
#define BOND_REQ_FLAG		(0x08)
#define ONLY_BOND_DEV_FLAG	(0x10)

#define HLTH_THERM_UUID_H	(0x18)
#define HLTH_THERM_UUID_L	(0x09)
#define HLTH_THERM_UUID_LEN	(2)

#define TEMP_MESMT_UUID_L	(0x1C)
#define TEMP_MESMT_UUID_H	(0x2A)
#define TEMP_MESMT_UUID_LEN	(2)

#define BTN_SERV_UUID_0		(0x89)
#define BTN_SERV_UUID_1		(0x62)
#define BTN_SERV_UUID_2		(0x13)
#define BTN_SERV_UUID_3		(0x2d)
#define BTN_SERV_UUID_4		(0x2a)
#define BTN_SERV_UUID_5		(0x65)
#define BTN_SERV_UUID_6		(0xec)
#define BTN_SERV_UUID_7		(0x87)
#define BTN_SERV_UUID_8		(0x3e)
#define BTN_SERV_UUID_9		(0x43)
#define BTN_SERV_UUID_10	(0xc8)
#define BTN_SERV_UUID_11	(0x38)
#define BTN_SERV_UUID_12	(0x01)
#define BTN_SERV_UUID_13	(0x00)
#define BTN_SERV_UUID_14	(0x00)
#define BTN_SERV_UUID_15	(0x00)
#define BTN_SERV_UUID_LEN	(16)

#define BTN_CHAR_UUID_12	(0x02)
#define BTN_CHAR_UUID_LEN	(16)

bool IsDeviceBonded;

typedef enum Bluetooth_State
{
	BT_DISCOVER_HT_SERVICES = 0,
	BT_DISCOVER_HT_CHARECTERISCTICS,
	BT_DISCOVER_BTN_SERVICES,
	BT_DISCOVER_BTN_CHARECTERISCTICS,
	BT_SEND_NOTIFICATION,
	BT_WAIT_FOR_NOTIFICATION
}Bluetooth_State;

Bluetooth_State BT_Present_State;
Bluetooth_State BT_Next_State;


/***
 * @Brief	Gecko defined State Machine for BLE
 *
 * @Param	BLE Event Structure
 *
 * @Return	Update Failure or Success
 ***/
bool gecko_update(struct gecko_cmd_packet* evt);

/***
 * @Brief	Assignment defined State Machine for BLE
 *
 * @Param	BLE Event Structure
 *
 * @Return	void
 ***/
void GeckoUpdate(struct gecko_cmd_packet* evt);

/***
 * @Brief	Checks if an event is encountered
 *
 * @Param	(1)Event under observation
 * 			(2)Mask for Event Under Observation
 *
 * @Return	Failure or Success
 ***/
__STATIC_INLINE uint8_t IsBLEEventSet(uint32_t EventBitmask, uint8_t Mask)
{
	return ((EventBitmask & Mask) == Mask);
}

/***
 * @Brief	Scales & Transmits temperature
 *
 * @Param	Temperature in Float
 *
 * @Return	void
 ***/
void TransmitTemperature(float TemperatureFloat);

/***
 * @Brief	Scans button state of PB0 & transmits over BLE
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Transmit_ButtonState(void);

/***
 * @return 	a float value based on a UINT32 value written by FLT_TO_UINT32 and
 * 			UINT32_TO_BITSTREAM
 * @param 	value_start_little_endian is a pointer to the first byte of the float
 * 			which is represented in UINT32 format from FLT_TO_UINT32/UINT32_TO_BITSTREAM
 ***/
float gattUint32ToFloat(const uint8_t *value_start_little_endian);


#endif /* SRC_BLE_H_ */
