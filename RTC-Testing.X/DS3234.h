#ifndef DS3234_H
#define	DS3234_H

#include <xc.h> 
#include "common.h"
#include "pindef.h"
#include "SPI.h"

#define REG_SECOND          0x00
#define REG_MINUTE          0x01
#define REG_HOUR            0x02
#define REG_DAY             0x03
#define REG_DATE            0x04
#define REG_MONTH           0x05
#define REG_YEAR            0x06

#define REG_CONTROL         0x0e
#define REG_CONTROL_STAT    0x0f

union bcd {
    unsigned char all;
    struct{
        unsigned ones   :4;
        unsigned tens   :3;
        unsigned ZERO   :1;
    };
};

union hour_t{
    unsigned char all;
    struct{
        unsigned ones   :4;
        unsigned tens   :1;
        unsigned nAM    :1;
        unsigned n24    :1;
        unsigned ZERO   :1;
    };
};

union month_t{
    unsigned char all;
    struct{
        unsigned ones       :4;
        unsigned tens       :1;
        unsigned ZERO       :2;
        unsigned century    :1;
    };
};

union bcd seconds_reg;
union bcd minutes_reg;
union hour_t hours_reg;
union bcd date_reg;
union month_t month_reg;
union bcd years_reg;

unsigned char allRegs[7];

void writeReg(unsigned char address, unsigned char data);
unsigned char readReg(unsigned char address);
void setTime(unsigned char seconds, unsigned char minutes, unsigned char hours, unsigned char date, unsigned char month, unsigned int year);
void readAll(void);

#endif	/* XC_HEADER_TEMPLATE_H */

