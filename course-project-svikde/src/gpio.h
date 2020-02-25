/***
 * @File	gpio.h
 * @Brief	Contains function declarations pertaining to GPIOS for the Sensors & the Switches.
 * @Author	Souvik De
 * @Date	11/01/2019
 * @Version	1.0
 ***/

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>

/* Enable PINs for the Display */
#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1

#define LCD_EN_port				(gpioPortD)
#define LCD_EN_pin				(15)
#define Extcomin_port			(gpioPortD)
#define Extcomin_pin			(13)

/* PORT & PINs for the Switches */
#define Switch0_port			(gpioPortF)
#define Switch0_pin				(6)
#define Switch1_port			(gpioPortF)
#define Switch1_pin				(7)

/* PORT & PINs for the PIR Sensor */
#define PIR_port				(gpioPortD)
#define PIR_pin					(10)

/* PORT & PINs for the Light Sensor */
#define LIGHT_SENSOR_PORT		(gpioPortF)
#define LIGHT_SENSOR_PIN		(3)

/***
 * @Brief	Enable the LCD.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void gpioEnableDisplay();

/***
 * @Brief	Set LCD Pins.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void gpioSetDisplayExtcomin(bool high);

/***
 * @Brief	GPIO EVEN IRQ Handler.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void GPIO_EVEN_IRQHandler(void);

/***
 * @Brief	Configure the PIR sensor as Interrupt.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void ConfigurePIRInterrupt(void);

/***
 * @Brief	Configure Switches as Input (for Factory Reset).
 *
 * @Param	void
 *
 * @Return	void
 ***/
void ConfigureSwitchInterrupt(void);

/***
 * @Brief	Load Power Management On for Light Sensor
 *
 * @Param	void
 *
 * @Return	void
 ***/
void EnableLightSensor(void);

/***
 * @Brief	Load Power Management Off for the Light Sensor.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void DisableLightSensor(void);

#endif /* SRC_GPIO_H_ */
