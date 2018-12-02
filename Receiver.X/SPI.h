#ifndef SPI_H
#define	SPI_H

#include <xc.h> // include processor files - each processor file is guarded.  
void SPIInit(); 
unsigned char SPI1Transfer(unsigned char data);
unsigned char SPI2Transfer(unsigned char data);
#endif	/* XC_HEADER_TEMPLATE_H */

