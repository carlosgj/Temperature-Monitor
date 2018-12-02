#ifndef SPI_H
#define	SPI_H

#include <xc.h> 
#include "common.h"
#include "pindef.h"

void SPIInit(void);
unsigned char SPITransfer(unsigned char data);

#endif	/* XC_HEADER_TEMPLATE_H */

