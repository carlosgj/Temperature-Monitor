#ifndef ANALOG_H
#define	ANALOG_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define ANALOG_TIMEOUT 60 //Tad = 1 us, so conversion time should be ~11.5 us, or 46 instruction cycles

unsigned char readAnalog(unsigned int *result);

#endif

