#ifndef DS1374_H
#define	DS1374_H

#include <xc.h> 
#include "common.h"
#include "I2C.h"
#include "datetime.h"

#define REG_SECOND          0x00
#define REG_MINUTE          0x01
#define REG_HOUR            0x02
#define REG_DAY             0x03
#define REG_DATE            0x04
#define REG_MONTH           0x05
#define REG_YEAR            0x06

#define REG_CONTROL         0x0f
#define REG_STAT    0x10

unsigned char allRegs[7];

void writeRTCReg(unsigned char address, unsigned char data);
unsigned char readRTCReg(unsigned char address);
void getTime(void);
void setTime(void);
void readAll(void);
void RTCOscRestart(void);

#endif	/* XC_HEADER_TEMPLATE_H */

