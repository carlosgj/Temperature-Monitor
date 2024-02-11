#include "UART.h"

void debug_UART_init(void){
    FIFOInit(&DbgUARTRxBuf);
    
    //Setup UART pins
    //RB4 is input
    ANSELBbits.ANSELB4 = FALSE;
    TRISBbits.TRISB4 = INPUT;
    U2RXPPS = 0b001100;
    
    //RB3 is output
    ANSELBbits.ANSELB3 = FALSE;
    TRISBbits.TRISB3 = OUTPUT;
    RB3PPS = 0x23;
    
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
    FIFOPush(&DbgUARTRxBuf, U2RXB);
}
