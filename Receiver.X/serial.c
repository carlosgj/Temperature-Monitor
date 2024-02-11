#include "serial.h"

void serialInit(void){
    //Configure pin
    //RX1PPS = 0b00010101; //C5
    
    //Setup BRG
    //BAUD1CONbits.BRG16 = TRUE;
    //TX1STAbits.CSRC = TRUE;
    //TX1STAbits.BRGH = TRUE;
    //SP1BRGH = 0x06;
    //SP1BRGL = 0x82;
    
    //Enable receiver
    //RC1STAbits.CREN = TRUE;
    
    //Enable receive interrupt
    //PIE3bits.RC1IE = TRUE;
    
    //Enable port
    //RC1STAbits.SPEN = TRUE;
}

void processMessage(void){
    if(messageBufferIdx != 13){
        messageBufferIdx = 0;
        messageInProgressFlag = FALSE;
        //return;
    }
    
    pktIndex = *((unsigned int *)messageBuffer);
    tempReadingRaw = *((unsigned int *)(messageBuffer+2));
    battVoltage = *((unsigned int *)(messageBuffer+4));
    solarVoltage = *((unsigned int *)(messageBuffer+6));
    intTempReadingRaw = *((unsigned int *)(messageBuffer+8));
    rcvCount = *((unsigned int *)(messageBuffer+10));
    rssi = *((signed char *)(messageBuffer+12));
    
    messageBufferIdx = 0;
    messageInProgressFlag = FALSE;
}