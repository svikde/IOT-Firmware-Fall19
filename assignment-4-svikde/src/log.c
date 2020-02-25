/*
 * log.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Dan Walkes
 */

#include "retargetserial.h"
#include "log.h"
#include <stdbool.h>

/**
 * @return a timestamp value for the logger, typically based on a free running timer.
 * This will be printed at the beginning of each log message.
 */

uint32_t loggerGetTimestamp(void)
{
	//return timerGetRunTimeMilliseconds();
	uint32_t counter_val = LETIMER_CounterGet(LETIMER0);
	uint32_t comp0_val = LETIMER_CompareGet(LETIMER0, 0);
	return (Counter3sec * comp0_val + (comp0_val - counter_val));
}

/**
 * Initialize logging for Blue Gecko.
 * See https://www.silabs.com/community/wireless/bluetooth/forum.topic.html/how_to_do_uart_loggi-ByI
 */
#if INCLUDE_LOGGING

void logFlush()
{
	RETARGET_SerialFlush();
}

void logInit(void)
{
	Counter3sec = 0;
	RETARGET_SerialInit();
	/**
	 * See https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__RetargetIo.html#ga9e36c68713259dd181ef349430ba0096
	 * RETARGET_SerialCrLf() ensures each linefeed also includes carriage return.  Without it, the first character is shifted in TeraTerm
	 */
	RETARGET_SerialCrLf(true);
	LOG_INFO("Initialized Logging");
}
#endif
