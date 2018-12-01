#include <xc.h>
#include "DS3234.h"

void writeReg(unsigned char address, unsigned char data){
    address |= 0b10000000; //Set W bit
    CS_LAT = FALSE;
    SPITransfer(address);
    SPITransfer(data);
    CS_LAT = TRUE;
}

unsigned char readReg(unsigned char address){
    address &= 0b01111111; //Clear W bit
    CS_LAT = FALSE;
    //asm("NOP");
    SPITransfer(address);
    //asm("NOP");
    unsigned char result = SPITransfer(0);
    CS_LAT = TRUE;
    return result;
}

void setTime(unsigned char seconds, unsigned char minutes, unsigned char hours, unsigned char date, unsigned char month, unsigned int year){
    seconds_reg.tens = seconds / 10;
    seconds_reg.ones = seconds % 10;
    
    minutes_reg.tens = minutes / 10;
    minutes_reg.ones = minutes % 10;
    
    hours_reg.n24 = FALSE; //Set 24-hour time
    if(hours > 19){
        hours_reg.nAM = TRUE;
    }
    else{
        hours_reg.nAM = FALSE;
    }
    if(hours > 9 && hours < 20){
        hours_reg.tens = TRUE;
    }
    else{
        hours_reg.tens = FALSE;
    }
    hours_reg.ones = hours % 10;
    
    date_reg.tens = date / 10;
    date_reg.ones = date % 10;
    
    month_reg.tens = month / 10;
    month_reg.ones = month % 10;
    
    years_reg.tens = year / 10;
    years_reg.ones = year % 10;
    
    writeReg(REG_SECOND, seconds_reg.all);
    writeReg(REG_MINUTE, minutes_reg.all);
    writeReg(REG_HOUR, hours_reg.all);
    writeReg(REG_DATE, date_reg.all);
    writeReg(REG_MONTH, month_reg.all);
    writeReg(REG_YEAR, years_reg.all);
    unsigned char oldStat = readReg(REG_CONTROL_STAT);
    oldStat &= 0b01111111;
    writeReg(REG_CONTROL_STAT, oldStat);
}

void readAll(void){
    CS_LAT = FALSE;
    SPITransfer(0); //Register 0
    unsigned char i;
    for(i=0; i<7; i++){
        allRegs[i] = SPITransfer(0);
    }
    CS_LAT = TRUE;
}