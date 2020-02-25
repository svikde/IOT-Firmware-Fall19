/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

#include "em_letimer.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"		// Oscillator Type Enumerators

#include "em_core.h"
#include "sleep.h"

#include "letimer.h"
#include "cmu.h"

#include "ble.h"
#include "i2c.h"
#include "log.h"
#include "event.h"
#include "display.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

#include "em_device.h"
#include "em_chip.h"
#include "gpio.h"
#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif


uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};

/*static void delayApproxOneSecond(void)
{*/
	/**
	 * Wait loops are a bad idea in general!  Don't copy this code in future assignments!
	 * We'll discuss how to do this a better way in the next assignment.
	 */
	/*volatile int i;
	for (i = 0; i < 35000; ) {
		  i=i+1;
	}
}*/

int main(void)
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  gpioInit();

  gpioLed1SetOff();

  /* Control Sleep Mode using this ENUM */
  const SLEEP_EnergyMode_t sleep_mode_blocked = sleepEM3;

  struct gecko_cmd_packet *evt;

  // Initialize stack
  gecko_init(&config);

  /* Configure CMU, Interrupt associated with LETimer0 and Sleep Mode */
  LETimer0_Interrupt_Init(sleep_mode_blocked);

  //Initialize the logger
  logInit();

  //Initialize I2C
  I2C_init();

  //Initialize Events
  Event_Init();

  displayInit();

  /* Infinite loop */
  while (1)
  {
	  evt = gecko_wait_event();
	  GeckoUpdate(evt);
  }
}
