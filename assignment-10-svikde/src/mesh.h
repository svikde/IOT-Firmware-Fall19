/***
 * @File	mesh.h
 * @Brief	Contains function declarations pertaining to BLE Mesh Operations.
 * @Author	Souvik De
 * @Date	11/05/2019
 * @Version	1.0
 ***/

#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#include "log.h"
#include "gpio.h"
#include <string.h>
#include "display.h"
#include "gatt_db.h"
#include <native_gecko.h>
#include "gecko_ble_errors.h"
#include "ble_mesh_device_type.h"

/* Timer handles defines */
#define TIMER_ID_RESTART          78
#define TIMER_ID_FACTORY_RESET    77
#define TIMER_ID_PROVISIONING     66
#define TIMER_ID_RETRANS          10
#define TIMER_ID_FRIEND_FIND      20
#define TIMER_ID_NODE_CONFIGURED  30

/* Tertiary Definitions */
#define CON_HANDLE_CLOSED		(0xFF)
#define SOFT_DELAY_2_SECS		(2 * 32768)
#define EN_ADV_GATT_BEARER		(0x3)
#define INDEX_DEFAULT			(0xffff)

/* Event Enumerator */
enum Event_t{
	TIMER_EXPIRED = 0x1,
	SWITCH0_PRESSED = 0x2,
	SWITCH1_PRESSED = 0x4
};

/* Global Variables */
uint16 _elem_index;	// For indexing elements of the node
uint16 _my_address;	// Address of the Primary Element of the Node

struct Button_State
{
	uint8_t onoff_current;          /**< Current generic on/off value */
	uint8_t onoff_target; 	 		/**< Target generic on/off value */
}ButtonState;

/* Function Prototypes */

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
 * @Brief	Initializes Global Variables
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Mesh_Init(void);

/***
 * @Brief	Factory reset & Remove Provisioning Data
 *
 * @Param	void
 *
 * @Return	void
 ***/
void initiate_factory_reset(void);

/***
 * @Brief	Assign Attribute Name
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Set_GattDB_Device_Name(bd_addr *Ble_addr);

/***
 * @Brief	Transmit Button State
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Send_ButtonState(void);

/***************************************************************************//**
 * Switch node initialization.
 * This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 ******************************************************************************/
void switch_node_init(void);

/* From Gecko Main File */
void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);

/* Custom Defined State Machine for Mesh */
void Mesh_Event_Handler(uint32_t evt_id, struct gecko_cmd_packet *evt);

static errorcode_t onoff_update(uint16_t element_index, uint32_t remaining_ms);

static errorcode_t onoff_update_and_publish(uint16_t element_index,
                                            uint32_t remaining_ms);

void Init_Models(void);

#endif /* SRC_MESH_H_ */
