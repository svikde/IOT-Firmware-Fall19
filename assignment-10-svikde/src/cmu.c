/***
 * @File	cmu.c
 * @Brief	Contains function definitions pertaining to the CMU.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	2.0
 ***/

#include "cmu.h"

void Clk_LETimer0_Init(void)
{
	/* ULFRCO has a frequency of 1000 Hz */
	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
	CMU_ClockEnable(cmuClock_LFA, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);
}


