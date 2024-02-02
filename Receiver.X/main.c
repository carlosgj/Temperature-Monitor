#include "main.h"

void main(void) {
    uint8_t initStatus = init();
    if (initStatus < 2) {
        //setDisplayMode(DISP_MODE_HOME);
        //setGraphMode(GRAPH_MODE_DAY);
        while (1) {
            //run();
        }
    } else {
        printf("Critical initialization error(s). System halted.");
        while (TRUE) {

        }
    }
}

/* Initialization
 * Returns:
 * 0: Success
 * 1: Noncritical errors; start running main loop
 * 2: Critical errors: halt 
 */
uint8_t init(void) {
    uint8_t initError = FALSE;
    uint8_t initErrorCode = 0;

    getResetCause();

    //Setup basic pin functions
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;

    //INTCON0bits.PEIE = TRUE; //Enable peripheral interrupts
    INTEN; //Enable interrupts

    debug_UART_init();
    printf("\n--------------------------\n");
    printf("Serial initialized.\n");
    printf("Last reset: %d\n", resetCause);
    printf("MG Temperature Monitor\n");
    printf("SW ver: %s\n", SW_VER_STR);
    printf("Compiled: %s %s\n", __DATE__, __TIME__);
    printf("Git ver: %s\n", GIT_VERSION);
    printf("\n");

    timerInit();

    printf("Initializing SPI...\n");
    SPIInit();
    printf("SPI initialized.\n\n");

    printf("Initializing display...\n");
    initErrorCode = displayInit();
    if (initErrorCode != 0) {
        printf("Display init error %d!\n", initErrorCode);
        if(initError == 0){
            initError = 2;
        }
    } else {
        printf("Display initialized.\n");
    }
    printf("\n");

    //Initialize ADC
    printf("Initializing ADC...\n");
    initErrorCode = thermInit();
    if (initErrorCode != 0) {
        printf("ADC init error %d!\n", initErrorCode);
        if(initError == 0){
            initError = 1;
        }
    } else {
        printf("ADC initialized.\n");
    }
    printf("\n");

    //Initialize I2C
    printf("Initializing I2C...\n");
    initErrorCode = I2C_init();
    if (initErrorCode != 0) {
        printf("I2C init error %d!\n", initErrorCode);
        if(initError == 0){
            initError = 1;
        }
    } else {
        printf("I2C initialized.\n");
    }
    printf("\n");

    if (initErrorCode == 0) {
        //Don't bother trying to init RTC if I2C failed
        
        datetime_tests();

        //Initialize RTC & get current timestamp
        printf("Initializing RTC...\n");
        //DEBUG CODE!
        //currentYear = 18;
        //currentMonth = 12;
        //currentDay = 18;
        //currentHour = 18;
        //getTime();

        //Verify that current date has been loaded
        //if (currentYear == 0xff || currentMonth == 0xff ||
        //        currentDay == 0xff || currentHour == 0xff) {
        //    PRINT("Current date not correctly loaded from RTC!\n");
        //    initError = TRUE;
        //}
        //    else{
        //        char datetime[2];
        //        //PRINT("Date: 0x");
        //        itoh8(years_reg.all, datetime);
        //        RA8875_textWrite(datetime, 2);
        //        //PRINT("-0x");
        //        itoh8(month_reg.all, datetime);
        //        RA8875_textWrite(datetime, 2);
        //        //PRINT("-0x");
        //        itoh8(date_reg.all, datetime);
        //        RA8875_textWrite(datetime, 2);
        //        //PRINT(" 0x");
        //        itoh8(hours_reg.all, datetime);
        //        RA8875_textWrite(datetime, 2);
        //        //PRINT(":0x");
        //        itoh8(minutes_reg.all, datetime);
        //        RA8875_textWrite(datetime, 2);
        //        //PRINT(":0x");
        //        itoh8(seconds_reg.all, datetime);
        //        RA8875_textWrite(datetime, 2);
        //        //PRINT("\n");
        //    }

        if (currentYear < 18) {
            //Clock was reset
            //PRINT("Time not set! Starting up in safe mode...\n");
            safeMode = TRUE;
        } else {
            safeMode = FALSE;
        }
    } else {
        printf("Skipping RTC init due to I2C failure.\n");
    }
    printf("\n");

    //Initialize SD card
    printf("Initializing SD card driver...\n");
    initErrorCode = FSInit();
    if (initErrorCode != 0) {
        printf("SD card driver init error %d!\n", initErrorCode);
        if(initError == 0){
            initError = 1;
        }
    } else {
        printf("SD card driver initialized.\n");
    }
    printf("\n");

    //    printf("Running FAT demo\n");
    //    uint16_t foo = MDD_SDSPI_ReadSectorSize();
    //    printf("Sector size: %d\n", foo);
    //    
    //    TRISCbits.TRISC7 = OUTPUT;
    //    
    //    uint8_t i;
    //    uint16_t startTime, oldTime, newTime, endTime;
    //    int16_t closeResult;
    //    uint8_t dataToWrite[12];
    //    uint8_t len;
    //    FSFILE *fob;
    //    for(i=0; i<2; i++){
    //        getMillis(&startTime);
    //        LATCbits.LATC7 = !LATCbits.LATC7;
    //        printf("Opening file...\n");
    //        getMillis(&oldTime);
    //        fob = FSfopen("test", "a+");
    //        getMillis(&newTime);
    //        printf("File opened; pointer=%p.\n", fob);
    //        printf("Error: %d\n", FSerror());
    //        printf("Open time: %d ms\n", newTime - oldTime);
    //        
    //        printf("Writing to file...\n");
    //        getMillis(&oldTime);
    //        len = sprintf(dataToWrite, "Line %d\n", i);
    //        initErrorCode = (uint8_t)FSfwrite(dataToWrite, (size_t)1, (size_t)len, fob);
    //        getMillis(&newTime);
    //        printf("Write result: %d\n", initErrorCode);
    //        printf("Write time: %d ms\n", newTime - oldTime);
    //        
    //        getMillis(&oldTime);
    //        closeResult = FSfclose(fob);
    //        getMillis(&newTime);
    //        printf("Close result: %d\n", closeResult);
    //        printf("Close time: %d ms\n", newTime - oldTime);
    //        getMillis(&endTime);
    //        printf("Total Time: %d\n", endTime - startTime);
    //    }
    //    
    //    printf("FAT demo done.\n\n");

    printf("Initializing RFM69...\n");
    initErrorCode = RFM69_initialize(0);
    if (initErrorCode != 0) {
        printf("RFM69 init error %d!\n", initErrorCode);
        if(initError == 0){
            initError = 1;
        }
    } else {
        printf("RFM69 initialized.\n");
    }
    printf("\n");


    if (initError != 0) {
        printf("Setting safe mode!\n");
        safeMode = TRUE;
    }

    return initError;
}

void run(void) {
    uint16_t oldMinuteOfDay = minuteOfDay;
    unsigned char oldDay = currentDay;
    if (messageInProgressFlag) {
        __delay_ms(20);
        processMessage();
        currentTemperatureByte = formatTemperatureToChar(tempReadingRaw);
        asm("NOP");
    }
    if (fast_tasks_timer == 0) {
        fast_tasks_timer = FAST_TASKS_RATE;
        //getTime();
        //drawTime();
        //updateButtons();
        //handleButtonActions();
        if (safeMode) {
            //drawSafeMode();
        }
    }
    //    drawHomeScreen();
    //    if(slow_tasks_timer == 0){
    //        slow_tasks_timer = SLOW_TASKS_RATE;
    //        //drawTemp();
    //        if (currentDisplayMode == DISP_MODE_HOME) {
    //            switch(currentGraphMode){
    //                case GRAPH_MODE_DAY:
    //                    //clearPlotXLabels();
    //                    //drawPlotDayXLabels();
    //                    //collectDayData();
    //                    //plotTemp(240);
    //                    break;
    //                case GRAPH_MODE_WEEK:
    //                    //clearPlotXLabels();
    //                    //drawPlotWeekXLabels();
    //                    //collectWeekData();
    //                    //plotTemp(336);
    //                    break;
    //                case GRAPH_MODE_MONTH:
    //                    //clearPlotXLabels();
    //                    //drawPlotMonthXLabels();
    //                    //collectMonthData();
    //                    //plotTemp(360);
    //                    break;
    //                case GRAPH_MODE_YEAR:
    //                    //clearPlotXLabels();
    //                    //drawPlotYearXLabels();
    //                    //collectYearData();
    //                    //plotTemp(365);
    //                    break;    
    //                    
    //            }
    //    }
    //    }
    //    
    //    fast_tasks_timer--;
    //    slow_tasks_timer--;
    //    
    //    //if(oldMinuteOfDay != minuteOfDay){
    //    //    if((minuteOfDay % 6 == 0)){
    //            //Write temperature to memory
    //            //unsigned char sampleIndex = minuteOfDay / 6;
    //            //ext_mem_write((currentEEPROMPage<<8)+2+sampleIndex , currentTemperatureByte);
    //    //    }
    //    //}
    //    if(oldDay != currentDay){
    //        //incrementEEPROMPageIndex();
    //    }
    //    
    //    //pros_seconds.all = 0;
    //    //pros_minutes.all = 0;
    //    //pros_hours.all = 0;
    //    //pros_date.all = 0;
    //    //pros_month.all = 0;
    //    //pros_years.all = 0;
    //    //setTime();
    //    //volatile unsigned char foo = readRTCReg(REG_CONTROL);
    //    //foo = readRTCReg(REG_STAT);
    //    //readAll();
    //    //writeRTCReg(REG_CONTROL, 0b10000100); //Enable write
    //    //foo = readRTCReg(REG_CONTROL);
    //    //writeRTCReg(REG_CONTROL, 0b00000100); //Enable oscillator
    //    //foo = readRTCReg(REG_CONTROL);
    //    __delay_ms(1);
}

void getResetCause(void) {
    //Attempt to figure out what caused reset
    if (!PCON1bits.nMEMV) {
        //Memory violation
        resetCause = RS_MEM;
        PCON1bits.nMEMV = TRUE;
        return;
    }

    if (PCON0bits.STKOVF) {
        //Stack overflow
        resetCause = RS_SO;
        PCON0bits.STKOVF = FALSE;
        return;
    }

    if (PCON0bits.STKUNF) {
        //Stack underflow
        resetCause = RS_SU;
        PCON0bits.STKUNF = FALSE;
        return;
    }

    if (!PCON0bits.nWDTWV) {
        //WDT window violation
        resetCause = RS_WIN;
        PCON0bits.nWDTWV = TRUE;
        return;
    }

    if (!PCON0bits.nRWDT) {
        //WDT reset
        resetCause = RS_WDT;
        PCON0bits.nRWDT = TRUE;
        return;
    }

    if (!PCON0bits.nRMCLR) {
        //MCLR
        resetCause = RS_MCLR;
        PCON0bits.nRMCLR = TRUE;
        return;
    }

    if (!PCON0bits.nPOR) {
        //Power-on reset
        resetCause = RS_POR;
        PCON0bits.nPOR = TRUE;
        return;
    }

    if (!PCON0bits.nBOR) {
        //Brownout
        resetCause = RS_BOR;
        PCON0bits.nBOR = TRUE;
        return;
    }

}

void __interrupt(irq(default), high_priority) DefaultISR(unsigned char id) {
    status.unhandledIntCount++;
    status.lastUnhandledInt = id;
    //printf("Unhandled interrupt!\n");
}
