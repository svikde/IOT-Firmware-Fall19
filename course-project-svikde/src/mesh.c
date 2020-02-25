/***
 * @File	mesh.c
 * @Brief	Contains functions definitions pertaining to BLE Mesh Operations.
 * @Author	Souvik De
 * @Date	10/04/2019
 * @Version	1.0
 ***/

#include "mesh.h"
#include "flash.h"
#include "lightsensor.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"

void Mesh_Init(void)
{
	_elem_index = 0l;
	_my_address = 0;

	TransmissionControl.occupency_count_read = 0;
	TransmissionControl.residing_lumens_read = 0;
	TransmissionControl.occupency_count_new = 0;
	TransmissionControl.residing_lumens_new = 0;
	TransmissionControl.PIR_Filter = 0;
	TransmissionControl.Absence_Counter = 0;
}

void DisplayUpdateFlashValues(void)
{
	TransmissionControl.occupency_count_read = Flash_FetchData(ROOM_OCCUPENCY_COUNT_ID);
	TransmissionControl.residing_lumens_read = Flash_FetchData(LUMENS_RESIDING_VALUE_ID);

	displayPrintf(DISAPLAY_FLASH_OCCUPENCY_COUNT, "Occupancy: %u", TransmissionControl.occupency_count_read);
	displayPrintf(DISPLAY_FLASH_RESIDING_LUMENS, "Last Lumen Sent: %u", TransmissionControl.residing_lumens_read);
}

void initiate_factory_reset(void)
{
	LOG_INFO("\nAt Factory Reset\n");

	gecko_cmd_flash_ps_erase_all();
	// reboot after a small delay
	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(SOFT_DELAY_2_SECS, TIMER_ID_FACTORY_RESET, true));
}

void Set_GattDB_Device_Name(bd_addr *Ble_addr)
{
	char Device_Name[10];

	sprintf(Device_Name, "5823Pub%02x%02x", Ble_addr->addr[1], Ble_addr->addr[0]);

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(Device_Name), (uint8_t*)Device_Name));
}

void switch_node_init(void)
{
  // Initialize mesh lib, up to 8 models
	LOG_INFO("\nAt Swirch Node Init\n");

	mesh_lib_init(malloc, free, 8);
}

void Send_PIRState(uint8_t value)
{
	uint16_t Response = 0;
	static uint8_t TransactionID = 0;
	struct mesh_generic_request Request;

	Request.kind = mesh_generic_request_on_off;
	Request.on_off = value? MESH_GENERIC_ON_OFF_STATE_ON : MESH_GENERIC_ON_OFF_STATE_OFF;

	Response = mesh_lib_generic_client_publish(MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID, _elem_index, TransactionID, &Request, 0, 0, 0);

	if(Response)
	{
		LOG_INFO("\nFailed to send PIR payload. Error Code %x\n", Response);
	}
	else
	{
		LOG_INFO("\nPIR State Sent: %u\n", value);
	}

	TransactionID++;
}

void Send_LightState(void)
{
	static uint8_t TransactionID = 0;
	struct mesh_generic_request Request;
	uint16_t Response = 0;

	Request.kind = mesh_generic_request_level;
	Request.level = (uint16_t)TransmissionControl.residing_lumens_new;

	Response = mesh_lib_generic_client_publish(MESH_GENERIC_LEVEL_CLIENT_MODEL_ID, _elem_index, TransactionID, &Request, 0, 0, 0);

	if(Response)
	{
		LOG_INFO("\nFailed to send Lighting payload. Error Code %x\n", Response);
	}
	else
	{
		LOG_INFO("\nLumens Sent\n");
	}

	TransactionID++;
}

void LowPowerNode_Init(void)
{
	uint16 result = 0;

	result = gecko_cmd_mesh_lpn_init()->result;
	if (result)
	{
		LOG_INFO("\nLPN init failed 0x%x\n", result);
		return;
	}

	// Configure with Minimum friend queue length = 2, Poll timeout = 1 seconds
	result = gecko_cmd_mesh_lpn_configure(FRIEND_QUEUE_LEN, POLL_TIMEOUT)->result;
	if (result)
	{
		LOG_INFO("\nLPN configure failed with result: 0x%x\n", result);
		return;
	}

	result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;
	if (result != 0)
	{
		LOG_INFO("Return Code %x", result);
	}
}

void Mesh_Event_Handler(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	static uint8_t IsProvisioned = 0;
	struct gecko_msg_system_get_bt_address_rsp_t *Ble_addr;
	uint16_t result = 0;

	uint8_t parser = 0;

	//uint8_t datasave = 0;

	handle_gecko_event(evt_id, evt);

	switch(evt_id)
	{
		case gecko_evt_system_boot_id:
			displayPrintf(DISPLAY_ROW_NAME, "Low Power Node-A");

			Ble_addr = gecko_cmd_system_get_bt_address();
			displayPrintf(DISPLAY_ROW_BTADDR,"%x:%x:%x:%x:%x:%x",Ble_addr->address.addr[5],
					Ble_addr->address.addr[4],Ble_addr->address.addr[3],Ble_addr->address.addr[2],
					Ble_addr->address.addr[1],Ble_addr->address.addr[0]);

			if((GPIO_PinInGet(Switch0_port, Switch0_pin) == 0) || (GPIO_PinInGet(Switch1_port, Switch1_pin) == 0))
			{
				displayPrintf(DISPLAY_ROW_ACTION,"FACTORY RESET");
				initiate_factory_reset();
			}
			else
			{
				displayPrintf(DISPLAY_ROW_ACTION, "");
				Set_GattDB_Device_Name(&Ble_addr->address);
			}

			//Flash_SaveData(ROOM_OCCUPENCY_COUNT_ID, &datasave);
			//Flash_SaveData(LUMENS_RESIDING_VALUE_ID, &datasave);

			TransmissionControl.occupency_count_read = Flash_FetchData(ROOM_OCCUPENCY_COUNT_ID);
			TransmissionControl.residing_lumens_read = Flash_FetchData(LUMENS_RESIDING_VALUE_ID);

			displayPrintf(DISAPLAY_FLASH_OCCUPENCY_COUNT, "Occupancy: %u", TransmissionControl.occupency_count_read);
			displayPrintf(DISPLAY_FLASH_RESIDING_LUMENS, "Last Lumen Sent: %u", TransmissionControl.residing_lumens_read);

			break;

		case gecko_evt_hardware_soft_timer_id:
			switch (evt->data.evt_hardware_soft_timer.handle)
			{
				case TIMER_ID_FACTORY_RESET:
					LOG_INFO("\nAt Soft Timer - Timer Factory Reset\n");
					gecko_cmd_system_reset(0);
					break;

				case TIMER_ID_FRIEND_FIND:
					result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;
					if (result != 0)
					{
						LOG_INFO("Return Code %x", result);
					}
					break;

			}
			break;

		case gecko_evt_mesh_node_initialized_id:
			LOG_INFO("\nNode Initialized\n");

			struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

			if (pData->provisioned)
			{
				LOG_INFO("\nNode is already Provisioned. Address:%x, ivi:%ld\n", pData->address, pData->ivi);
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");

				_my_address = pData->address;
				_elem_index = 0;

				BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_generic_client_init());

				switch_node_init();

				LowPowerNode_Init();

				/* Enable PIR as GPIO Interrupt */
				ConfigurePIRInterrupt();

				IsProvisioned = 1;
			}
			else
			{
				LOG_INFO("\nNode is Unprovisioned\n");

				LOG_INFO("Starting Unprovisioned Beaconing...\n");
			}
			break;

		case gecko_evt_mesh_node_provisioning_started_id:
			LOG_INFO("\nStarted Provisioning\n");
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioning");
			break;

		case gecko_evt_mesh_node_provisioned_id:
			_elem_index = 0;

			BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_generic_client_init());

			switch_node_init();
			LOG_INFO("\nNode Provisioned, got address=%x\n", evt->data.evt_mesh_node_provisioned.address);
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");

			/* Enable PIR as GPIO Interrupt */
			ConfigurePIRInterrupt();

			IsProvisioned = 1;
			break;

		case gecko_evt_mesh_node_provisioning_failed_id:
			LOG_INFO("\nProvisioning Failed, code %x\n", evt->data.evt_mesh_node_provisioning_failed.result);
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioning Fail");
			break;

		case gecko_evt_le_connection_opened_id:
			LOG_INFO("\nConnection Opened\n");
			displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
			gecko_cmd_mesh_lpn_deinit();
			break;

		case gecko_evt_le_connection_closed_id:
			LOG_INFO("\nConnection Closed\n");
			displayPrintf(DISPLAY_ROW_CONNECTION, "");
			LowPowerNode_Init();
			break;

		case gecko_evt_mesh_node_reset_id:
			LOG_INFO("\nAt Node Reset\n");
			initiate_factory_reset();
			break;

		case gecko_evt_mesh_lpn_friendship_established_id:
			displayPrintf(DISPLAY_ROW_FRIENDSTATUS, "Friend Connected");
			LOG_INFO("\nFriendship Established\n");
		    break;

		case gecko_evt_mesh_lpn_friendship_failed_id:
			displayPrintf(DISPLAY_ROW_FRIENDSTATUS, "");
			LOG_INFO("\nFailed to establish Friendship\n");
			gecko_cmd_hardware_set_soft_timer(SOFT_DELAY_2_SECS, TIMER_ID_FRIEND_FIND, true);
			break;

		case gecko_evt_mesh_lpn_friendship_terminated_id:
			displayPrintf(DISPLAY_ROW_FRIENDSTATUS, "");
			LOG_INFO("\nFriendship Terminated\n");
			gecko_cmd_hardware_set_soft_timer(SOFT_DELAY_2_SECS, TIMER_ID_FRIEND_FIND, true);
			break;

		case gecko_evt_system_external_signal_id:
			if(IsEventSet(evt->data.evt_system_external_signal.extsignals, MOTION_DETECTED))
			{
				if(GPIO_PinInGet(PIR_port, PIR_pin) == 0)
				{
					TransmissionControl.PIR_Filter++;
					LOG_INFO("\nMotion Detected\n");
				}
				else
				{
					LOG_INFO("\nRoom Empty\n");
				}
			}

			//Register Room Occupied only if PIR is triggered at-least half the given filter count
			if(TransmissionControl.PIR_Filter >= (PIR_FILTER_LEN/2))
			{
				LOG_INFO("\nMotion Detected\n");
				displayPrintf(DISPLAY_PIR_STATE,"Room Occupied");

				//If activity detected after being inactive for long time just send Room Occupied
				if(TransmissionControl.Absence_Counter >= ABSENCE_MARK_THREHOLD)
				{
					//Publish multiple times to account for missing packets
					for(parser = 0 ; parser <= PIR_FILTER_LEN/2 ; parser++)
					{
						Send_PIRState(ROOM_OCCUPIED);
						displayPrintf(DISPLAY_ROW_PUBLISHING, "Pub : Occupied");
					}
				}

				//Reset Absence Counter
				TransmissionControl.Absence_Counter = 0;
			}
			else
			{
				//Send only if provisioned
				if(IsProvisioned)
				{
					LOG_INFO("\nRoom Empty\n");
					displayPrintf(DISPLAY_ROW_PUBLISHING, "");
					displayPrintf(DISPLAY_PIR_STATE,"Room Empty");

					//Track PIR is inactive period
					TransmissionControl.Absence_Counter++;
				}
			}

			if(IsEventSet(evt->data.evt_system_external_signal.extsignals, TIMER_EXPIRED))
			{
				displayUpdate();
			}

			//Start Light Sensor State Machine Only if Motion Detected
			if(TransmissionControl.PIR_Filter >= (PIR_FILTER_LEN/2))
			{
				LightSensor_StateMachine(evt->data.evt_system_external_signal.extsignals);
			}

			//If PIR is active for more than 30 second, publish room is empty
			if(TransmissionControl.Absence_Counter >= ABSENCE_MARK_THREHOLD)
			{
				//Publish at 5 second interval
				if((TransmissionControl.Absence_Counter % ABSENCE_PUBLISH_DELAY) == 0)
				{
					Send_PIRState(ROOM_EMPTY);
					displayPrintf(DISPLAY_ROW_PUBLISHING, "Pub : Empty");
				}
			}

			break;
	}
}


