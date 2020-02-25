/***
 * @File	letimer.h
 * @Brief	Contains macros and function declarations
 * 			pertaining to LETimer0. The following functions
 * 			are used to configure the LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	3.0
 ***/

#ifndef SRC_LETIMER_H_
#define SRC_LETIMER_H_

#include "em_letimer.h"
#include "em_core.h"
#include "sleep.h"
#include "gpio.h"
#include "cmu.h"

#define LEDONTIME_MS		(175)								//On time of LED in MS
#define LED_BLKPERIOD_MS	(2250)								//Blink Period of LED in MS
#define FCPU_LFXO			(32768)								//LFXO Frequency in Hz
#define FCPU_ULFRCO			(1000)								//ULFRCO Frequency in Hz

#define INT_PERIOD_MS		(3000)								//Interrupt Period in ms

/* To Calculate Comparator Values - Macro referenced from Lecture-5 */
#define MS2COMP_VALUE(ms_value, fcpu, prescaler)	(uint16_t)(((ms_value) *((fcpu)/(prescaler)))/1000)

/*Even Bit mask for LETIMER0 Interrupt*/
volatile uint32_t Interrupt_Event_Bitmask;

/* Indicates if designated delay is achieved */
volatile uint8_t IsDelayDone;

/***
 * @Brief	Initializes LETimer0 by setting the inherent
 * 			structure variable and setting comparator values.
 * 			Also involves configurations for interrupt flags.
 *
 *
 * 	@Param	[SLEEP_EnergyMode_t] used to pass type of sleep mode
 * 			to conditionally alter comparator contents.
 *
 *  @Return	Void
 ***/
void LETimer0_Init(const SLEEP_EnergyMode_t Energy_Mode);

/***
 * @Brief	Initializes Clock sources, Oscillators, LETimer0
 * 			Interrupt Registers & Sleep Mode for LETimer0
 *
 * 	@Param	[SLEEP_EnergyMode_t] used to pass type of sleep mode
 * 			to conditionally alter comparator contents.
 *
 * 	@Return	Void
 ***/
void LETimer0_Interrupt_Init(const SLEEP_EnergyMode_t Energy_Mode);

/***
 * @Brief	Used to configure sleep modes for LETimer0
 *
 * @Param	[SLEEP_EnergyMode_t] used to pass type of sleep mode
 * 			to conditionally alter comparator contents.
 *
 * @Return Void
 ***/
void LETimer0_EMBlockStart(const SLEEP_EnergyMode_t Energy_Mode);

/***
 * @Brief	Use to provide delay using LETIMER0
 *
 * @Param	Delay duration in micro-seconds
 *
 * @Return Void
 ***/
void timerWaitUs(uint32_t us_wait);

/***
 * @Brief	LETimer0 Interrupt Handler
 *
 * @Param	Void
 *
 * @Return	Void
 ***/
void LETIMER0_IRQHandler(void);

#endif /* SRC_LETIMER_H_ */
