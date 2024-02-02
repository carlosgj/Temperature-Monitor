#include "SPI.h"

void SPIInit(void){
    //MSSP1 setup
    //SCK1_TRIS = OUTPUT;
    //MISO1_TRIS = INPUT;
    //MOSI1_TRIS = OUTPUT;
    //DISP_CS_TRIS = OUTPUT;
    //DISP_CS_LAT = TRUE;
    
    //Set up inputs
    //SSP1CLKPPS = 0b10011; //SSP1 clk in on C3
    //SSP1DATPPS = 0b10100; //SSP1 MISO on C4 
    
    //Set up outputs
    //RC3PPS = 0x0F; //MSSP1 SCK on C3
    //RB3PPS = 0x10; //MSSP1 SDO on B3
    
    //SSP1STATbits.SMP = 0;
    //SSP1STATbits.CKE = 1;
    //SSP1CON1bits.CKP = 0;
    //SSP1CON1bits.SSPM = 0b0010; //Fosc/64
    //SSP1CON3bits.BOEN = TRUE;
    //SSP1CON1bits.SSPEN = TRUE; //Enable
    
    //MSSP 2 setup
    SCK2_TRIS = OUTPUT;
    MOSI2_TRIS = OUTPUT;
    MISO2_TRIS = INPUT;
    
    SD_CS_LAT = TRUE;
    SD_CS_TRIS = OUTPUT;
    
    //Set up inputs
    //SPI2SCKPPS = 0b011001; //SPI2 clk on D1
    SPI2SDIPPS = 0b011010; //SSP2 MISO on D2 
    
    //Set up outputs
    RD1PPS = 0x34; //SPI2 clk out on D1
    RD3PPS = 0x35; //SPI22 MOSI on D3
}

void SPI2_Open_SDSlow(void){
    if(SPI2CON0bits.EN){
        printf("Warning! SPI2 was already open when Open_SDSlow was called.\n");
        SPI2CON0bits.EN = FALSE;
    }
    SPI2CON0bits.MST = TRUE; //Master
    SPI2CON0bits.BMODE = TRUE;
    SPI2CON0bits.LSBF = FALSE;
    SPI2CON1bits.SMP = 0;
    SPI2CON1bits.CKE = 1;
    SPI2CON1bits.CKP = 0;
    SPI2CON2bits.RXR = TRUE;
    SPI2CON2bits.TXR = TRUE;
    //SSP2CON1bits.SSPM = 0b0010; //Fosc/64
    //SSP2CON3bits.BOEN = TRUE;
    SPI2CLK = 0; //Fosc
    SPI2BAUD = 100;
    SPI2CON0bits.EN = TRUE; //Enable
}

void SPI2_Open_SDFast(void){
    if(SPI2CON0bits.EN){
        printf("Warning! SPI2 was already open when Open_SDFast was called.\n");
        SPI2CON0bits.EN = FALSE;
    }
    SPI2CON0bits.MST = TRUE; //Master
    SPI2CON0bits.BMODE = TRUE;
    SPI2CON0bits.LSBF = FALSE;
    SPI2CON1bits.SMP = 0;
    SPI2CON1bits.CKE = 1;
    SPI2CON1bits.CKP = 0;
    SPI2CON2bits.RXR = TRUE;
    SPI2CON2bits.TXR = TRUE;
    //SSP2CON1bits.SSPM = 0b0010; //Fosc/64
    //SSP2CON3bits.BOEN = TRUE;
    SPI2CLK = 0; //Fosc
    SPI2BAUD = 10;
    SPI2CON0bits.EN = TRUE; //Enable
}

void SPI2_Open_RFM69(void){
    if(SPI2CON0bits.EN){
        printf("Warning! SPI2 was already open when Open_RFM69 was called.\n");
        SPI2CON0bits.EN = FALSE;
    }
    SPI2CON0bits.MST = TRUE; //Master
    SPI2CON0bits.BMODE = TRUE;
    SPI2CON0bits.LSBF = FALSE;
    SPI2CON1bits.SMP = 0;
    SPI2CON1bits.CKE = 1;
    SPI2CON1bits.CKP = 0;
    SPI2CON2bits.RXR = TRUE;
    SPI2CON2bits.TXR = TRUE;
    //SSP2CON1bits.SSPM = 0b0010; //Fosc/64
    //SSP2CON3bits.BOEN = TRUE;
    SPI2CLK = 0; //Fosc
    SPI2BAUD = 100;
    SPI2CON0bits.EN = TRUE; //Enable
}

void SPI2_Close(void){
    SPI2CON0bits.EN = FALSE; //Disable
}

uint8_t SPI2Transfer(uint8_t data){
    uint8_t timeout = 100;
    uint8_t result = 0;
    SPI2TXB = data;
    asm("NOP");
    asm("NOP");
    while(SPI2CON2bits.BUSY && timeout > 0){
        timeout--;
    }
    result = SPI2RXB;
    if(timeout == 0){
        printf("SPI timeout!\n");
        return 0xff;
    }
    return result;
}

uint8_t SD_ExchangeByte(uint8_t data){
    uint8_t temp_result;
    temp_result = SPI2Transfer(data);
    //printf("SD: < %X\n", temp_result);
    return temp_result;
}

void SD_ExchangeBlock(uint8_t *data, uint16_t len){
    uint8_t temp_result;
    uint16_t i;
    for(i=0; i<len; i++){
        temp_result = SPI2Transfer(data[i]);
        data[i] = temp_result;
    }
}

unsigned char SPI1Transfer(unsigned char data){
    uint8_t timeout = 100;
    uint8_t result = 0;
    //SSP1BUF = data;
    //while(!SSP1STATbits.BF && timeout > 0){
    //    timeout--;
    //}
    //unsigned char result = SSP1BUF;
    if(timeout == 0){
        return 0xff;
    }
    return result;
}

void setSSP2CKE(unsigned char newVal){
    //unsigned char oldVal = SSP2STATbits.CKE;
    //if(newVal == oldVal){
    //    return;
    //}
    //else{
    //    SSP2CON1bits.SSPEN = FALSE;
    //    SSP2STATbits.CKE = newVal;
    //    SSP2CON1bits.SSPEN = TRUE;
    //    __delay_us(100);
    //}
}
