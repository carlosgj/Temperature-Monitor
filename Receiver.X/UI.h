#ifndef UI_H
#define	UI_H

#include "common.h"
#include "display.h"

union buttonReg{
    unsigned char all;
    struct{
        unsigned B1 :1;
        unsigned B2 :1;
        unsigned B3 :1;
        unsigned B4 :1;
        unsigned B5 :1;
        unsigned B6 :1;
        unsigned RES:2;
    };
};

union buttonReg buttonState;
union buttonReg oldButtonState;
union buttonReg buttonPressed;

uint8_t UI_init(void);
void UI_periodic(uint8_t minorCycle);
void updateButtons(void);
void handleButtonActions(void);

#endif	/* XC_HEADER_TEMPLATE_H */

