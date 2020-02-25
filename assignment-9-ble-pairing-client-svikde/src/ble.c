/***
 * @File	ble.c
 * @Brief	Contains functions definitions pertaining to BLE Operations.
 * @Author	Souvik De
 * @Date	10/04/2019
 * @Version	1.0
 ***/

#include "ble.h"

#if DEVICE_IS_BLE_SERVER
void GeckoUpdate(struct gecko_cmd_packet* evt)
{
	int rssi_val = 0;
	static uint8_t con_handle = 0;
	static uint8_t passkey_con_handle = 0;
	struct gecko_msg_system_get_bt_address_rsp_t *Ble_addr;

	gecko_update(evt);
    switch (BGLIB_MSG_ID(evt->header))
    {
    	case gecko_evt_system_boot_id:
    		IsDeviceBonded = false;

    		displayPrintf(DISPLAY_ROW_NAME,"Server");
    		Ble_addr = gecko_cmd_system_get_bt_address();
    		displayPrintf(DISPLAY_ROW_BTADDR,"%x:%x:%x:%x:%x:%x",Ble_addr->address.addr[5],
    				Ble_addr->address.addr[4],Ble_addr->address.addr[3],Ble_addr->address.addr[2],
					Ble_addr->address.addr[1],Ble_addr->address.addr[0]);
    		displayPrintf(DISPLAY_ROW_CONNECTION,"Advertising");
    		displayUpdate();

    		/* Delete all bonding & force pairing process */
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());

    		/* Set to allow device to accept new bonding */
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_set_bondable_mode(BONDINGS_ALLOWED));

    		/* Configure security requirements and I/O capabilities of the system */
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_configure(MITM_FLAG | ENCRIPT_FLAG | SECURE_CON_FLAG | BOND_REQ_FLAG, sm_io_capability_displayyesno));

    		//Set Power to 0dB on reset
    		gecko_cmd_system_set_tx_power(TX_POWER_0DB);
			//Set Advertising Min and Max to 250mS (250 * 1.6 = 400)
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, ADV_INTVL_MIN, ADV_INTVL_MAX, 0, 0));
			/* Start general advertising and enable connections.*/
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));

			/* Enable PB0 Interrupt */
			ConfigureSwitchInterrupt();
    		break;

    	case gecko_evt_sm_confirm_bonding_id:
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_bonding_confirm(evt->data.evt_sm_confirm_bonding.connection, ACCEPT_BOND_REQ));
    		break;

    	case gecko_evt_sm_confirm_passkey_id:
    		passkey_con_handle = evt->data.evt_sm_confirm_passkey.connection;
    		displayPrintf(DISPLAY_ROW_PASSKEY,"%lu",evt->data.evt_sm_confirm_passkey.passkey);
    		displayPrintf(DISPLAY_ROW_ACTION,"Confirm with PB0");
    		displayUpdate();
    		break;

    	case gecko_evt_sm_bonded_id:
    		IsDeviceBonded = true;
    		displayPrintf(DISPLAY_ROW_CONNECTION,"Bonded");
    		displayPrintf(DISPLAY_ROW_PASSKEY,"");
			displayPrintf(DISPLAY_ROW_ACTION,"");
			displayUpdate();
    		break;

    	case gecko_evt_sm_bonding_failed_id:
    		IsDeviceBonded = false;
    		displayPrintf(DISPLAY_ROW_CONNECTION,"Bonding Failed");
			displayPrintf(DISPLAY_ROW_PASSKEY,"");
			displayPrintf(DISPLAY_ROW_ACTION,"");
			displayUpdate();
			LOG_INFO("\nBonding Failed\n");
    		break;

    	case gecko_evt_le_connection_opened_id:
    		displayPrintf(DISPLAY_ROW_CONNECTION,"Connected");
    		displayUpdate();
    		con_handle = evt->data.evt_le_connection_opened.connection;
    		//Connection Interval min & max to 75mS (75/1.25=60) and Slave Latency timing to 300mS
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_timing_parameters(con_handle,CON_INTVL_MIN,CON_INTVL_MAX,SLAVE_LATENCY,TIMEOUT,0,0));
			//Enable Underflow Interrupt
			LETIMER_IntEnable(LETIMER0, LETIMER_IFC_UF);
    		break;

    	case gecko_evt_le_connection_closed_id:
    		IsDeviceBonded = false;
    		displayPrintf(DISPLAY_ROW_CONNECTION,"Advertising");
    		displayPrintf(DISPLAY_ROW_CLIENTADDR,"");
    		displayPrintf(DISPLAY_ROW_TEMPVALUE,"");
			displayUpdate();

			BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());

    		//Set Power to 0dB when connection disconnected
			gecko_cmd_system_set_tx_power(TX_POWER_0DB);
    		//Disable Interrupt when connection disconnected
    		LETIMER_IntDisable(LETIMER0, LETIMER_IFC_UF);
    		con_handle = 0;

    		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
    		break;

    	case gecko_evt_le_connection_rssi_id:
    		rssi_val = evt->data.evt_le_connection_rssi.rssi;

    		if (rssi_val > RSSI_NEG35DB)
    			gecko_cmd_system_set_tx_power(TX_POWER_MIN);
    		else if ((rssi_val <= RSSI_NEG35DB) && (rssi_val > RSSI_NEG45DB))
    			gecko_cmd_system_set_tx_power(TX_POWER_NEG20DB);
    		else if ((rssi_val <= RSSI_NEG45DB) && (rssi_val > RSSI_NEG55DB))
    			gecko_cmd_system_set_tx_power(TX_POWER_NEG15DB);
    		else if ((rssi_val <= RSSI_NEG55DB) && (rssi_val > RSSI_NEG65DB))
    			gecko_cmd_system_set_tx_power(TX_POWER_NEG5DB);
    		else if ((rssi_val <= RSSI_NEG65DB) && (rssi_val > RSSI_NEG75DB))
    			gecko_cmd_system_set_tx_power(TX_POWER_0DB);
    		else if ((rssi_val <= RSSI_NEG75DB) && (rssi_val > RSSI_NEG85DB))
    			gecko_cmd_system_set_tx_power(TX_POWER_5DB);
    		else
    			gecko_cmd_system_set_tx_power(TX_POWER_MAX);

    		break;

    	case gecko_evt_system_external_signal_id:
    		if(IsBLEEventSet(evt->data.evt_system_external_signal.extsignals, PASSKEY_CONFIRMED))
    		{
    			if(IsDeviceBonded)
    			{
    				Transmit_ButtonState();
    			}
    			else
    			{
    				BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_passkey_confirm(passkey_con_handle, true));
    			}
    		}
    		else
    		{
    			Change_State(evt->data.evt_system_external_signal.extsignals);
    			gecko_cmd_le_connection_get_rssi(con_handle);
    		}
    		break;
    }
}

void Transmit_ButtonState(void)
{
	uint8_t button_state = 0;

	button_state = !GPIO_PinInGet(Switch0_port, Switch0_pin);

	displayPrintf(DISPLAY_ROW_CLIENTADDR,"Button State: %u", button_state);
	displayUpdate();

	LOG_INFO("Button State = %u\n", button_state);
	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_button_state, 1, &button_state));
}

void TransmitTemperature(float TemperatureFloat)
{
	uint8_t htmTempBuffer[5]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
	uint8_t flags = 0x00;   /* HTM flags set as 0 for Celsius, no time stamp and no temperature type. */
	uint32_t temperature;   /* Stores the temperature data read from the sensor in the correct format */
	uint8_t *p = htmTempBuffer; /* Pointer to HTM temperature buffer needed for converting values to bitstream. */

	/* Convert flags to bitstream and append them in the HTM temperature data buffer (htmTempBuffer) */
	UINT8_TO_BITSTREAM(p, flags);

	/* Convert sensor data to correct temperature format */
	temperature = FLT_TO_UINT32((uint32_t)(TemperatureFloat * 1000), -3);
	/* Convert temperature to bitstream and place it in the HTM temperature data buffer (htmTempBuffer) */
	UINT32_TO_BITSTREAM(p, temperature);

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_temperature_measurement, 5, htmTempBuffer));
}

#else

float gattUint32ToFloat(const uint8_t *value_start_little_endian)
{
	int8_t exponent = (int8_t)value_start_little_endian[4];
	uint32_t mantissa = value_start_little_endian[1] +
			(((uint32_t)value_start_little_endian[2]) << 8) +
			(((uint32_t)value_start_little_endian[3]) << 16);

	return (float)mantissa*pow(10,exponent);
}

void GeckoUpdate(struct gecko_cmd_packet* evt)
{
	static uint8_t con_handle = 0;
	static uint8_t passkey_con_handle = 0;
	static uint32_t gatt_service_handle = 0;
	static uint16_t gatt_characteristic_handle = 0;
	static uint32_t gatt_service_handle_btn = 0;
	static uint16_t gatt_characteristic_handle_btn = 0;
	static float temperature = 0;
	uint8_t *button_state;

	/* Client Address */
	struct gecko_msg_system_get_bt_address_rsp_t *client_addr;

	/* Server Address */
	const uint8_t server_addr[] = SERVER_BT_ADDRESS;

	/* Service UUID Health Thermometer */
	uint8array HealthTermometerUUID;

	/* Characteristics UUID Health Thermometer */
	uint8array TemperatureMeasurementUUID;

	/* Service UUID Button Press */
	uint8_t ButtonServiceUUID[BTN_SERV_UUID_LEN];

	/* Characteristics UUID Button Press */
	uint8_t ButtonStateUUID[BTN_SERV_UUID_LEN];

	/* Service HT */
	HealthTermometerUUID.data[0] = HLTH_THERM_UUID_L;
	HealthTermometerUUID.data[1] = HLTH_THERM_UUID_H;
	HealthTermometerUUID.len = HLTH_THERM_UUID_LEN;

	/* Characteristic HT */
	TemperatureMeasurementUUID.data[0] = TEMP_MESMT_UUID_L;
	TemperatureMeasurementUUID.data[1] = TEMP_MESMT_UUID_H;
	TemperatureMeasurementUUID.len = TEMP_MESMT_UUID_LEN;

	/* Service Button */
	ButtonServiceUUID[0] = BTN_SERV_UUID_0;
	ButtonServiceUUID[1] = BTN_SERV_UUID_1;
	ButtonServiceUUID[2] = BTN_SERV_UUID_2;
	ButtonServiceUUID[3] = BTN_SERV_UUID_3;
	ButtonServiceUUID[4] = BTN_SERV_UUID_4;
	ButtonServiceUUID[5] = BTN_SERV_UUID_5;
	ButtonServiceUUID[6] = BTN_SERV_UUID_6;
	ButtonServiceUUID[7] = BTN_SERV_UUID_7;
	ButtonServiceUUID[8] = BTN_SERV_UUID_8;
	ButtonServiceUUID[9] = BTN_SERV_UUID_9;
	ButtonServiceUUID[10] = BTN_SERV_UUID_10;
	ButtonServiceUUID[11] = BTN_SERV_UUID_11;
	ButtonServiceUUID[12] = BTN_SERV_UUID_12;
	ButtonServiceUUID[13] = BTN_SERV_UUID_13;
	ButtonServiceUUID[14] = BTN_SERV_UUID_14;
	ButtonServiceUUID[15] = BTN_SERV_UUID_15;
	//ButtonServiceUUID.len = BTN_SERV_UUID_LEN;

	/* Characteristics - Button */
	ButtonStateUUID[0] = BTN_SERV_UUID_0;
	ButtonStateUUID[1] = BTN_SERV_UUID_1;
	ButtonStateUUID[2] = BTN_SERV_UUID_2;
	ButtonStateUUID[3] = BTN_SERV_UUID_3;
	ButtonStateUUID[4] = BTN_SERV_UUID_4;
	ButtonStateUUID[5] = BTN_SERV_UUID_5;
	ButtonStateUUID[6] = BTN_SERV_UUID_6;
	ButtonStateUUID[7] = BTN_SERV_UUID_7;
	ButtonStateUUID[8] = BTN_SERV_UUID_8;
	ButtonStateUUID[9] = BTN_SERV_UUID_9;
	ButtonStateUUID[10] = BTN_SERV_UUID_10;
	ButtonStateUUID[11] = BTN_SERV_UUID_11;
	ButtonStateUUID[12] = BTN_CHAR_UUID_12;
	ButtonStateUUID[13] = BTN_SERV_UUID_13;
	ButtonStateUUID[14] = BTN_SERV_UUID_14;
	ButtonStateUUID[15] = BTN_SERV_UUID_15;
	//ButtonStateUUID.len = BTN_CHAR_UUID_LEN;

	/* State Machine */
	switch(BGLIB_MSG_ID(evt->header))
	{
		case gecko_evt_system_boot_id:
			displayPrintf(DISPLAY_ROW_NAME,"Client");
			client_addr = gecko_cmd_system_get_bt_address();
			displayPrintf(DISPLAY_ROW_BTADDR,"%x:%x:%x:%x:%x:%x",client_addr->address.addr[5],
					client_addr->address.addr[4],client_addr->address.addr[3],client_addr->address.addr[2],
					client_addr->address.addr[1],client_addr->address.addr[0]);
			displayPrintf(DISPLAY_ROW_BTADDR2,"%x:%x:%x:%x:%x:%x", server_addr[5], server_addr[4],
					server_addr[3], server_addr[2], server_addr[1], server_addr[0]);
			displayPrintf(DISPLAY_ROW_CONNECTION,"Discovering");
			displayUpdate();

			/* Delete all bonding & force pairing process */
			BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());

			/* Set to allow device to accept new bonding */
			BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_set_bondable_mode(BONDINGS_ALLOWED));

			/* Configure security requirements and I/O capabilities of the system */
			BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_configure(MITM_FLAG | ENCRIPT_FLAG | SECURE_CON_FLAG | BOND_REQ_FLAG, sm_io_capability_displayyesno));

			/* Enable PB0 Interrupt */
			ConfigureSwitchInterrupt();

			//Start Discovery
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_general_discoverable));
			break;

		case gecko_evt_le_gap_scan_response_id:
			//Connect Client to Server based on server address defined in macro
			if(memcmp(&evt->data.evt_le_gap_scan_response.address.addr, &server_addr, sizeof(server_addr)) == 0)
			{
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_connect(evt->data.evt_le_gap_scan_response.address, evt->data.evt_le_gap_scan_response.address_type, le_gap_phy_1m));
			}
			break;

    	case gecko_evt_sm_confirm_passkey_id:
    		passkey_con_handle = evt->data.evt_sm_confirm_passkey.connection;
    		displayPrintf(DISPLAY_ROW_PASSKEY,"%lu",evt->data.evt_sm_confirm_passkey.passkey);
    		displayPrintf(DISPLAY_ROW_ACTION,"Confirm with PB0");
    		displayUpdate();
    		break;

    	case gecko_evt_sm_bonded_id:
    		if(evt->data.evt_sm_bonded.bonding == 0xFF)
			{
				LOG_INFO("\nPained but could not bond successfully\n");
			}
			else
			{
				IsDeviceBonded = true;
				displayPrintf(DISPLAY_ROW_CONNECTION,"Bonded");
				displayPrintf(DISPLAY_ROW_PASSKEY,"");
				displayPrintf(DISPLAY_ROW_ACTION,"");
				displayUpdate();

				BT_Present_State = BT_DISCOVER_BTN_SERVICES;

				BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_primary_services_by_uuid(evt->data.evt_sm_bonded.connection, BTN_SERV_UUID_LEN, ButtonServiceUUID));
			}
    		break;

    	case gecko_evt_sm_bonding_failed_id:
    		IsDeviceBonded = false;
    		displayPrintf(DISPLAY_ROW_CONNECTION,"Bonding Failed");
			displayPrintf(DISPLAY_ROW_PASSKEY,"");
			displayPrintf(DISPLAY_ROW_ACTION,"");
			displayUpdate();
    		break;

		case gecko_evt_le_connection_opened_id:
			BT_Present_State = BT_DISCOVER_HT_SERVICES;
			displayPrintf(DISPLAY_ROW_CONNECTION,"Connected");
			displayUpdate();

			//Get connection handle
			con_handle = evt->data.evt_le_connection_opened.connection;

			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_timing_parameters(con_handle,CON_INTVL_MIN,CON_INTVL_MAX,SLAVE_LATENCY,TIMEOUT,0,0));

			BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_increase_security(con_handle));

			//Discover GATT services
			BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_primary_services_by_uuid(con_handle, HealthTermometerUUID.len, HealthTermometerUUID.data));
			break;

		case gecko_evt_gatt_service_id:
			if((evt->data.evt_gatt_service.uuid.data[1] == HealthTermometerUUID.data[1]) && (evt->data.evt_gatt_service.uuid.data[0] == HealthTermometerUUID.data[0]))
			{
				//Get Service Handle
				gatt_service_handle = evt->data.evt_gatt_service.service;
			}
			else
			{
				gatt_service_handle_btn = evt->data.evt_gatt_service.service;
			}
			break;

		//Sub State Machine
		case gecko_evt_gatt_procedure_completed_id:

			switch(BT_Present_State)
			{
				case BT_DISCOVER_HT_SERVICES:
					if(evt->data.evt_gatt_procedure_completed.result == 0)
					{
						//Discover Characteristics Command
						BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_characteristics_by_uuid(con_handle, gatt_service_handle, TemperatureMeasurementUUID.len, TemperatureMeasurementUUID.data));
						BT_Next_State = BT_DISCOVER_HT_CHARECTERISCTICS;
					}
					else
					{
						LOG_ERROR("Failed to Discover Characteristics");
					}
					break;

				case BT_DISCOVER_HT_CHARECTERISCTICS:
					if(evt->data.evt_gatt_procedure_completed.result == 0)
					{

						BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_set_characteristic_notification(con_handle, gatt_characteristic_handle, gatt_indication));
						BT_Next_State = BT_SEND_NOTIFICATION;
					}
					else
					{
						LOG_ERROR("Failed to send Temperature Notification");
					}
					break;

				case BT_DISCOVER_BTN_SERVICES:
					if(evt->data.evt_gatt_procedure_completed.result == 0)
					{
						//Discover Characteristics Command
						BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_characteristics_by_uuid(con_handle, gatt_service_handle_btn, BTN_SERV_UUID_LEN, ButtonStateUUID));
						BT_Next_State = BT_DISCOVER_BTN_CHARECTERISCTICS;
					}
					else
					{
						LOG_ERROR("Failed to Discover Characteristics for Button");
					}
					break;

				case BT_DISCOVER_BTN_CHARECTERISCTICS:
					if(evt->data.evt_gatt_procedure_completed.result == 0)
					{

						BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_set_characteristic_notification(con_handle, gatt_characteristic_handle_btn, gatt_indication));
						BT_Next_State = BT_SEND_NOTIFICATION;
					}
					else
					{
						LOG_ERROR("Failed to send Button Notification");
					}
					break;

				case BT_SEND_NOTIFICATION:
					if(evt->data.evt_gatt_procedure_completed.result == 0)
					{
						//Suspend sub state machine in dummy state to wait for notification forever
						BT_Next_State = BT_WAIT_FOR_NOTIFICATION;
					}
					else
					{
						LOG_ERROR("Failed to send Characteristic Confirmation");
					}
					break;
			}

			if(BT_Present_State != BT_Next_State)
			{
				BT_Present_State = BT_Next_State;
			}
			break;

		case gecko_evt_gatt_characteristic_id:
			if((evt->data.evt_gatt_characteristic.uuid.data[1] == TemperatureMeasurementUUID.data[1]) && (evt->data.evt_gatt_characteristic.uuid.data[0] == TemperatureMeasurementUUID.data[0]))
			{
				//Get characteristics handle
				gatt_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
			}
			else
			{
				gatt_characteristic_handle_btn = evt->data.evt_gatt_characteristic.characteristic;
			}
			break;

		case gecko_evt_gatt_characteristic_value_id:
			if(evt->data.evt_gatt_characteristic_value.characteristic == gatt_characteristic_handle)
			{
				//Confirm characteristics
				gecko_cmd_gatt_send_characteristic_confirmation(con_handle);
				displayPrintf(DISPLAY_ROW_CONNECTION,"Handling Indications");
				//Convert received temperature into float value
				temperature = gattUint32ToFloat(evt->data.evt_gatt_characteristic_value.value.data);
				displayPrintf(DISPLAY_ROW_TEMPVALUE,"Temperature:%.2f",temperature);
				displayUpdate();
			}
			else
			{
				gecko_cmd_gatt_send_characteristic_confirmation(con_handle);
				displayPrintf(DISPLAY_ROW_CONNECTION,"Handling Indications");
				button_state = evt->data.evt_gatt_characteristic_value.value.data;

				LOG_INFO("Button State = %u", *button_state);

				if(*button_state)
					displayPrintf(DISPLAY_ROW_ACTION,"Button Pressed");
				else
					displayPrintf(DISPLAY_ROW_ACTION,"Button Released");

				displayUpdate();
			}
			break;

		case gecko_evt_system_external_signal_id:
			if(IsBLEEventSet(evt->data.evt_system_external_signal.extsignals, PASSKEY_CONFIRMED))
			{
				BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_passkey_confirm(passkey_con_handle, true));
			}
			break;

		case gecko_evt_le_connection_closed_id:
			BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());
			displayPrintf(DISPLAY_ROW_CONNECTION,"Discovering");
			displayPrintf(DISPLAY_ROW_TEMPVALUE,"");
			displayUpdate();

			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_general_discoverable));
			break;
	}
}

#endif
