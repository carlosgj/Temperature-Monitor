#ifndef SPI_H
#define	SPI_H

#include "common.h"
#include "pindef.h"

void SPIInit(void); 
void SPI2_Open_SDSlow(void);
void SPI2_Close(void);
unsigned char SPI1Transfer(unsigned char data);
uint8_t SPI2Transfer(uint8_t data);
uint8_t SD_ExchangeByte(uint8_t data);
void SD_ExchangeBlock(uint8_t *data, uint8_t len);
void setSSP2CKE(unsigned char newVal);
#endif	/* XC_HEADER_TEMPLATE_H */

