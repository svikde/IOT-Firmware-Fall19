/***
 * @File	letimer.c
 * @Brief	Contains function definitions pertaining to LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	4.0
 ***/

#include "letimer.h"
#include "display.h"

void LETimer0_Init(void)
{
	int IntPeriodCount = 0;

	IsDelayDone = 0;

	LETIMER_Init_TypeDef LETimer_Config = {
			.enable = true,
			.debugRun = false,
			.comp0Top = true,				//COMP1 is the TOP Value instead of 0xFFFF
			.bufTop = false,
			.out0Pol = 0,
			.out1Pol = 0,
			.ufoa0 = letimerUFOANone,
			.ufoa1 = letimerUFOANone,
			.repMode = letimerRepeatFree	//Configure in Free Running Mode
	};

	LETIMER_Init(LETIMER0, &LETimer_Config);

	//Get count equivalent for the defined period of operation
	IntPeriodCount = MS2COMP_VALUE(INT_PERIOD_MS, FCPU_ULFRCO, 1);

	//Load count value into Comparator 0
	LETIMER_CompareSet(LETIMER0, 0, IntPeriodCount);

	//Load max count value in Comparator 1 to avoid firing COMP1 Interrupt
	LETIMER_CompareSet(LETIMER0, 1, MAX_COUNTER_VAL);

	//Disable COMP0 Interrupt
	LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP0);

	//Disable COMP1 Interrupt
	LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);

	//Enable Underflow Interrupt
	LETIMER_IntEnable(LETIMER0, LETIMER_IFC_UF);

	/* Enable Interrupt in NVIC */
	NVIC_EnableIRQ(LETIMER0_IRQn);

	/* Finalize configurations with LETimer0 */
	LETIMER_Enable(LETIMER0, true);
}

void LETimer0_Interrupt_Init(void)
{
	Clk_LETimer0_Init();
	LETimer0_Init();
}

void DelayMS(uint32_t ms_until_wakeup)
{
	uint32_t CurrentCount = 0;
	uint32_t CountDelay = 0;
	uint32_t Comp0Val = 0;

	//Get Count Equivalent to the desired delay duration
	CountDelay = MS2COMP_VALUE(ms_until_wakeup, FCPU_ULFRCO, 1);

	//Get Current Counter value
	CurrentCount = LETIMER_CounterGet(LETIMER0);

	//Wrap around condition
	if(CurrentCount < CountDelay)
	{
		Comp0Val = LETIMER_CompareGet(LETIMER0, 0);
		LETIMER_CompareSet(LETIMER0, 1, Comp0Val - (CountDelay - CurrentCount));
	}
	else
	{
		LETIMER_CompareSet(LETIMER0, 1, CurrentCount - CountDelay);
	}

	/*Enable Interrupt for Comparator1 Match and Underflow*/
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);
}

void LETIMER0_IRQHandler(void)
{
	/* Check flag to determine Interrupt Condition */
	int flag = LETIMER_IntGet(LETIMER0);

	/* For Underflow */
	if(flag & LETIMER_IF_UF)
	{
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_UF);

		//For Time Stamp Purposes
		CountSec += 1;

		gecko_external_signal(TIMER_EXPIRED);
	}

	/* For Compare Match of Comp1 Value */
	if(flag & LETIMER_IF_COMP1)
	{
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
		LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);

		gecko_external_signal(DELAY_EXPIRED);
	}
}


