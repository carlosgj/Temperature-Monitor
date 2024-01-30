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

#endif	/* XC_HEADER_TEMPLATE_H */

