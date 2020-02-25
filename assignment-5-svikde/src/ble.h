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
#include "event.h"

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

#endif /* SRC_BLE_H_ */
