
#include <xc.h>
#include "therm.h"
#include "display.h"

//This is now blank, with serial data...
unsigned char thermInit(void){
    return FALSE;
}

unsigned char formatTemperatureToChar(unsigned int rawReading){
    //float exactResult = ((float)rawReading * 0.1125f) + 32;
    unsigned char result = 0; //TODO
    //if(exactResult >= 255.0){
    //    result = 255;
    //}
    //else{
    //    result = (unsigned char)exactResult;
    //}
    //float remainder = exactResult - result;
    //result <<= 1;
    //if(remainder >= 0.5){
    //    result |= 1;
    //}
    return result;
}