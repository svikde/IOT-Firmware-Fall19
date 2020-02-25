/***
 * @File	lightsensor.h
 * @Brief	Contains declarations pertaining to the Ambient Light Sensors.
 * @Author	Souvik De
 * @Date	12/01/2019
 * @Version	1.0
 ***/

#ifndef SRC_LIGHTSENSOR_H_
#define SRC_LIGHTSENSOR_H_

#include "i2c.h"
#include "mesh.h"

/* Register Addresses of APDS-9301 */
#define CMD_CONTROL_REGISTER		(0x80)
#define CONTROL_REGISTER			(0x03)
#define CMD_DATA0LOW_REGISTER_8		(0x8C)
#define CMD_DATA0HIGH_REGISTER_8	(0x8D)
#define CMD_DATA1LOW_REGISTER_8		(0x8E)
#define CMD_DATA1HIGH_REGISTER_8	(0x8F)

/* Set Max-Min Lumen Values to send (Scaled for PWM of LED) */
#define LUMEN_TRANSM_LOWER_LIMIT	(1)
#define LUMEN_TRANSM_UPPER_LIMIT	(200)

/* Values used to calculate Lumnes in APDS-9301 */
#define LUX_CONST1				(0)
#define LUX_CONST2				(0.50)
#define LUX_CONST3				(0.0304)
#define LUX_CONST4				(0.062)
#define LUX_CONST5				(1.4)
#define LUX_CONST6				(0.61)
#define LUX_CONST7				(0.0224)
#define LUX_CONST8				(0.031)
#define LUX_CONST9				(0.80)
#define LUX_CONST10				(0.0128)
#define LUX_CONST11				(0.0153)
#define LUX_CONST12				(1.30)
#define LUX_CONST13				(0.00146)
#define LUX_CONST14				(.00112)

/* Sequencer States */
#define SEQUENCER_STATE0		(0)
#define SEQUENCER_STATE1		(1)
#define SEQUENCER_STATE2		(2)
#define SEQUENCER_STATE3		(3)
#define SEQUENCER_STATE4		(4)
#define SEQUENCER_STATE5		(5)
#define SEQUENCER_STATE6		(6)

/* Light Sensor State Enumerator */
typedef enum{
	ENABLE_SENSOR,						//Load Power Management On
	POWER_UP_1,							//Power Up Instructions
	POWER_UP_2,							//Power Up Instructions Continued
	POWER_UP_DONE,						//Power Up Acknowledgement
	I2C_WRITE_CMD_DATA0LOW_REGISTER,	//Write to Data-0 Low Register
	I2C_READ_VISIBLE_LUX_LOW,
	I2C_WRITE_CMD_DATA0HIGH_REGISTER,
	I2C_READ_VISIBLE_LUX_HIGH,
	I2C_WRITE_CMD_DATA1LOW_REGISTER,
	I2C_READ_IR_LUX_LOW,
	I2C_WRITE_CMD_DATA1HIGH_REGISTER,
	I2C_READ_IR_LUX_HIGH,
	CALCULATE_LUMENS,					//Calculate Lumens
	SEQUENCER							//Sequences Delay States
}State_t;

State_t next_state;		//Next State Variable

/* Light Sensor Structure */
struct LightSensor
{
	uint8_t cmd;				//To Write Command
	uint8_t data;				//To Write / Read Data
	uint8_t visible_luxlow;
	uint8_t visible_luxhigh;
	uint8_t ir_luxlow;
	uint8_t ir_luxhigh;
	int lux_visiblelight;
	int lux_irlight;
	float lumen;
	State_t current_state;
}light_obj;

/***
 * @Brief	Initializes the Light Sensor Structure.
 *
 * @Param	void
 *
 * @Return	void
 ***/
void Init_LightSensor(void);

/***
 * @Brief	Calculates Luminosity Values from the Visible & IR Lux values as per manufacturer (APDS-9301) Data-sheet.
 * 			Refer to CH1/CH0 values & Sensor LUX Formula in Page-4 of the Data-sheet (https://cdn.sparkfun.com/assets/3/2/c/0/8/AV02-2315EN0.pdf)
 *
 * @Param	(1) Visible Lux Value, (2) IR Lux Value
 *
 * @Return	Lumens
 ***/
float CalculateLumen(int visible_lux, int ir_lux);

/***
 * @Brief	Light Sensor State Machine.
 *
 * @Param	Gecko External Event Interrupt Bitmask
 *
 * @Return	void
 ***/
void LightSensor_StateMachine(uint32_t EventBitmask);

#endif /* SRC_LIGHTSENSOR_H_ */
