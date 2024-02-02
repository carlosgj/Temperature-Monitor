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
#include "UI.h"
//#include "sd_spi/sd_spi.h"
//#include "fatfs/fatfs_demo.h"
#include "MDD_File_System/SD-SPI.h"
#include "MDD_File_System/FSIO.h"
#include "time.h"

#define FAST_TASKS_RATE 100
#define SLOW_TASKS_RATE 10000

unsigned char init(void);
void run(void);
void updateButtons(void);
void handleButtonActions(void);
void getResetCause(void);

unsigned int fast_tasks_timer = FAST_TASKS_RATE;
unsigned int slow_tasks_timer = SLOW_TASKS_RATE;

#endif

