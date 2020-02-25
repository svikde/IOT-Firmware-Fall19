/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>
#include "event.h"
#include "ble.h"
#include <stdint.h>
#include "log.h"

/**
 * TODO: define these.  See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
 * and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
 */
#define	LED0_port				(gpioPortF)	//LED on PORTF
#define LED0_pin				(4)			//LED0 on PF4
#define LED1_port				(gpioPortF)	//LED on PORTF
#define LED1_pin				(5)			//LED1 on PF5
#define Si7021SENSOR_EN_port	(gpioPortD)	//Sensor Enable for I2C Si7021
#define Si7021SENSOR_EN_pin		(15)		//Sensor Enable for I2C Si7021 at PD15

#define LCD_EN_port				(gpioPortD)
#define LCD_EN_pin				(15)
#define Extcomin_port			(gpioPortD)
#define Extcomin_pin			(13)

void gpioInit()
{
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
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
void gpioLed1Toggle()
{
	GPIO_PinOutToggle(LED1_port, LED1_pin);
}

void gpioI2CSensorEnSetOn()
{
	GPIO_PinOutSet(Si7021SENSOR_EN_port,Si7021SENSOR_EN_pin);
}

void gpioI2CSensorEnSetOff()
{
	GPIO_PinOutClear(Si7021SENSOR_EN_port,Si7021SENSOR_EN_pin);
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
	gecko_external_signal(PASSKEY_CONFIRMED);
}

void ConfigureSwitchInterrupt(void)
{
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
