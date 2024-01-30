#include <xc.h>
#include "datetime.h"

void formatTime(unsigned char seconds, unsigned char minutes, unsigned char hours, unsigned char date, unsigned char month, unsigned int year){
    seconds_reg.tens = seconds / 10;
    seconds_reg.ones = seconds % 10;
    
    minutes_reg.tens = minutes / 10;
    minutes_reg.ones = minutes % 10;
    
    hours_reg.n24 = FALSE; //Set 24-hour time
    hours_reg.tens = hours / 10;
    hours_reg.ones = hours % 10;
    
    date_reg.tens = date / 10;
    date_reg.ones = date % 10;
    
    month_reg.tens = month / 10;
    month_reg.ones = month % 10;
    
    years_reg.tens = year / 10;
    years_reg.ones = year % 10;
}