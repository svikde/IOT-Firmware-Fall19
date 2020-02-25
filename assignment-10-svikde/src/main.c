#include <stdbool.h>
#include "native_gecko.h"
#include "log.h"

#include "cmu.h"
#include "log.h"
#include "mesh.h"
#include "gpio.h"
#include "letimer.h"
#include "display.h"

extern void gecko_main_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
/*extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);*/

int main(void)
{
	// Initialize stack
	gecko_main_init();

	logInit();

	LETimer0_Interrupt_Init();

	displayInit();

	ConfigureSwitchInterrupt();

	Mesh_Init();

	/* Infinite loop */
	while (1)
	{
		struct gecko_cmd_packet *evt = gecko_wait_event();

		bool pass = mesh_bgapi_listener(evt);

		if (pass)
		{
			Mesh_Event_Handler(BGLIB_MSG_ID(evt->header), evt);
			//handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
		}
	}
}
