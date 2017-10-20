#include <xc.h>
#include "common.h"
#include "analog.h"
unsigned char readAnalog(unsigned int *result){
    ADCON0bits.CHS = 0b01001; //AN9/RB3
    ADCON1bits.ADCS = 0b101; //Fosc/16
    ADCON1bits.ADFM = 1; //Right-justified
    ADCON0bits.ADON = TRUE; //Turn on ADC
    
    //Assume Tacq is no longer than 10 us (should be 6.87 us at 100 C)
    __delay_us(10);
    unsigned char i = ANALOG_TIMEOUT;
    ADCON0bits.ADGO = TRUE; //Start conversion
    while(i>0){
        if(!ADCON0bits.ADGO){
            break;
        }
        i--;
    }
    if(i>0){ //success
        *result = 0;
        //Load upper bits
        *result |= ((ADRESH & 0b11) << 8);
        //Load lower bits
        *result |= ADRESL;
        return 0;
    }
    else{ //failure
        return 1;
    }
}
