/***
 * @File	letimer.h
 * @Brief	Contains macros and function declarations
 * 			pertaining to LETimer0. The following functions
 * 			are used to configure the LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	4.0
 ***/

#ifndef SRC_LETIMER_H_
#define SRC_LETIMER_H_

#include "em_letimer.h"
#include "em_core.h"
#include "gpio.h"
#include "mesh.h"
#include "cmu.h"
#include "log.h"

#define LEDONTIME_MS		(175)								//On time of LED in MS
#define LED_BLKPERIOD_MS	(2250)								//Blink Period of LED in MS
#define FCPU_LFXO			(32768)								//LFXO Frequency in Hz
#define FCPU_ULFRCO			(1000)								//ULFRCO Frequency in Hz

#define MAX_COUNTER_VAL		(0xFFFF)							//Maximum Counter Value for LETIMER0
#define INT_PERIOD_MS		(1000)								//Interrupt Period in ms
#define DELAY80MS			(80)								//Delay 80 ms
#define DELAY10MS			(10)								//Delay 10 ms

/* To Calculate Comparator Values - Macro referenced from Lecture-5 */
#define MS2COMP_VALUE(ms_value, fcpu, prescaler)	(uint16_t)(((ms_value) *((fcpu)/(prescaler)))/1000)

/* Indicates if designated delay is achieved */
volatile uint8_t IsDelayDone;

/***
 * 	@Brief	Initializes LETimer0 by setting the inherent
 * 			structure variable and setting comparator values.
 * 			Also involves configurations for interrupt flags.
 *
 *
 * 	@Param	void
 *
 *  @Return	Void
 ***/
void LETimer0_Init(void);

/***
 * 	@Brief	Initializes Clock sources, Oscillators, LETimer0
 * 			Interrupt Registers for LETimer0
 *
 * 	@Param	void
 *
 * 	@Return	Void
 ***/
void LETimer0_Interrupt_Init(void);

/***
 * 	@Brief	Used to provide a define amount of delay in MS
 * 			without blocking. Uses COMP1 interrupt of LETimer0
 * 			to provide required delay.
 *
 * 	@Param	Delay duration in MS.
 *
 * 	@Return	Void
 ***/
void timerSetEventInMS(uint32_t ms_until_wakeup);

/***
 * @Brief	LETimer0 Interrupt Handler
 *
 * @Param	Void
 *
 * @Return	Void
 ***/
void LETIMER0_IRQHandler(void);

#endif /* SRC_LETIMER_H_ */
