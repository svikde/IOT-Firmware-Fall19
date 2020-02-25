/***
 * @File	event.c
 * @Brief	Contains State Machine Definition and function definitions
 * 			pertaining to handling events.
 * @Author	Souvik De
 * @Date	09/27/2019
 * @Version	1.0
 ***/

#include "event.h"

void Event_Init(void)
{
	Interrupt_Event_Bitmask = 0;
	temperature_sensor.data8 = 0;
	temperature_sensor.temperature_celsius = 0;
	temperature_sensor.current_state = POWER_UP;
}

void Set_Event(uint8_t Event_Mask)
{
	CORE_DECLARE_IRQ_STATE;

	CORE_ENTER_CRITICAL();
	Interrupt_Event_Bitmask |= (1 << Event_Mask);
	CORE_EXIT_CRITICAL();
}

uint8_t IsEventSet(uint8_t Event)
{
	uint8_t value = 0;

	CORE_DECLARE_IRQ_STATE;

	CORE_ENTER_CRITICAL();
	value = Interrupt_Event_Bitmask & Event;
	CORE_EXIT_CRITICAL();

	return (value == Event);
}

void Clear_Event(uint8_t Event_Mask)
{
	CORE_DECLARE_IRQ_STATE;

	CORE_ENTER_CRITICAL();
	Interrupt_Event_Bitmask &= ~(1 << Event_Mask);
	CORE_EXIT_CRITICAL();
}

void Change_State(uint32_t EventBitmask)
{
	switch(temperature_sensor.current_state)
	{
		case POWER_UP:
			if (IsBLEEventSet(EventBitmask, TIMER_EXPIRED))
			{
				//Clear_Event(TIMER_EXPIRED_MASK);
				//displayUpdate();
				//gpioI2CSensorEnSetOn();
				timerSetEventInMS(DELAY80MS);
				next_state = I2C_WRITE_START;
			}
			break;

		case I2C_WRITE_START:
			if (IsBLEEventSet(EventBitmask, DELAY_EXPIRED))
			{
				//Clear_Event(DELAY_EXPIRED_MASK);
				temperature_sensor.data8 = MEASURE_TEMP_CMD;
				SleepBlockStart(sleepEM1);
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &temperature_sensor.data8, sizeof(uint8_t));
				next_state = I2C_WRITE_DONE;
			}
			break;

		case I2C_WRITE_DONE:
			if (IsBLEEventSet(EventBitmask, I2C_TRANSFER_DONE))
			{
				//Clear_Event(I2C_TRANSFER_DONE_MASK);
				SleepBlockEnd(sleepEM1);
				timerSetEventInMS(DELAY10MS);
				next_state = I2C_READ_START;
			}
			break;

		case I2C_READ_START:
			if (IsBLEEventSet(EventBitmask, DELAY_EXPIRED))
			{
				//Clear_Event(DELAY_EXPIRED_MASK);
				//temperature_sensor.data = 0;
				SleepBlockStart(sleepEM1);
				I2C_transfer_nonblocking(I2C_FLAG_READ, temperature_sensor.data16, sizeof(uint16_t));
				next_state = POWER_DOWN;
			}
			break;

		case POWER_DOWN:
			if (IsBLEEventSet(EventBitmask, I2C_TRANSFER_DONE))
			{
				//Clear_Event(I2C_TRANSFER_DONE_MASK);
				SleepBlockEnd(sleepEM1);
				temperature_sensor.temperature_celsius = CONVERT2CELS(SWAP_BYTES(*(temperature_sensor.data16)));
				/*LOG_INFO("Temperature is %.2f C\n", temperature_sensor.temperature_celsius); //Log Temperature*/
				//displayUpdate();
				displayPrintf(DISPLAY_ROW_TEMPVALUE,"Temperature: %.2f",temperature_sensor.temperature_celsius);
				displayUpdate();

				TransmitTemperature(temperature_sensor.temperature_celsius);
				//gpioI2CSensorEnSetOff();
				next_state = POWER_UP;
			}
			break;
	}

	if (temperature_sensor.current_state != next_state)
	{
		//Log change of state
		LOG_INFO("Temperature Sensor Transitioned from state %d to %d\n", temperature_sensor.current_state, next_state);
		temperature_sensor.current_state = next_state;
	}
}

