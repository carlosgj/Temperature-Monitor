
#include <xc.h>
#include "therm.h"
#include "display.h"

unsigned char thermInit(void){
    unsigned char result = FALSE;
    unsigned char timeout;
    THERM_TRIS = INPUT;
    THERM_ANSEL = TRUE;
    
    //Setup ADC
    ADPCH = 0b010101;
    ADCON0bits.ADCS = FALSE; //Use scaled Fosc
    ADCLK = 0b111111; //Fosc/128
    ADCON0bits.ADFM = TRUE; //Right-justify result
    
    //Setup FVR
    FVRCONbits.ADFVR = 0b10;
    FVRCONbits.FVREN = TRUE; //Enable FVR
    timeout = FVR_TIMEOUT;
    while(!FVRCONbits.FVRRDY){
        timeout--;
        if(timeout == 0){
            PRINT("FVR initialization failed!");
            return TRUE;
        }
    }
    
    ADREFbits.ADPREF = 0b11; //Connect ADC positive reference to FVR
    
    ADCON0bits.ADON = TRUE; //Turn on ADC
    
    return result;
}

unsigned char takeReading(unsigned int *result_p){
    unsigned int result = 0;
    ADCON0bits.ADGO = TRUE; //Start conversion
    while(ADCON0bits.ADGO){
        
    }
    result = ADRESH;
    result <<= 8;
    result |= ADRESL;
    *result_p = result;
    return FALSE;
}

unsigned char formatTemperatureToChar(unsigned int rawReading){
    //10-bit ADC
    //0x3ff = 2.048 V (per FVR setting)
    //MCP9701: V0c = 400 mV, Tc = 19.5 mV/*C
    //Tf = raw * 0.1846 - 4.9231
    float exactResult = ((float)rawReading * 0.1846) - 4.9231;
    unsigned char result = (unsigned char)exactResult;
    float remainder = exactResult - result;
    result <<= 1;
    if(remainder >= 0.5){
        result |= 1;
    }
    return result;
}

void getTemperature(void){
    unsigned int rawReading;
    unsigned int accumulator = 0;
    unsigned int i;
    unsigned char failure = FALSE;
    for(i= 0; i<4; i++){
        failure |= takeReading(&rawReading);
        __delay_ms(10);
        if(!failure){
            accumulator += rawReading;
        }
        else{
            //Error handling?
            break;
        }
    }
    if(!failure){
        accumulator >>= 2;
        currentTemperatureByte = formatTemperatureToChar(accumulator);
    }
}