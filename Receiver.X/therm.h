#ifndef THERM_H
#define	THERM_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "pindef.h"
#include "common.h"

#define FVR_TIMEOUT 100

unsigned char thermInit(void);
unsigned char takeReading(unsigned int *result_p);
unsigned char formatTemperatureToChar(unsigned int rawReading);
void getTemperature(void);

#endif	/* XC_HEADER_TEMPLATE_H */

