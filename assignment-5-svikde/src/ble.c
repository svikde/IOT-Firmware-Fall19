/***
 * @File	ble.c
 * @Brief	Contains functions definitions pertaining to BLE Operations.
 * @Author	Souvik De
 * @Date	10/04/2019
 * @Version	1.0
 ***/

#include "ble.h"

void GeckoUpdate(struct gecko_cmd_packet* evt)
{
	int rssi_val = 0;
	static uint8_t con_handle = 0;

	gecko_update(evt);
    switch (BGLIB_MSG_ID(evt->header))
    {
    	case gecko_evt_system_boot_id:
    		//Set Power to 0dB on reset
    		gecko_cmd_system_set_tx_power(TX_POWER_0DB);
			//Set Advertising Min and Max to 250mS (250 * 1.6 = 400)
			gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0);
			/* Start general advertising and enable connections.*/
			gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
    		break;

    	case gecko_evt_le_connection_opened_id:
    		con_handle = evt->data.evt_le_connection_opened.connection;
    		//Connection Interval min & max to 75mS (75/1.25=60) and Slave Latency timing to 300mS
			gecko_cmd_le_connection_set_timing_parameters(con_handle,60,60,3,600,0,0);
			//Enable Underflow Interrupt
			LETIMER_IntEnable(LETIMER0, LETIMER_IFC_UF);
    		break;

    	case gecko_evt_le_connection_closed_id:
    		//Set Power to 0dB when connection disconnected
			gecko_cmd_system_set_tx_power(TX_POWER_0DB);
    		//Disable Interrupt when connection disconnected
    		LETIMER_IntDisable(LETIMER0, LETIMER_IFC_UF);
    		con_handle = 0;
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
    		Change_State(evt->data.evt_system_external_signal.extsignals);
    		gecko_cmd_le_connection_get_rssi(con_handle);
    		break;
    }
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

	gecko_cmd_gatt_server_send_characteristic_notification(
	0xFF, gattdb_temperature_measurement, 5, htmTempBuffer);
}
