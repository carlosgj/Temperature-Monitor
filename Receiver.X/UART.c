#include "UART.h"

void debug_UART_init(void){
    FIFOInit(&DbgUARTRxBuf); //TODO (shrunk to get FAT code to compile)
    
    //Setup UART pins
    //RB2 is input
    ANSELBbits.ANSELB2 = FALSE;
    TRISBbits.TRISB2 = INPUT;
    U2RXPPS = 0b001010;
    
    //RB1 is output
    ANSELBbits.ANSELB1 = FALSE;
    TRISBbits.TRISB1 = OUTPUT;
    RB1PPS = 0x23;
    
    //Setup BRG
    U2CON0bits.BRGS = TRUE;
    U2BRGH = 0;
    U2BRGL = 138;
    
    //Setup transmitter
    U2CON0bits.TXEN = TRUE;

    //Setup receiver
    U2CON0bits.RXEN = TRUE;
    
    //Turn on port
    U2CON1bits.ON = TRUE;
    
    PIE8bits.U2RXIE = TRUE;
}

void putch(unsigned char theByte){
    U2TXB = theByte;
    while(!U2ERRIRbits.TXMTIF){
    }
}

void __interrupt(irq(U2RX),high_priority) DbgUARTRxISR(void){
    FIFOPush(&DbgUARTRxBuf, U2RXB); //TODO (shrunk to get FAT code to compile)
}
