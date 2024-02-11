#include "DS1374.h"

uint8_t RTC_init(void){
    uint8_t status = 0;
    
    //Check for presence of RTC
//    I2C1ADB1 = (DS1374_I2C_ADDRESS << 1) | 1; //Set read bit
//    I2C1CNTL = 0;
//    I2C1CON0bits.S = TRUE;
//    
//    __delay_ms(10);
//    
//    
//    if(I2C1CON1bits.ACKSTAT){
//        //ACK not received
//        printf("RTC: I2C ACK not received.\n");
//        status = 1;
//    } else{
//        //ACK received
//        printf("RTC: ACK received.\n");
//    }
    
    RTC_readAll();
    
    uint32_t testSeconds;
    uint8_t stat;
    stat = RTC_getSeconds(&testSeconds);
    printf("\tRTC: Status: %d Time: %lu s\n", stat, testSeconds);
    
    return status;
}

uint8_t RTC_writeReg(uint8_t address, uint8_t data){
    uint8_t status = 0;
    uint8_t buf[2];
    buf[0] = address;
    buf[1] = data;
    status = I2C_write(DS1374_I2C_ADDRESS, buf, 2);
    return status;
}

uint8_t RTC_readReg(uint8_t address, uint8_t *data){
    uint8_t status = 0;
    *data = address; //Use data as temp buffer
    status = I2C_write(DS1374_I2C_ADDRESS, data, 1);
    if(status != 0){
        return status;
    }
    status = I2C_read(DS1374_I2C_ADDRESS, data, 1);
    return status;
}

uint8_t RTC_getSeconds(uint32_t *secs){
    uint8_t status = 0;
    uint8_t buf[4];
    buf[0] = 0;
    status = I2C_write(DS1374_I2C_ADDRESS, buf, 1);
    if(status != 0){
        return status;
    }
    status = I2C_read(DS1374_I2C_ADDRESS, buf, 4);
    *secs = *(uint32_t *)buf;
    
    return status;
}

void getTime(){
    currentYear = (years_reg.tens * 10)+years_reg.ones;
    currentMonth = (month_reg.tens * 10)+month_reg.ones;
    currentDay = (date_reg.tens*10)+date_reg.ones;
    currentHour = (hours_reg.tens*10)+hours_reg.ones;
    minuteOfDay = (minutes_reg.tens*10)+minutes_reg.ones+(currentHour*60);
}

void setTime(void){
}

void RTC_readAll(void){
    uint8_t i = 0;
    uint8_t status;
    uint8_t buf[12];
    //Set register pointer to 0
    memset(buf, 0, 12);
    status = I2C_write(DS1374_I2C_ADDRESS, buf, 1);
    if(status != 0){
        printf("RTC I2C write error: %d\n", status);
        return;
    }
    //Read 10 bytes
    status = I2C_read(DS1374_I2C_ADDRESS, buf, 10);
    if(status != 0){
        printf("RTC I2C write error: %d\n", status);
        return;
    }
    
    printf("\tRTC Reg:");
    for(i=0; i<10; i++){
        printf(" %02X", i);
    }
    printf("\n");
    
    printf("\tReg Val:");
    for(i=0; i<10; i++){
        printf(" %02X", buf[i]);
    }
    printf("\n");
}