#ifndef COMMON_H
#define	COMMON_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdio.h>
#include "FIFO.h"
#include "gitVersion.h"
#include "pindef.h"
#include "timers.h"

#pragma warning disable 520

#define TRUE 1
#define FALSE 0

#define INPUT 1
#define OUTPUT 0

#define _XTAL_FREQ 64000000

#define SW_VER_STR "2.0.0"

#define INTEN   INTCON0bits.GIE = TRUE
#define INTDIS  INTCON0bits.GIE = FALSE

uint8_t unhandledIntCount;
uint8_t lastUnhandledInt;
uint8_t unhandledIntFlag = FALSE;
//uint8_t loopOverruns;

enum ResetCause {
    RS_POR,
    RS_BOR,
    RS_MCLR,
    RS_WDT,
    RS_WIN,
    RS_RST,
    RS_SO,
    RS_SU,
    RS_DP,
    RS_VREG,
    RS_MEM,
    RS_INVALID
};

enum ResetCause resetCause = RS_INVALID;


unsigned int ms_count=0;
unsigned int sec_count =0;

unsigned char currentYear = 0xff;
unsigned char currentMonth = 0xff;
unsigned char currentDay = 0xff;
unsigned char currentHour = 0xff;
unsigned int minuteOfDay = 0xffff;

unsigned char firstRun = TRUE;

unsigned char safeMode = TRUE;

unsigned char currentTemperatureByte = 0x80; 

unsigned char tempData[365] = {140, 146, 154, 162, 170, 178, 184, 190, 196, 202, 206, 210, 214, 216, 218, 218, 218, 218, 216, 214, 212, 208, 204, 198, 194, 186, 180, 174, 166, 158, 150, 142, 134, 126, 118, 110, 104, 96, 90, 84, 78, 74, 70, 66, 62, 60, 60, 60, 60, 60, 62, 64, 68, 72, 78, 82, 88, 94, 102, 110, 116, 124, 132, 140, 148, 156, 164, 172, 178, 186, 192, 198, 202, 208, 210, 214, 216, 218, 218, 218, 218, 216, 214, 212, 208, 202, 198, 192, 186, 180, 172, 164, 156, 148, 140, 132, 126, 118, 110, 102, 96, 88, 84, 78, 72, 68, 66, 62, 60, 60, 60, 60, 60, 62, 66, 68, 74, 78, 84, 90, 96, 104, 110, 118, 126, 134, 142, 150, 158, 166, 172, 180, 186, 192, 198, 204, 208, 212, 214, 216, 218, 218, 218, 218, 216, 214, 210, 206, 202, 196, 192, 184, 178, 170, 164, 156, 148, 140, 132, 124, 116, 108, 102, 94, 88, 82, 76, 72, 68, 64, 62, 60, 60, 60, 60, 60, 64, 66, 70, 74, 78, 84, 90, 98, 104, 112, 120, 128, 136, 144, 150, 158, 166, 174, 180, 188, 194, 200, 204, 208, 212, 214, 218, 218, 218, 218, 218, 216, 214, 210, 206, 202, 196, 190, 184, 176, 170, 162, 154, 146, 138, 130, 122, 114, 108, 100, 94, 86, 82, 76, 72, 68, 64, 62, 60, 60, 60, 60, 62, 64, 66, 70, 74, 80, 86, 92, 98, 106, 112, 120, 128, 136, 144, 152, 160, 168, 176, 182, 188, 194, 200, 204, 210, 212, 216, 218, 218, 218, 218, 218, 216, 212, 210, 206, 200, 194, 188, 182, 176, 168, 160, 152, 144, 136, 128, 122, 114, 106, 98, 92, 86, 80, 76, 70, 66, 64, 62, 60, 60, 60, 60, 62, 64, 68, 72, 76, 80, 86, 92, 100, 106, 114, 122, 130, 138, 146, 154, 162, 168, 176, 184, 190, 196, 200, 206, 210, 214, 216, 218, 218, 218, 218, 218, 216, 212, 208, 204, 200, 194, 188, 182, 174, 166, 160, 152, 144, 136, 128, 120, 112, 104, 98, 92, 84, 80, 74, 70, 66, 64, 62, 60, 60, 60, 60, 62};

const unsigned char daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

FIFO DbgUARTRxBuf;

#endif	/* XC_HEADER_TEMPLATE_H */

