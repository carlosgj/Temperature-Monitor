#include "DS1374.h"

uint8_t RTC_init(void){
    return 1;
}

void RTC_writeReg(uint8_t address, uint8_t data){

}

uint8_t RTC_readReg(uint8_t address){
    return 0;
}

void getTime(){
    seconds_reg.all = RTC_readReg(REG_SECOND);
    minutes_reg.all = RTC_readReg(REG_MINUTE);
    hours_reg.all = RTC_readReg(REG_HOUR);
    date_reg.all = RTC_readReg(REG_DATE);
    month_reg.all = RTC_readReg(REG_MONTH);
    years_reg.all = RTC_readReg(REG_YEAR);
    
    currentYear = (years_reg.tens * 10)+years_reg.ones;
    currentMonth = (month_reg.tens * 10)+month_reg.ones;
    currentDay = (date_reg.tens*10)+date_reg.ones;
    currentHour = (hours_reg.tens*10)+hours_reg.ones;
    minuteOfDay = (minutes_reg.tens*10)+minutes_reg.ones+(currentHour*60);
}

//Writes the prospective time (in pros_*) to RTC
void setTime(void){
    RTC_writeReg(REG_SECOND, pros_seconds.all);
    __delay_ms(1);
    RTC_writeReg(REG_MINUTE, pros_minutes.all);
    __delay_ms(1);
    RTC_writeReg(REG_HOUR, pros_hours.all);
    __delay_ms(1);
    RTC_writeReg(REG_DATE, pros_date.all);
    __delay_ms(1);
    RTC_writeReg(REG_MONTH, pros_month.all);
    __delay_ms(1);
    RTC_writeReg(REG_YEAR, pros_years.all);
    //Clear OSF flag
    //unsigned char oldStat = readRTCReg(REG_STAT);
    //oldStat &= 0b01111111;
    //writeRTCReg(REG_STAT, oldStat);
}

void readAll(void){
    
}


void RTCOscRestart(void){
    RTC_writeReg(REG_CONTROL, 0b10000100); //Enable write
    //foo = readRTCReg(REG_CONTROL);
    RTC_writeReg(REG_CONTROL, 0b10000100); //Disable oscillator
    __delay_ms(10);
    RTC_writeReg(REG_CONTROL, 0b00000100); //Enable oscillator
}