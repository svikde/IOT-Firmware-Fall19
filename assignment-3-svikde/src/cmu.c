/***
 * @File	cmu.c
 * @Brief	Contains function definitions pertaining to the CMU.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	2.0
 ***/

#include "cmu.h"

void Clk_LETimer0_Init(const SLEEP_EnergyMode_t Energy_Mode)
{
	/* ULFRCO for Energy Mode EM3 */
	if(Energy_Mode == sleepEM3)
	{
		/* ULFRCO has a frequency of 1000 Hz */
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
		CMU_ClockEnable(cmuClock_LFA, true);
	}
	else
	{
		/* LFXO for EM0, EM1, EM2 */
		/* LFXO has a frequency of 32768 Hz */
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
		CMU_ClockEnable(cmuClock_LFA, true);
		/* Dividing 4 to get max 8sec in 1 full count of 2 raised to 16 (0XFFFF) */
		CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_4);
	}

	CMU_ClockEnable(cmuClock_LETIMER0, true);
}


