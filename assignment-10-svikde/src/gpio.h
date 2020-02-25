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

#define LCD_EN_port				(gpioPortD)
#define LCD_EN_pin				(15)
#define Extcomin_port			(gpioPortD)
#define Extcomin_pin			(13)

#define Switch0_port			(gpioPortF)
#define Switch0_pin				(6)
#define Switch1_port			(gpioPortF)
#define Switch1_pin				(7)

void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();

void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);

void GPIO_EVEN_IRQHandler(void);
void ConfigureSwitchInterrupt(void);
void DisableSwitchInterrupt(void);

#endif /* SRC_GPIO_H_ */
