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
#include "MDD_File_System/SD-SPI.h"
#include "MDD_File_System/FSIO.h"
#include "time.h"

#define MINOR_CYCLE_TIME_LIMIT_MS   100
#define MAJOR_CYCLE_LENGTH  10

unsigned char init(void);
void run(void);
void updateButtons(void);
void handleButtonActions(void);
void getResetCause(void);

#endif

