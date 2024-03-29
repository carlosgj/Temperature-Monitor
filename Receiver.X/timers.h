#ifndef TIMERS_H
#define	TIMERS_H

#include <xc.h> 
#include "common.h"

#define MS_TMR_VAL (63535)

volatile uint16_t seconds = 0;
volatile uint16_t milliseconds = 0; //MIllisecond-of-second
volatile uint16_t monoMillis = 0; //Monotonic millisecond counter

void timerInit(void);
inline void getMillis(uint16_t *destination);

void variableDelayMs(uint16_t ms);

#endif


