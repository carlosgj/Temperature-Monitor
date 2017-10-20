// PIC16F1827 Configuration Bit Settings

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config WDTE = ON        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#include <xc.h>
#include "common.h"
#include "main.h"
#include "analog.h"
#include "comm.h"

unsigned int rawTemp;

void main(void) {
    init();
    while(TRUE){
        loop();
    }
}

void init(void){
    //Setup pins
    TRISAbits.TRISA0 = INPUT;
    TRISAbits.TRISA1 = INPUT;
    TRISAbits.TRISA2 = INPUT;
    TRISAbits.TRISA3 = INPUT;
    TRISAbits.TRISA4 = INPUT;
    
    TRISBbits.TRISB5 = OUTPUT;
    
    TRISBbits.TRISB3 = INPUT;
    ANSELBbits.ANSB3 = TRUE;
}

void loop(void){
    //Get new reading
    unsigned char ADCsuccess = 255;
    unsigned char ADCattempts = 0;
    while(ADCsuccess != 0 && ADCattempts < MAX_ADC_ATTEMPTS){
        ADCsuccess = readAnalog(&rawTemp);
        ADCattempts++;
    }
    if(ADCsuccess == 0){
        //Transmit reading
        sendReading(rawTemp);
    }
    else{
        //Alert base station of ADC failure
        
    }
    sendStatus();
    unsigned int countdown = TRANSMISSION_INTERVAL;
    while(countdown > 0){
        __delay_ms(1000);
        countdown--;
    }
}