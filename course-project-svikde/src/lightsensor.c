/***
 * @File	lightsensor.c
 * @Brief	Contains the Light Sensor I2C-based State Machine and function definitions
 * 			pertaining to the Light Sensor.
 * @Author	Souvik De
 * @Date	12/01/2019
 * @Version	1.0
 ***/

#include "lightsensor.h"
#include "flash.h"
#include <math.h>

float CalculateLumen(int visible_lux, int ir_lux)
{
	float div_result, lumen;

	div_result = (float)ir_lux/(float)visible_lux;

	if(div_result > LUX_CONST1 && div_result <= LUX_CONST2)
		lumen = (LUX_CONST3 * visible_lux) - (LUX_CONST4 * visible_lux * pow(div_result,LUX_CONST5));
	else if(div_result > LUX_CONST2 && div_result <= LUX_CONST6)
		lumen = (LUX_CONST7 * visible_lux) - (LUX_CONST8 * ir_lux);
	else if(div_result > LUX_CONST6 && div_result <= LUX_CONST9)
		lumen = (LUX_CONST10 * visible_lux) - (LUX_CONST11 * ir_lux);
	else if(div_result > LUX_CONST9 && div_result <= LUX_CONST12)
		lumen = (LUX_CONST13 * visible_lux) - (LUX_CONST14 * ir_lux);
	else
		lumen = LUX_CONST1;

	return lumen;
}

void Init_LightSensor(void)
{
	light_obj.cmd = 0;
	light_obj.data = 0;
	light_obj.lumen = 0;
	light_obj.lux_irlight = 0;
	light_obj.ir_luxlow = 0;
	light_obj.ir_luxhigh = 0;
	light_obj.lux_visiblelight = 0;
	light_obj.visible_luxlow = 0;
	light_obj.visible_luxhigh = 0;
	light_obj.current_state = ENABLE_SENSOR;
}

void LightSensor_StateMachine(uint32_t EventBitmask)
{
	static uint8_t sequencer = 0;
	uint8_t parser = 0;

	switch(light_obj.current_state)
	{
		case ENABLE_SENSOR:
			if (IsEventSet(EventBitmask, TIMER_EXPIRED))
			{
				EnableLightSensor();
				DelayMS(50);
				next_state = POWER_UP_1;
			}
			break;

		case POWER_UP_1:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				light_obj.cmd = CMD_CONTROL_REGISTER;
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &light_obj.cmd, sizeof(uint8_t));
				next_state = POWER_UP_2;
			}
			break;

		case POWER_UP_2:
			if (IsEventSet(EventBitmask, I2C_TRANSFER_DONE))
			{
				light_obj.cmd = CONTROL_REGISTER;
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &light_obj.cmd, sizeof(uint8_t));
				next_state = POWER_UP_DONE;
			}
			break;

		case POWER_UP_DONE:
			if (IsEventSet(EventBitmask, I2C_TRANSFER_DONE))
			{
				I2C_transfer_nonblocking(I2C_FLAG_READ, &light_obj.data, sizeof(uint8_t));
				next_state = I2C_WRITE_CMD_DATA0LOW_REGISTER;
			}
			break;

		case I2C_WRITE_CMD_DATA0LOW_REGISTER:
			if (IsEventSet(EventBitmask, TIMER_EXPIRED))
			{
				light_obj.cmd = CMD_DATA0LOW_REGISTER_8;
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &light_obj.cmd, sizeof(uint8_t));
				next_state = SEQUENCER;
			}
			break;

		case I2C_READ_VISIBLE_LUX_LOW:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				I2C_transfer_nonblocking(I2C_FLAG_READ, &light_obj.visible_luxlow, sizeof(uint8_t));
				next_state = SEQUENCER;
			}
			break;

		case I2C_WRITE_CMD_DATA0HIGH_REGISTER:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				light_obj.cmd = CMD_DATA0HIGH_REGISTER_8;
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &light_obj.cmd, sizeof(uint8_t));
				next_state = SEQUENCER;
			}
			break;

		case I2C_READ_VISIBLE_LUX_HIGH:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				I2C_transfer_nonblocking(I2C_FLAG_READ, &light_obj.visible_luxhigh, sizeof(uint8_t));
				next_state = SEQUENCER;
			}
			break;

		case I2C_WRITE_CMD_DATA1LOW_REGISTER:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				light_obj.lux_visiblelight = light_obj.visible_luxhigh << 8 | light_obj.visible_luxlow;

				//LOG_INFO("\nVisible LUX is %u\n", light_obj.lux_visiblelight, light_obj.visible_luxhigh);

				light_obj.cmd = CMD_DATA1LOW_REGISTER_8;
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &light_obj.cmd, sizeof(uint8_t));
				next_state = SEQUENCER;
			}
			break;

		case I2C_READ_IR_LUX_LOW:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				I2C_transfer_nonblocking(I2C_FLAG_READ, &light_obj.ir_luxlow, sizeof(uint8_t));
				next_state = I2C_WRITE_CMD_DATA1HIGH_REGISTER;
			}
			break;

		case I2C_WRITE_CMD_DATA1HIGH_REGISTER:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				light_obj.cmd = CMD_DATA1HIGH_REGISTER_8;
				I2C_transfer_nonblocking(I2C_FLAG_WRITE, &light_obj.cmd, sizeof(uint8_t));
				next_state = SEQUENCER;
			}
			break;

		case I2C_READ_IR_LUX_HIGH:
			if (IsEventSet(EventBitmask, DELAY_EXPIRED))
			{
				I2C_transfer_nonblocking(I2C_FLAG_READ, &light_obj.ir_luxhigh, sizeof(uint8_t));
				next_state = CALCULATE_LUMENS;
			}
			break;

		case SEQUENCER:
			if (IsEventSet(EventBitmask, I2C_TRANSFER_DONE))
			{
				DelayMS(5);

				switch(sequencer)
				{
					case SEQUENCER_STATE0:
						next_state = I2C_READ_VISIBLE_LUX_LOW;
						break;

					case SEQUENCER_STATE1:
						next_state = I2C_WRITE_CMD_DATA0HIGH_REGISTER;
						break;

					case SEQUENCER_STATE2:
						next_state = I2C_READ_VISIBLE_LUX_HIGH;
						break;

					case SEQUENCER_STATE3:
						next_state = I2C_WRITE_CMD_DATA1LOW_REGISTER;
						break;

					case SEQUENCER_STATE4:
						next_state = I2C_READ_IR_LUX_LOW;
						break;

					case SEQUENCER_STATE5:
						next_state = I2C_WRITE_CMD_DATA1HIGH_REGISTER;
						break;

					case SEQUENCER_STATE6:
						next_state = I2C_READ_IR_LUX_HIGH;
						break;
				}

				if (sequencer == SEQUENCER_STATE6)
					sequencer = SEQUENCER_STATE0;
				else
					sequencer++;
			}
			break;

		case CALCULATE_LUMENS:
			if (IsEventSet(EventBitmask, I2C_TRANSFER_DONE))
			{
				light_obj.lux_irlight = light_obj.ir_luxhigh << 8 | light_obj.ir_luxlow;

				//LOG_INFO("\nIR LUX is %u\n", light_obj.lux_irlight);

				light_obj.lumen = CalculateLumen(light_obj.lux_visiblelight, light_obj.lux_irlight);

				LOG_INFO("\nLight Intensity is %.3f\n", light_obj.lumen);

				displayPrintf(DISPLAY_ROW_LUMENS,"Lumens: %.3f",light_obj.lumen);

				TransmissionControl.occupency_count_read = Flash_FetchData(ROOM_OCCUPENCY_COUNT_ID);
				TransmissionControl.residing_lumens_read = Flash_FetchData(LUMENS_RESIDING_VALUE_ID);

				TransmissionControl.occupency_count_new = TransmissionControl.occupency_count_read + 1;
				Flash_SaveData(ROOM_OCCUPENCY_COUNT_ID, &TransmissionControl.occupency_count_new);

				/* Send only if Generated Value is Over residing value by given threshold */
				if(abs((uint8_t)light_obj.lumen - TransmissionControl.residing_lumens_read) >= LUMEN_THRESHOLD)
				{
					if(light_obj.lumen <= LUMEN_TRANSM_LOWER_LIMIT)
					{
						TransmissionControl.residing_lumens_new = LUMEN_TRANSM_LOWER_LIMIT;
						Flash_SaveData(LUMENS_RESIDING_VALUE_ID, &TransmissionControl.residing_lumens_new);
					}
					else if(light_obj.lumen >= LUMEN_TRANSM_UPPER_LIMIT)
					{
						TransmissionControl.residing_lumens_new = LUMEN_TRANSM_UPPER_LIMIT;
						Flash_SaveData(LUMENS_RESIDING_VALUE_ID, &TransmissionControl.residing_lumens_new);
					}
					else
					{
						TransmissionControl.residing_lumens_new = (uint8_t)light_obj.lumen;
						Flash_SaveData(LUMENS_RESIDING_VALUE_ID, &TransmissionControl.residing_lumens_new);
					}

					//Publish multiple times to account for missing packets
					for(parser = 0 ; parser <= PIR_FILTER_LEN/2 ; parser++)
					{
						//Send PIR State
						Send_PIRState(ROOM_OCCUPIED);

						//Send Lumen Value
						Send_LightState();

						displayPrintf(DISPLAY_ROW_PUBLISHING, "Pub : Occupied - %u", TransmissionControl.residing_lumens_new);
					}
				}

				DisplayUpdateFlashValues();

				//Load Power Management Off
				DisableLightSensor();

				TransmissionControl.PIR_Filter = 0;

				next_state = ENABLE_SENSOR;
			}
			break;
	}

	if (light_obj.current_state != next_state)
	{
		LOG_INFO("Light Sensor Transitioned from state %d to %d\n",light_obj.current_state, next_state);
		light_obj.current_state = next_state;
	}
}
