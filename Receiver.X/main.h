#ifndef MAIN_H
#define	MAIN_H

#include "common.h"
#include "RFM69.h"
#include "RFM69registers.h"
#include "Adafruit_RA8875.h"
#include "display.h"
#include "therm.h"
#include "serial.h"
#include "config.h"
#include "UART.h"
#include "SPI.h"
#include "I2C.h"

#define FAST_TASKS_RATE 100
#define SLOW_TASKS_RATE 10000

unsigned char init(void);
void run(void);
void updateButtons(void);
void handleButtonActions(void);
void getResetCause(void);

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

