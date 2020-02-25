/***
 * @File	letimer.c
 * @Brief	Contains function definitions pertaining to LETimer0.
 * @Author	Souvik De
 * @Date	09/11/2019
 * @Version	3.0
 ***/

#include "letimer.h"

void LETimer0_Init(const SLEEP_EnergyMode_t Energy_Mode)
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

	IntPeriodCount = MS2COMP_VALUE(INT_PERIOD_MS, FCPU_ULFRCO, 1);

	LETIMER_CompareSet(LETIMER0, 0, IntPeriodCount);

	LETIMER_IntEnable(LETIMER0, LETIMER_IFC_UF);

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

void timerWaitUs(uint32_t us_wait)
{
	uint32_t CurrentCount = 0;
	uint32_t CountDelay = 0;

	CountDelay = MS2COMP_VALUE(us_wait/1000, FCPU_ULFRCO, 1);
	CurrentCount = LETIMER_CounterGet(LETIMER0);

	LETIMER_CompareSet(LETIMER0, 1, CurrentCount - CountDelay);

	/*Enable Interrupt for Comparator1 Match and Underflow*/
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);

	while(!IsDelayDone)
	{
		SLEEP_Sleep();
	}

	IsDelayDone = 0;
	LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);
}

void LETIMER0_IRQHandler(void)
{
	/* Check flag to determine Interrupt Condition */
	int flag = LETIMER_IntGet(LETIMER0);

	/* For Underflow */
	if(flag & LETIMER_IF_UF)
	{
		//gpioLed1Toggle();
		Interrupt_Event_Bitmask = 0x1;
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_UF);
	}

	/* For Compare Match of Comp1 Value */
	if(flag & LETIMER_IF_COMP1)
	{
		//gpioLed1Toggle();
		IsDelayDone = 1;
		LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
	}
}


