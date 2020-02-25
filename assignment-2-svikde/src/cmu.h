/***
 * @File	cmu.h
 * @Brief	Contains function definitions pertaining to the CMU.
 * 			Used to configure clock source and oscillator for
 * 			LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	2.0
 ***/

#ifndef SRC_CMU_H_
#define SRC_CMU_H_

#include "sleep.h"
#include "em_cmu.h"		// Oscillator Type Enumerators

/***
 * @Brief	Configures Clock Source, Oscillator & Clock Pre-scaler
 * 			for LETimer0 depending upon the desired energy mode
 *
 * @Param	[SLEEP_EnergyMode_t] used to pass type of sleep mode
 * 			to conditionally alter comparator contents.
 *
 * @Return	Void
 ***/
void Clk_LETimer0_Init(const SLEEP_EnergyMode_t Energy_Mode);

#endif /* SRC_CMU_H_ */
