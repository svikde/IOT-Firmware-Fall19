/***
 * @File	letimer.c
 * @Brief	Contains function definitions pertaining to LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	2.0
 ***/

#include "letimer.h"

void LETimer0_Init(const SLEEP_EnergyMode_t Energy_Mode)
{
	int OnCount = 0;
	int PeriodCount = 0;

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

	/* FCPU_ULFRCO/1000 = 1 for EM3 */
	if(Energy_Mode == sleepEM3)
	{
		OnCount = MS2COMP_VALUE(LEDONTIME_MS, FCPU_ULFRCO, 1);
		PeriodCount = MS2COMP_VALUE(LED_BLKPERIOD_MS, FCPU_ULFRCO, 1);
	}
	else
	{
		/* FCPU_LFXO/(Prescaler_4 * 1000) = 8.192 for EM0, EM1, EM2 */
		OnCount = MS2COMP_VALUE(LEDONTIME_MS, FCPU_LFXO, 4);
		PeriodCount = MS2COMP_VALUE(LED_BLKPERIOD_MS, FCPU_LFXO, 4);
	}

	/* Load Compare Register 0 with count equivalent of the Time Period */
	LETIMER_CompareSet(LETIMER0, 0, PeriodCount);
	/* Load Compare Register 1 with count equivalent of the Led On Time */
	LETIMER_CompareSet(LETIMER0, 1, OnCount);

	/* Enable Interrupt for Comparator1 Match and Underflow */
	LETIMER_IntEnable(LETIMER0, LETIMER_IFC_UF | LETIMER_IEN_COMP1);

	/* Enable Interrupt in NVIC */
	NVIC_EnableIRQ(LETIMER0_IRQn);

	/* Finalize configurations with LETimer0 */
	LETIMER_Enable(LETIMER0, true);
}

void LETimer0_EMBlockStart(const SLEEP_EnergyMode_t Energy_Mode)
{
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	if(Energy_Mode == sleepEM1)
		SLEEP_SleepBlockBegin(sleepEM2); 	//Block EM2 and higher
	else
		SLEEP_SleepBlockBegin(sleepEM3);	//Block EM3 and higher

	CORE_EXIT_CRITICAL();
}

void LETimer0_Interrupt_Init(const SLEEP_EnergyMode_t Energy_Mode)
{
	Clk_LETimer0_Init(Energy_Mode);
	LETimer0_Init(Energy_Mode);

	/* Configure Only if EM1 and EM2 are requested */
	if((Energy_Mode != sleepEM3) || (Energy_Mode != sleepEM0))
		LETimer0_EMBlockStart(Energy_Mode);
}

void LETIMER0_IRQHandler(void)
{
	/* Check flag to determine Interrupt Condition */
	int flag = LETIMER_IntGet(LETIMER0);

	/* For Underflow */
	if(flag & LETIMER_IF_UF)
	{
		gpioLed1Toggle();
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_UF);
	}

	/* For Compare Match of Comp1 Value */
	if(flag & LETIMER_IF_COMP1)
	{
		gpioLed1Toggle();
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
	}
}


