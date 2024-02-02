#include "time.h"

void timerInit(void){
#ifdef MSOUT
    //TRISAbits.TRISA0 = OUTPUT;
    //LATAbits.LATA0 = FALSE;
#endif
    
    //Utility (TMR0)
    T0CON0bits.T016BIT = FALSE;
    T0CON1bits.T0CS = 0b010; //Fosc/4
    T0CON1bits.ASYNC = TRUE;
    T0CON1bits.T0CKPS = 0b0110; //1:128
    TMR0H = 250;
    PIR3bits.TMR0IF = FALSE;
    T0CON0bits.EN = TRUE;
    
    //MS clock (TMR1)
    T1CLKbits.CS = 0b00001; //Fosc/4
    T1CONbits.CKPS = 0b11; //8x prescaler
    //T1CONbits.RD16 = TRUE;
    TMR1H = (unsigned char)(MS_TMR_VAL >> 8);
    TMR1L = (unsigned char)(MS_TMR_VAL & 0xff);
    PIE3bits.TMR1IE = TRUE;
    PIR3bits.TMR1IF = FALSE;
    T1CONbits.ON = TRUE;
    
    
}

inline void getMillis(uint16_t *destination){
    INTDIS;
    *destination = monoMillis;
    INTEN;
}

//Implement ms counter
void __interrupt(irq(TMR1), high_priority) TMR1ISR(void){
    milliseconds++;
    monoMillis++;
    if(milliseconds == 1000){
        seconds++;
        milliseconds = 0;
    }
    TMR1H = (unsigned char)(MS_TMR_VAL >> 8);
    TMR1L = (unsigned char)(MS_TMR_VAL & 0xff);
#ifdef MSOUT
    X3LAT = !X3LAT;
#endif
    PIR3bits.TMR1IF = FALSE;
}
