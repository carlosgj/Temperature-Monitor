#include <xc.h>
#include "SPI.h"

void SPIInit(void){
    ANSELD = 0;
    ANSELA = 0;
    ANSELB = 0; 
    ANSELC = 0;
    
    SCK_TRIS = OUTPUT;
    MOSI_TRIS = OUTPUT;
    MISO_TRIS = INPUT;
    CS_TRIS = OUTPUT;
    MEM1CS_TRIS = OUTPUT;
    MEM2CS_TRIS = OUTPUT;
    RADIOCS_TRIS = OUTPUT;
    MEM1CS_LAT = TRUE;
    MEM2CS_LAT = TRUE;
    RADIOCS_LAT = TRUE;
    
    
    CS_LAT = TRUE;
    
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

unsigned char SPITransfer(unsigned char data){
    //unsigned char throwaway = SSPBUF;
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