#ifndef COMM_H
#define	COMM_H

#include <xc.h> // include processor files - each processor file is guarded.  

unsigned char sendReading(unsigned int reading);
unsigned char sendStatus(void);

#endif

