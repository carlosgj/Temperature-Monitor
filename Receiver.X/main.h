#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  

unsigned char init(void);
void run(void);
void updateButtons(void);
void handleButtonActions(void);

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

unsigned char radioStatus = 0xff;
union buttonReg buttonState;
union buttonReg oldButtonState;
union buttonReg buttonPressed;

#endif

