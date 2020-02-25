/***
 * @File	main.c
 * @Brief	Main File for the Low Power Node-A of the Home Automation Project
 * @Author	Souvik De
 * @Date	11/01/2019
 * @Version	1.0
 ***/

#include <stdbool.h>
#include "native_gecko.h"
#include "log.h"

#include "cmu.h"
#include "log.h"
#include "mesh.h"
#include "gpio.h"
#include "letimer.h"
#include "display.h"
#include "lightsensor.h"

extern void gecko_main_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);

int main(void)
{
	// Initialize stack
	gecko_main_init();

	/* Initialize Logging */
	logInit();

	/* Configure LETimer0 Interrupt */
	LETimer0_Interrupt_Init();

	/* Initialize Display */
	displayInit();

	/* Configure Switch as Inputs */
	ConfigureSwitchInterrupt();

	/* Initialize I2C */
	I2C_init();

	/* Initialize Light Sensor Dependencies */
	Init_LightSensor();

	/* Initialize Mesh dependencies */
	Mesh_Init();

	/* Infinite loop */
	while (1)
	{
		struct gecko_cmd_packet *evt = gecko_wait_event();

		bool pass = mesh_bgapi_listener(evt);

		if (pass)
		{
			Mesh_Event_Handler(BGLIB_MSG_ID(evt->header), evt);
		}
	}
}
