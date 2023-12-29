#ifndef SERIAL_H
#define	SERIAL_H

#include <xc.h> 
#include "common.h"
#include "pindef.h"
#include "therm.h"

unsigned char messageInProgressFlag = FALSE;
unsigned char messageBuffer[256];
volatile unsigned char messageBufferIdx = 0;

unsigned int pktIndex = 0;
unsigned int tempReadingRaw = 0;
unsigned int intTempReadingRaw = 0;
unsigned int battVoltage = 0;
unsigned int solarVoltage = 0;
unsigned int rcvCount = 0;
signed char rssi = 0;

void serialInit(void);
void processMessage(void);

#endif

