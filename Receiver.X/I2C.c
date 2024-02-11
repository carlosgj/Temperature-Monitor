#include <xc.h>
#include "I2C.h"

uint8_t I2C_init(void){
    uint8_t status = 0;
    
    ANSELCbits.ANSELC3 = 0;
    ANSELCbits.ANSELC4 = 0;
    
    //Set up pins
    I2C_SCL_TRIS = OUTPUT;
    I2C_SDA_TRIS = OUTPUT;
    LATCbits.LATC3 = FALSE;
    LATCbits.LATC4 = FALSE;
    
    ODCONCbits.ODCC3 = TRUE;
    ODCONCbits.ODCC4 = TRUE;
    
    I2C1SCLPPS = 0b010011;
    I2C1SDAPPS = 0b010100;
    
    //Set PPS Inputs
    RC3PPS = 0x37;
    RC4PPS = 0x38;
    
    RC3I2Cbits.TH = 0b01; //Standard I2C
    RC3I2Cbits.PU = 0b10; //???
    
    RC4I2Cbits.TH = 0b01; //Standard I2C
    RC4I2Cbits.PU = 0b10; //???
    
    I2C1CON0bits.MODE = 0b100; //Host mode, 7-bit addressing
    I2C1CON1bits.ACKCNT = TRUE;
    
    I2C1CLK = 0; //Fosc/4 (16 MHz)
    I2C1BAUD = 31; //About 100 kHz
    
    I2C1BTO = 0x00;
    I2C1ERR = 0x00;
    
    I2C1CON0bits.EN = TRUE; //Turn on
    
    __delay_ms(5);
    
    //Check that bus is free
    if(!I2C1STAT0bits.BFRE){
        printf("I2C bus not free after init!\n");
        status = 1;
    }
    
    
    return status;
}

uint8_t I2C_read(uint8_t address, uint8_t *data, uint8_t count){
    uint8_t status = 0;
    uint8_t i = 0;
    uint8_t addr_byte = address << 1;
    addr_byte |= 0b1;
    
    I2C1ADB1 = addr_byte;
    I2C1CNTL = count;
    
    I2C1CON0bits.S = TRUE;
    
    while (I2C1CON0bits.S){
        //TODO: timeout
    }
    
    //While in host mode...
    while (I2C1STAT0bits.MMA){
        if (I2C1STAT1bits.RXBF){
            //Read Data
            data[i] = I2C1RXB;
            i++;
        }
    }
    
    if(I2C1CNTL != 0){
        status = 2;
    }
    return status;
}

uint8_t I2C_write(uint8_t address, uint8_t *data, uint8_t count){
    uint8_t status = 0;
    uint8_t i = 1;
    uint8_t addr_byte = address << 1;
    I2C1ADB1 = addr_byte;
    
    I2C1TXB = data[0];
    I2C1CNTL = count;
    
    I2C1CON0bits.S = 1;
    
    while (I2C1CON0bits.S){
        //TODO: timeout
    }
    
    while (I2C1STAT0bits.MMA){
        if (I2C1CON0bits.MDR){
            if (I2C1STAT1bits.TXBE){
                if (i >= count){
                    //Somehow we got here... probably a junk byte
                    I2C1TXB = 0x00;
                    return 0;
                }
                else{
                    //Load next byte
                    I2C1TXB = data[i++];
                }
            }
        }
    }
        
    if(I2C1CNTL != 0){
        status = 2;
    }
    return status;
}