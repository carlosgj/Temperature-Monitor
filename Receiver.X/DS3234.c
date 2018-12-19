#include <xc.h>
#include "DS3234.h"

void writeRTCReg(unsigned char address, unsigned char data){
    address |= 0b10000000; //Set W bit
    RTC_CS_LAT = TRUE;
    SPI2Transfer(address);
    SPI2Transfer(data);
    RTC_CS_LAT = FALSE;
}

unsigned char readRTCReg(unsigned char address){
    address &= 0b01111111; //Clear W bit
    RTC_CS_LAT = TRUE;
    //asm("NOP");
    SPI2Transfer(address);
    //asm("NOP");
    unsigned char result = SPI2Transfer(0);
    RTC_CS_LAT = FALSE;
    return result;
}

void getTime(){
    seconds_reg.all = readRTCReg(REG_SECOND);
    minutes_reg.all = readRTCReg(REG_MINUTE);
    hours_reg.all = readRTCReg(REG_HOUR);
    date_reg.all = readRTCReg(REG_DATE);
    month_reg.all = readRTCReg(REG_MONTH);
    years_reg.all = readRTCReg(REG_YEAR);
}

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

//Writes the prospective time (in pros_*) to RTC
void setTime(void){
    writeRTCReg(REG_SECOND, pros_seconds.all);
    writeRTCReg(REG_MINUTE, pros_minutes.all);
    writeRTCReg(REG_HOUR, pros_hours.all);
    writeRTCReg(REG_DATE, pros_date.all);
    writeRTCReg(REG_MONTH, pros_month.all);
    writeRTCReg(REG_YEAR, pros_years.all);
    //Clear OSF flag
    unsigned char oldStat = readRTCReg(REG_STAT);
    oldStat &= 0b01111111;
    writeRTCReg(REG_STAT, oldStat);
}

void readAll(void){
    RTC_CS_LAT = FALSE;
    SPI2Transfer(0); //Register 0
    unsigned char i;
    for(i=0; i<7; i++){
        allRegs[i] = SPI2Transfer(0);
    }
    RTC_CS_LAT = TRUE;
}