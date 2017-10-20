#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define TRANSMISSION_INTERVAL 300 //Time between transmissions, in seconds
#define MAX_ADC_ATTEMPTS 4 //Times to try getting a reading

void main(void);
void init(void);
void loop(void);

#endif

