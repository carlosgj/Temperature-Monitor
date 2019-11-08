#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define FAST_TASKS_RATE 100
#define SLOW_TASKS_RATE 10000

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

union buttonReg buttonState;
union buttonReg oldButtonState;
union buttonReg buttonPressed;

unsigned int fast_tasks_timer = FAST_TASKS_RATE;
unsigned int slow_tasks_timer = SLOW_TASKS_RATE;

#endif

