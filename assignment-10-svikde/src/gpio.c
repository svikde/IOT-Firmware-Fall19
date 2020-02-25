/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include "mesh.h"
#include "em_gpio.h"
#include <stdint.h>
#include <string.h>

#define	LED0_port gpioPortF
#define LED0_pin	4
#define LED1_port gpioPortF
#define LED1_pin 5

void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);
}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

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
	gecko_external_signal(SWITCH0_PRESSED);
}

void ConfigureSwitchInterrupt(void)
{
	GPIO_PinModeSet(Switch1_port,Switch1_pin, gpioModeInput, true);
	GPIO_PinModeSet(Switch0_port,Switch0_pin, gpioModeInput, true);
	GPIO_IntConfig(Switch0_port, Switch0_pin, true, true, true);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

void DisableSwitchInterrupt(void)
{
	GPIO_PinModeSet(Switch0_port,Switch0_pin, gpioModeDisabled, false);
	GPIO_ExtIntConfig(Switch0_port, Switch0_pin, Switch0_pin, false, true, false);
	NVIC_DisableIRQ(GPIO_EVEN_IRQn);
}
