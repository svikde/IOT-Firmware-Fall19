/***
 * @File	mesh.c
 * @Brief	Contains functions definitions pertaining to BLE Mesh Operations.
 * @Author	Souvik De
 * @Date	10/04/2019
 * @Version	1.0
 ***/

#include "mesh.h"
#include "mesh_generic_model_capi_types.h"

void Mesh_Init(void)
{
	_elem_index = 0l;
	_my_address = 0;

	ButtonState.onoff_current = 0;
	ButtonState.onoff_target = 0;
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

	#if DEVICE_IS_ONOFF_PUBLISHER

	sprintf(Device_Name, "5823Pub%02x%02x", Ble_addr->addr[1], Ble_addr->addr[0]);

	#else

	sprintf(Device_Name, "5823Sub%02x%02x", Ble_addr->addr[1], Ble_addr->addr[0]);

	#endif

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(Device_Name), (uint8_t*)Device_Name));
}

static void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags)
{
	if (ButtonState.onoff_current == request->on_off)
	{
		LOG_INFO("\nRequest for current state received; no op\n");
	}
	else
	{
		LOG_INFO("\nButton State : %s\n", request->on_off ? "Pressed" : "Released");
		if(request->on_off)
			displayPrintf(DISPLAY_ROW_CLIENTADDR,"Button Pressed");
		else
			displayPrintf(DISPLAY_ROW_CLIENTADDR,"Button Released");

		if (transition_ms == 0 && delay_ms == 0)
		{
			ButtonState.onoff_current = request->on_off;
			ButtonState.onoff_target = request->on_off;
		}
	}

	onoff_update_and_publish(element_index, 0);
}

static void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
	if (current->on_off.on != ButtonState.onoff_current)
	{
		LOG_INFO("\nOn-Off state changed %u to %u\n", ButtonState.onoff_current, current->on_off.on);
		ButtonState.onoff_current = current->on_off.on;
	}
	else
	{
		LOG_INFO("\nDummy onoff change - same state as before\n");
	}
}

static errorcode_t onoff_update(uint16_t element_index, uint32_t remaining_ms)
{
	struct mesh_generic_state current, target;

	current.kind = mesh_generic_state_on_off;
	current.on_off.on = ButtonState.onoff_current;

	target.kind = mesh_generic_state_on_off;
	target.on_off.on = ButtonState.onoff_target;

	return mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        element_index,
	                                        &current,
	                                        &target,
	                                        remaining_ms);
}

static errorcode_t onoff_update_and_publish(uint16_t element_index,
                                            uint32_t remaining_ms)
{
  errorcode_t e;

  e = onoff_update(element_index, remaining_ms);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_generic_state_on_off);
  }

  return e;
}

void Init_Models(void)
{
	uint16_t element_index = 0;

	mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
		                                           0,
		                                           onoff_request,
												   onoff_change);
	onoff_update_and_publish(element_index, 0);
}

void switch_node_init(void)
{
  // Initialize mesh lib, up to 8 models
	LOG_INFO("\nAt Swirch Node Init\n");

	#if DEVICE_IS_ONOFF_PUBLISHER

	mesh_lib_init(malloc, free, 8);

	#else

	mesh_lib_init(malloc, free, 9);

	Init_Models();

	#endif
}

void Send_ButtonState(void)
{
	uint8_t button_state = 0;
	uint16_t Response = 0;
	static uint8_t TransactionID = 0;
	struct mesh_generic_request Request;

	button_state = !GPIO_PinInGet(Switch0_port, Switch0_pin);

	displayPrintf(DISPLAY_ROW_CLIENTADDR,"Button State: %u", button_state);

	Request.kind = mesh_generic_request_on_off;
	Request.on_off = button_state? MESH_GENERIC_ON_OFF_STATE_ON : MESH_GENERIC_ON_OFF_STATE_OFF;

	Response = mesh_lib_generic_client_publish(MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID, _elem_index, TransactionID, &Request, 0,0,0);

	if(Response)
	{
		LOG_INFO("\nFailed to send payload. Error Code %x\n", Response);
	}
	else
	{
		LOG_INFO("\nButton State Sent\n");
	}

	TransactionID++;
}

void Mesh_Event_Handler(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	static uint8_t IsSwitchScannedAtBoot = 0;
	struct gecko_msg_system_get_bt_address_rsp_t *Ble_addr;

	handle_gecko_event(evt_id, evt);

	switch(evt_id)
	{
		case gecko_evt_system_boot_id:
			DeviceIsOnOffPublisher()?displayPrintf(DISPLAY_ROW_NAME, "Publisher"):displayPrintf(DISPLAY_ROW_NAME, "Subscriber");

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

			IsSwitchScannedAtBoot = 1;
			break;

		case gecko_evt_hardware_soft_timer_id:
			switch (evt->data.evt_hardware_soft_timer.handle)
			{
				case TIMER_ID_FACTORY_RESET:
					LOG_INFO("\nAt Soft Timer - Timer Factory Reset\n");
					gecko_cmd_system_reset(0);
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

				#if DEVICE_IS_ONOFF_PUBLISHER

				gecko_cmd_mesh_generic_client_init();

				#else

				gecko_cmd_mesh_generic_server_init();

				#endif

				switch_node_init();
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
			#if DEVICE_IS_ONOFF_PUBLISHER

			gecko_cmd_mesh_generic_client_init();

			#else

			gecko_cmd_mesh_generic_server_init();

			#endif
			switch_node_init();
			LOG_INFO("\nNode Provisioned, got address=%x\n", evt->data.evt_mesh_node_provisioned.address);
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");
			break;

		case gecko_evt_mesh_node_provisioning_failed_id:
			LOG_INFO("\nProvisioning Failed, code %x\n", evt->data.evt_mesh_node_provisioning_failed.result);
			displayPrintf(DISPLAY_ROW_ACTION, "Provisioning Fail");
			break;

		case gecko_evt_mesh_generic_server_client_request_id:
			LOG_INFO("\nEvt gecko_evt_mesh_generic_server_client_request_id\n");
			if(DeviceUsesServerModel())
				mesh_lib_generic_server_event_handler(evt);
			break;

		case gecko_evt_mesh_generic_server_state_changed_id:
			if(DeviceUsesServerModel())
				mesh_lib_generic_server_event_handler(evt);
			break;

		case gecko_evt_le_connection_opened_id:
			LOG_INFO("\nConnection Opened\n");
			displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
			break;

		case gecko_evt_le_connection_closed_id:
			LOG_INFO("\nConnection Closed\n");
			displayPrintf(DISPLAY_ROW_CONNECTION, "");
			break;

		case gecko_evt_mesh_node_reset_id:
			LOG_INFO("\nAt Node Reset\n");
			initiate_factory_reset();
			break;

		case gecko_evt_system_external_signal_id:
			if(IsBLEEventSet(evt->data.evt_system_external_signal.extsignals, TIMER_EXPIRED))
			{
				displayUpdate();
			}

			if(IsBLEEventSet(evt->data.evt_system_external_signal.extsignals, SWITCH0_PRESSED))
			{
				if(DeviceUsesClientModel())
				{
					if(IsSwitchScannedAtBoot)
					{
						Send_ButtonState();
					}
				}
			}
			break;
	}
}


