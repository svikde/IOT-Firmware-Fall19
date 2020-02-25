/*
 * gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>

#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1

#define Switch0_port			(gpioPortF)
#define Switch0_pin				(6)

void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpioLed1Toggle();
void gpioI2CSensorEnSetOn();
void gpioI2CSensorEnSetOff();

void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);

void PGPIO_EVEN_IRQHandler(void);
void ConfigureSwitchInterrupt(void);
void DisableSwitchInterrupt(void);

#endif /* SRC_GPIO_H_ */
