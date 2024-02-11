#ifndef DS1374_H
#define	DS1374_H

#include <xc.h> 
#include "common.h"
#include "I2C.h"
#include "datetime.h"
#include <string.h>

#define DS1374_I2C_ADDRESS  (0b01101000)

#define DS1374_REG_SECOND          0x00

#define DS1374_REG_CONTROL         0x0f
#define DS1374_REG_STAT    0x10

unsigned char allRegs[7];

uint8_t RTC_init(void);
uint8_t RTC_writeReg(uint8_t address, uint8_t data);
uint8_t RTC_readReg(uint8_t address, uint8_t *data);
void getTime(void);
void setTime(void);
void RTC_readAll(void);
uint8_t RTC_getSeconds(uint32_t *secs);

#endif	/* XC_HEADER_TEMPLATE_H */

