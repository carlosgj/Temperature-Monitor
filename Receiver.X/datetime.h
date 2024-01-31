#ifndef DATETIME_H
#define	DATETIME_H

#include <xc.h> 
#include "common.h"

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

struct date_t{
    uint16_t year;
    uint8_t month;
    uint8_t dom;
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

//void formatTime(unsigned char seconds, unsigned char minutes, unsigned char hours, unsigned char date, unsigned char month, unsigned int year);

// -1 is a placeholder for indexing purposes.
const uint8_t DAYS_IN_MONTH[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// -1 is a placeholder for indexing purposes.
const uint16_t DAYS_BEFORE_MONTH[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

// Month and day names.  For localized versions, see the calendar module.
const char* const MONTHNAMES[] = {"NUL", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* const DAYNAMES[] = {"NUL", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

//DI4Y = 4 * 365 + 1;
#define DI4Y    (1461L)

//DI100Y == 25 * DI4Y - 1
#define DI100Y  (36524L)

//DI400Y == 4 * DI100Y + 1
#define DI400Y  (146097)




void datetime_tests(void);

uint8_t is_leap(uint16_t year);
uint32_t days_before_year(uint16_t year);
uint8_t days_in_month(uint16_t year, uint8_t month);
uint16_t days_before_month(uint16_t year, uint8_t month);
uint32_t ymd2ord(struct date_t *ymd);
void ord2ymd(uint32_t n, struct date_t *ymd);

#endif	/* XC_HEADER_TEMPLATE_H */

