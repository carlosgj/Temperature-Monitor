#include <xc.h>
#include "SPI.h"
#include "common.h"
#include "pindef.h"

void SPIInit(){
    SCK2_TRIS = OUTPUT;
    MOSI2_TRIS = OUTPUT;
    MISO2_TRIS = INPUT;
    RTC_CS_TRIS = OUTPUT;
    MEM1_CS_TRIS = OUTPUT;
    MEM2_CS_TRIS = OUTPUT;
    RFM69_CS_TRIS = OUTPUT;
    MEM1_CS_LAT = TRUE;
    MEM2_CS_LAT = TRUE;
    RFM69_CS_LAT = TRUE;
    
    
    RTC_CS_LAT = TRUE;
    
    //Set up inputs
    SSP2CLKPPS = 0b00001001; //SSP2 clk in on B1
    SSP2DATPPS = 0b00001010; //SSP2 MISO on B2 
    
    //Set up outputs
    RB1PPS = 0x11; //MSSP2 clk out on RB1;
    RD4PPS = 0x12; //MSSP2 MOSI on D4
    
    SSP2STATbits.SMP = 0;
    SSP2STATbits.CKE = 0;
    SSP2CON1bits.CKP = 0;
    SSP2CON1bits.SSPM = 0b0001; //Fosc/16
    SSP2CON3bits.BOEN = TRUE;
    SSP2CON1bits.SSPEN = TRUE; //Enable
}

unsigned char SPI2Transfer(unsigned char data){
    unsigned char timeout = 100;
    SSP2BUF = data;
    while(!SSP2STATbits.BF && timeout > 0){
        timeout--;
    }
    unsigned char result = SSP2BUF;
    if(timeout == 0){
        return 0xff;
    }
    return result;
}
