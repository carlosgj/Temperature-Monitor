#include <xc.h>
#include "SPI.h"
#include "common.h"
#include "pindef.h"

void SPIInit(){
    SCK1_TRIS = OUTPUT;
    MISO1_TRIS = INPUT;
    MOSI1_TRIS = OUTPUT;
    DISP_CS_TRIS = OUTPUT;
    DISP_CS_LAT = TRUE;
    
    //Set up inputs
    SSP1CLKPPS = 0b10011; //SSP1 clk in on C3
    SSP1DATPPS = 0b10100; //SSP1 MISO on C4 
    
    //Set up outputs
    RC3PPS = 0x0F; //MSSP1 SCK on C3
    RB3PPS = 0x10; //MSSP1 SDO on B3
    
    SSP1STATbits.SMP = 0;
    SSP1STATbits.CKE = 1;
    SSP1CON1bits.CKP = 0;
    SSP1CON1bits.SSPM = 0b0010; //Fosc/64
    SSP1CON3bits.BOEN = TRUE;
    SSP1CON1bits.SSPEN = TRUE; //Enable
    
    //MSSP 2 setup
    SCK2_TRIS = OUTPUT;
    MOSI2_TRIS = OUTPUT;
    MISO2_TRIS = INPUT;
    
    RTC_CS_TRIS = OUTPUT;
    MEM1_CS_TRIS = OUTPUT;
    MEM2_CS_TRIS = OUTPUT;
    RFM69_CS_TRIS = OUTPUT;
    
    RTC_CS_LAT = FALSE;
    MEM1_CS_LAT = TRUE;
    MEM2_CS_LAT = TRUE;
    RFM69_CS_LAT = TRUE;
    
    //Set up inputs
    SSP2CLKPPS = 0b00001001; //SSP2 clk in on B1
    SSP2DATPPS = 0b00001010; //SSP2 MISO on B2 
    
    //Set up outputs
    RB1PPS = 0x11; //MSSP2 clk out on RB1;
    RD4PPS = 0x12; //MSSP2 MOSI on D4
    
    SSP2STATbits.SMP = 0;
    SSP2STATbits.CKE = 0;
    SSP2CON1bits.CKP = 0;
    SSP2CON1bits.SSPM = 0b0010; //Fosc/64
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

unsigned char SPI1Transfer(unsigned char data){
    unsigned char timeout = 100;
    SSP1BUF = data;
    while(!SSP1STATbits.BF && timeout > 0){
        timeout--;
    }
    unsigned char result = SSP1BUF;
    if(timeout == 0){
        return 0xff;
    }
    return result;
}

void setSSP2CKE(unsigned char newVal){
    unsigned char oldVal = SSP2STATbits.CKE;
    if(newVal == oldVal){
        return;
    }
    else{
        SSP2CON1bits.SSPEN = FALSE;
        SSP2STATbits.CKE = newVal;
        SSP2CON1bits.SSPEN = TRUE;
        __delay_us(100);
    }
    }
