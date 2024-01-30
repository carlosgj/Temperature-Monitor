#ifndef SPI_H
#define	SPI_H

#include "common.h"
#include "pindef.h"

void SPIInit(); 
unsigned char SPI1Transfer(unsigned char data);
unsigned char SPI2Transfer(unsigned char data);
void setSSP2CKE(unsigned char newVal);
#endif	/* XC_HEADER_TEMPLATE_H */

