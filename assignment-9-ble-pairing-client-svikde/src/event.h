/***
 * @File	event.h
 * @Brief	Contains resource declarations required to handle events
 * 			from I2C and LETimer Interrupts and trigger the State
 * 			Machine.
 * @Author	Souvik De
 * @Date	09/27/2019
 * @Version	1.0
 ***/

#ifndef SRC_EVENT_H_
#define SRC_EVENT_H_

#include "ble.h"
#include "i2c.h"
#include "gpio.h"
#include "display.h"
#include "letimer.h"
#include "em_device.h"

//To Swap the MS Byte and LS Byte of the I2C data obtained after read
#define SWAP_BYTES(UINT16)	(((UINT16 & 0xFF00)>>8) | ((UINT16 & 0x00FF)<<8))

/*Even Bit mask for LETIMER0 Interrupt*/
volatile uint32_t Interrupt_Event_Bitmask;

/* State Enumerator */
typedef enum{
	POWER_UP,
	I2C_WRITE_START,
	I2C_WRITE_DONE,
	I2C_READ_START,
	POWER_DOWN
}State_t;

/* Event Enumerator */
enum Event_t{
	TIMER_EXPIRED_MASK = 0,
	TIMER_EXPIRED = 0x1,
	DELAY_EXPIRED_MASK = 1,
	DELAY_EXPIRED = 0x2,
	I2C_TRANSFER_DONE_MASK = 2,
	I2C_TRANSFER_DONE = 0x4,
	PASSKEY_CONFIRMED = 0x8
};

/* Structure containing parameters to operate the state machine and handle
 * I2C Transfers */
struct Si7021_Sensor
{
	uint8_t data8;
	uint8_t data16[2];
	float temperature_celsius;
	State_t current_state;
}temperature_sensor;

State_t next_state;		//Next State Variable

/***
 * @Brief	Initializes the Event Parameters.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Event_Init(void);

/***
 * @Brief	Sets an event atomically in a critical section
 *
 * @Param	Event Mask for Intended Event
 *
 * @Return	void
 ***/
void Set_Event(uint8_t Event_Mask);

/***
 * @Brief	Clears an event atomically in a critical section
 *
 * @Param	Event Mask for Intended Event
 *
 * @Return	void
 ***/
void Clear_Event(uint8_t Event_Mask);

/***
 * @Brief	Checks if an event is encountered
 *
 * @Param	Event under observation
 *
 * @Return	Failure or Success
 ***/
uint8_t IsEventSet(uint8_t Event);

/***
 * @Brief	State machine to acquire temperature from the sensor
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Change_State(uint32_t EventBitmask);

#endif /* SRC_EVENT_H_ */
