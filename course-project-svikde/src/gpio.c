/***
 * @File	gpio.c
 * @Brief	Contains function definitions pertaining to GPIOs for the Sensors & the Switches.
 * @Author	Souvik De
 * @Date	11/01/2019
 * @Version	1.0
 ***/

#include "gpio.h"
#include "mesh.h"
#include "em_gpio.h"
#include <stdint.h>
#include <string.h>

void gpioEnableDisplay()
{
	GPIO_PinOutSet(LCD_EN_port,LCD_EN_pin);
}

void gpioSetDisplayExtcomin(bool high)
{
	(high)?(GPIO_PinOutSet(Extcomin_port, Extcomin_pin)):(GPIO_PinOutClear(Extcomin_port, Extcomin_pin));
}

void GPIO_EVEN_IRQHandler(void)
{
	uint32_t Flag = GPIO_IntGet();
	GPIO_IntClear(Flag);
	gecko_external_signal(MOTION_DETECTED);
}

void ConfigurePIRInterrupt(void)
{
	GPIO_PinModeSet(PIR_port,PIR_pin, gpioModeInputPull, true);
	GPIO_IntConfig(PIR_port, PIR_pin, true, true, true);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

void ConfigureSwitchInterrupt(void)
{
	GPIO_PinModeSet(Switch1_port,Switch1_pin, gpioModeInput, true);
	GPIO_PinModeSet(Switch0_port,Switch0_pin, gpioModeInput, true);
	/*GPIO_IntConfig(Switch0_port, Switch0_pin, true, true, true);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);*/
}

void EnableLightSensor(void)
{
	GPIO_PinOutSet(LIGHT_SENSOR_PORT, LIGHT_SENSOR_PIN);
}

void DisableLightSensor(void)
{
	GPIO_PinOutClear(LIGHT_SENSOR_PORT, LIGHT_SENSOR_PIN);
}
