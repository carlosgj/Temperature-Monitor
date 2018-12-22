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

#define REG_CONTROL         0x0f
#define REG_STAT    0x10

union bcd {
    unsigned char all;
    struct{
        unsigned ones   :4;
        unsigned tens   :3;
    };
};

union hour_t{
    unsigned char all;
    struct{
        unsigned ones   :4;
        unsigned tens   :2;
        unsigned n24    :1;
        unsigned ZERO   :1;
    };
};

union bcd seconds_reg;
union bcd minutes_reg;
union hour_t hours_reg;
union bcd date_reg;
union bcd month_reg;
union bcd years_reg;

union bcd pros_seconds;
union bcd pros_minutes;
union hour_t pros_hours;
union bcd pros_date;
union bcd pros_month;
union bcd pros_years;

unsigned char allRegs[7];

void writeRTCReg(unsigned char address, unsigned char data);
unsigned char readRTCReg(unsigned char address);
void formatTime(unsigned char seconds, unsigned char minutes, unsigned char hours, unsigned char date, unsigned char month, unsigned int year);
void getTime(void);
void setTime(void);
void readAll(void);

#endif	/* XC_HEADER_TEMPLATE_H */

