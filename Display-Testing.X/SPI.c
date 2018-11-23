#include <xc.h>
#include "SPI.h"

void SPIInit(void){
    SCK_TRIS = OUTPUT;
    MOSI_TRIS = OUTPUT;
    MISO_TRIS = INPUT;
    CS_TRIS = OUTPUT;
    RST_TRIS = OUTPUT;
    
    CS_LAT = TRUE;
    RST_LAT = TRUE;
    
    SSPSTATbits.SMP = 0;
    SSPSTATbits.CKE = 1;
    SSPCON1bits.CKP = 0;
    SSPCON1bits.SSPM = 0b0001; //Fosc/16
    SSPCON1bits.SSPEN = TRUE; //Enable
    
    
}

unsigned char SPITransfer(unsigned char data){
    unsigned char throwaway = SSPBUF;
    SSPBUF = data;
    unsigned char timeout = 100;
    while(!SSPSTATbits.BF && timeout > 0){
        timeout--;
    }
    unsigned char result = SSPBUF;
    if(timeout == 0){
        return 0xff;
    }
    return result;
}