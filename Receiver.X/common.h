#ifndef COMMON_H
#define	COMMON_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define TRUE 1
#define FALSE 0

#define INPUT 1
#define OUTPUT 0

#define _XTAL_FREQ 64000000

unsigned int ms_count=0;
unsigned int sec_count =0;

unsigned char currentYear = 0xff;
unsigned char currentMonth = 0xff;
unsigned char currentDay = 0xff;
unsigned char currentHour = 0xff;

unsigned char firstRun = TRUE;

unsigned char currentTemperatureByte; 

#endif	/* XC_HEADER_TEMPLATE_H */

