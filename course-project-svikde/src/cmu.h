/***
 * @File	cmu.h
 * @Brief	Contains function declarations pertaining to the CMU.
 * 			Used to configure clock source and oscillator for
 * 			LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	2.0
 ***/

#ifndef SRC_CMU_H_
#define SRC_CMU_H_

#include "em_cmu.h"		// Oscillator Type Enumerators

/***
 * @Brief	Configures Clock Source, Oscillator & Clock Pre-scaler
 * 			for LETimer0
 *
 * @Param	void
 *
 * @Return	Void
 ***/
void Clk_LETimer0_Init(void);

#endif /* SRC_CMU_H_ */
