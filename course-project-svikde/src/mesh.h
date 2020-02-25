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

#define PIR_FILTER_LEN			(5)		//PIR must trigger at-least half this value to register Motion.
#define LUMEN_THRESHOLD			(5)		//Lumen values will be transmitted when residing values change above/beyond this threshold.

#define ABSENCE_MARK_THREHOLD	(30)	//PIR will detect Room Empty when not triggered for the given value in seconds
#define ABSENCE_PUBLISH_DELAY	(5)		//Publish Room Empty at this Interval

#define ROOM_EMPTY				(0)
#define ROOM_OCCUPIED			(1)

#define FRIEND_QUEUE_LEN		(2)
#define POLL_TIMEOUT			(1000)

/* Event Enumerator */
enum Event_t{
	TIMER_EXPIRED = 0x01,
	SWITCH0_PRESSED = 0x02,
	SWITCH1_PRESSED = 0x04,
	MOTION_DETECTED = 0x08,
	I2C_TRANSFER_DONE = 0x10,
	DELAY_EXPIRED = 0x20
};

/* Global Variables */
uint16 _elem_index;	// For indexing elements of the node
uint16 _my_address;	// Address of the Primary Element of the Node

/* Controls and and how this Low Power Node Publishes Data */
struct Transmission_Control
{
	uint8_t occupency_count_read;	//Residing Count Value in Flash
	uint8_t occupency_count_new;	//Newly calculated count value
	uint8_t residing_lumens_read;	//Residing Lumen Value in Flash
	uint8_t residing_lumens_new;	//Newly generated Lumen Value
	uint8_t PIR_Filter;				//Number of time PIR got Fired
	uint16_t Absence_Counter;		//Count to detect if room is empty
}TransmissionControl;



/* Function Prototypes */

/***
 * @Brief	Checks if an event is encountered
 *
 * @Param	(1)Event under observation
 * 			(2)Mask for Event Under Observation
 *
 * @Return	Failure or Success
 ***/
__STATIC_INLINE uint8_t IsEventSet(uint32_t EventBitmask, uint8_t Mask)
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
void Send_PIRState(uint8_t value);

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

/***
 * @Brief	Send Lumen using the Generic Level Client Model.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Send_LightState(void);

/***
 * @Brief	Initialize Low Power Mode.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void LowPowerNode_Init(void);

/***
 * @Brief	Update Flash Values and print them on the LCD.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void DisplayUpdateFlashValues(void);

#endif /* SRC_MESH_H_ */
