#include "main.h"

void main(void) {
    if (init() == 0) {
        //setDisplayMode(DISP_MODE_HOME);
        //setGraphMode(GRAPH_MODE_DAY);
        while (1) {
            //run();
        }
    } else {
        printf("Initialization error. System halted.");
        while (TRUE) {

        }
    }
}

unsigned char init(void) {
    uint8_t initError = FALSE;
    uint8_t initErrorCode = 0;

    //Setup basic pin functions
    TRISBbits.TRISB5 = OUTPUT;
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;

    //Setup timing & clocks
    //OSCCON1bits.NDIV = 0b0000;
    //Setup ms counter
    //Osc speed = 64 MHz
    //Instruction speed = 16 MHz
    //Instruction cycle = 0.0625 us
    //1:128 prescaler gives 8 us timer tick
    //125 * 8 us = 1 ms
    //Set timer initial value to 255-125=130 to roll over once per ms
    //T0CON1bits.T0CS = 0b010; //Increment Timer0 on instruction cycle
    //T0CON1bits.T0CKPS = 0b0111; //1:128 prescaler
    //TMR0 = 130;
    //PIE0bits.TMR0IE = TRUE;
    //T0CON0bits.T0EN = TRUE; //Turn on timer

    //INTCON0bits.PEIE = TRUE; //Enable peripheral interrupts
    INTCON0bits.GIE = TRUE; //Enable interrupts
    
    debug_UART_init();
    printf("--------------------------\n");
    printf("Serial initialized.\n");
    printf("Last reset: %d\n", resetCause);
    printf("Temperature Monitor\n");
    printf("SW ver: %s\n", SW_VER_STR);
    printf("Compiled: %s %s\n", __DATE__, __TIME__);
    printf("Git ver: %s\n", GIT_VERSION);

    printf("Initializing SPI...\n");
    SPIInit();
    printf("SPI initialized.\n");

    printf("Initializing display...\n");
    initErrorCode = displayInit();
    if (initErrorCode != 0) {
        printf("Display init error %d!\n", initErrorCode);
        initError = TRUE;
    }
    else{
        printf("Display initialized.\n");
    }

    //Initialize ADC
    printf("Initializing ADC...\n");
    initErrorCode = thermInit();
    if(initErrorCode != 0){
        printf("ADC init error %d!\n", initErrorCode);
        initError = TRUE;
    }
    else{
        printf("ADC initialized.\n");
    }
    
    
    //Initialize RTC & get current timestamp
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

    if(currentYear < 18){
        //Clock was reset
        //PRINT("Time not set! Starting up in safe mode...\n");
        safeMode = TRUE;
    }
    else{
        safeMode = FALSE;
    }

    //Bootstrap memory
    safeMode = FALSE;
    //setEEPROMPageIndex(0x00DC);
    
    if (!initError) {
        //if (loadEEPROMPageIndex()) {
        //    PRINT("Data memory setup failed!\n");
        //    //initError = TRUE;
        //    safeMode = TRUE;
        //}
    }
    //setupEEPROMPage(0, currentYear, currentMonth, currentDay);
    //fillExtMemPage(0);
    //ext_mem_write(0x000005, 0xaa);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    //if(currentEEPROMPage != 0xffff){
    //dumpExtMemPage(currentEEPROMPage);
    //}
    //else{
    //    PRINT("No valid EEPROM page found!\n");
    //}
    
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);

    //Set up buttons
    BUTTON1_TRIS = INPUT;
    BUTTON2_TRIS = INPUT;
    BUTTON3_TRIS = INPUT;
    BUTTON4_TRIS = INPUT;
    BUTTON5_TRIS = INPUT;
    BUTTON6_TRIS = INPUT;

    BUTTON1_WPU = TRUE;
    BUTTON2_WPU = TRUE;
    BUTTON3_WPU = TRUE;
    BUTTON4_WPU = TRUE;
    BUTTON5_WPU = TRUE;
    BUTTON6_WPU = TRUE;

    buttonState.all = 0;
    oldButtonState.all = 0;
    buttonPressed.all = 0;
    
    serialInit();   
    
    safeMode = TRUE;

    return initError;
}

void run(void) {
//    uint16_t oldMinuteOfDay = minuteOfDay;
//    unsigned char oldDay = currentDay;
//    if(messageInProgressFlag){
//        __delay_ms(20);
//        processMessage();
//        currentTemperatureByte = formatTemperatureToChar(tempReadingRaw);
//        asm("NOP");
//    }
//    if (fast_tasks_timer == 0) {
//        fast_tasks_timer = FAST_TASKS_RATE;
//        //getTime();
//        //drawTime();
//        //updateButtons();
//        //handleButtonActions();
//        if(safeMode){
//            //drawSafeMode();
//        }
//    }
//    //drawHomeScreen();
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

void updateButtons(void) {
    oldButtonState.all = buttonState.all;

    buttonState.B1 = !BUTTON1_PORT;
    buttonState.B2 = !BUTTON2_PORT;
    buttonState.B3 = !BUTTON3_PORT;
    buttonState.B4 = !BUTTON4_PORT;
    buttonState.B5 = !BUTTON5_PORT;
    buttonState.B6 = !BUTTON6_PORT;

    buttonPressed.all = buttonState.all & ~oldButtonState.all;
}

void handleButtonActions(void) {
//    //If any button is pressed when asleep, wake up
//    if (buttonPressed.all != 0 && isSleep) {
//        setSleep(FALSE);
//        __delay_ms(250);
//        return;
//    }
//    if (buttonPressed.B1) {
//        switch (currentDisplayMode) {
//            case DISP_MODE_HOME:
//                setGraphMode(GRAPH_MODE_DAY);
//                break;
//            case DISP_MODE_UTIL:
//                //Switch to home
//                setDisplayMode(DISP_MODE_HOME);
//                break;
//            case DISP_MODE_SETTIME:
//                //Switch to home
//                setDisplayMode(DISP_MODE_HOME);
//                break;
//        }
//        return;
//    }
//
//    if (buttonPressed.B2) {
//        switch (currentDisplayMode) {
//            case DISP_MODE_HOME:
//                //Switch to past-week plot
//                setGraphMode(GRAPH_MODE_WEEK);
//                break;
//            case DISP_MODE_UTIL:
//                //Switch to time-set screen
//                setDisplayMode(DISP_MODE_SETTIME);
//                getTime();
//                pros_seconds.all = seconds_reg.all;
//                pros_minutes.all = minutes_reg.all;
//                pros_hours.all = hours_reg.all;
//                pros_date.all = date_reg.all;
//                pros_month.all = month_reg.all;
//                pros_years.all = years_reg.all;
//                activeTimeChar = ACTIVE_TIME_10YR;
//                drawProspectiveTime();
//                break;
//            case DISP_MODE_SETTIME:
//                setTime();
//                setDisplayMode(DISP_MODE_HOME);
//                break;
//        }
//        return;
//    }
//
//    if (buttonPressed.B3) {
//        switch (currentDisplayMode) {
//            case DISP_MODE_HOME:
//                //Switch to past-month plot
//                setGraphMode(GRAPH_MODE_MONTH);
//                break;
//            case DISP_MODE_UTIL:
//                //Force start RTC
//                RTCOscRestart();
//                break;
//            case DISP_MODE_SETTIME:
//                //Move cursor left
//                if (activeTimeChar == 0) {
//                    activeTimeChar = ACTIVE_TIME_SECOND;
//                } else {
//                    activeTimeChar--;
//                }
//                drawProspectiveTime();
//                break;
//            case DISP_MODE_VERIFY_RESET:
//                //"No" selected
//                setDisplayMode(DISP_MODE_HOME);
//                break;
//        }
//        return;
//    }
//
//    if (buttonPressed.B4) {
//        switch (currentDisplayMode) {
//            case DISP_MODE_HOME:
//                //Switch to past-year plot
//                setGraphMode(GRAPH_MODE_YEAR);
//                break;
//            case DISP_MODE_UTIL:
//                //Reset
//                RESET();
//                break;
//            case DISP_MODE_SETTIME:
//                //Move cursor right
//                if (activeTimeChar == ACTIVE_TIME_SECOND) {
//                    activeTimeChar = 0;
//                } else {
//                    activeTimeChar++;
//                }
//                drawProspectiveTime();
//                break;
//            case DISP_MODE_VERIFY_RESET:
//                //"Yes" selected
//                safeMode = FALSE;
//                //resetEEPROMPageIndex();
//                RESET();
//                break;
//        }
//        return;
//    }
//
//    if (buttonPressed.B5) {
//        switch (currentDisplayMode) {
//            case DISP_MODE_HOME:
//                //Go to util screen
//                setDisplayMode(DISP_MODE_UTIL);
//                break;
//            case DISP_MODE_UTIL:
//                break;
//            case DISP_MODE_SETTIME:
//                //Increment
//                incrementActiveChar();
//                drawProspectiveTime();
//                break;
//        }
//        return;
//    }
//
//    if (buttonPressed.B6) {
//        switch (currentDisplayMode) {
//            case DISP_MODE_HOME:
//                //Turn off screen
//                setSleep(TRUE);
//                break;
//            case DISP_MODE_UTIL:
//                setDisplayMode(DISP_MODE_VERIFY_RESET);
//                break;
//            case DISP_MODE_SETTIME:
//                //Decrement
//                decrementActiveChar();
//                drawProspectiveTime();
//                break;
//        }
//        return;
//    }
//
//    //If a button was pressed, do the action, then delay to ignore bounces
//    if (buttonPressed.all != 0) {
//        __delay_ms(300);
//    }
}

//void interrupt ISR(void) {
//    if(PIR3bits.RC1IF){
//        messageBuffer[messageBufferIdx++] = RC1REG;
//        PIR3bits.RC1IF = FALSE;
//        messageInProgressFlag = TRUE;
//        return;
//    }
//    if (PIR0bits.TMR0IF) {
//        LATBbits.LATB5 = !LATBbits.LATB5;
//        PIR0bits.TMR0IF = FALSE;
//        ms_count++;
//        TMR0 = 130;
//        return;
//    }
//}

void getResetCause(void){
    //Attempt to figure out what caused reset
    if(!PCON1bits.nMEMV){
        //Memory violation
        resetCause = RS_MEM;
        PCON1bits.nMEMV = TRUE;
        return;
    }
    
    if(PCON0bits.STKOVF){
        //Stack overflow
        resetCause = RS_SO;
        PCON0bits.STKOVF = FALSE;
        return;
    }
    
    if(PCON0bits.STKUNF){
        //Stack underflow
        resetCause = RS_SU;
        PCON0bits.STKUNF = FALSE;
        return;
    }
    
    if(!PCON0bits.nWDTWV){
        //WDT window violation
        resetCause = RS_WIN;
        PCON0bits.nWDTWV = TRUE;
        return;
    }
    
    if(!PCON0bits.nRWDT){
        //WDT reset
        resetCause = RS_WDT;
        PCON0bits.nRWDT = TRUE;
        return;
    }
    
    if(!PCON0bits.nRMCLR){
        //MCLR
        resetCause = RS_MCLR;
        PCON0bits.nRMCLR = TRUE;
        return;
    }
    
    if(!PCON0bits.nPOR){
        //Power-on reset
        resetCause = RS_POR;
        PCON0bits.nPOR = TRUE;
        return;
    }
    
    if(!PCON0bits.nBOR){
        //Brownout
        resetCause = RS_BOR;
        PCON0bits.nBOR = TRUE;
        return;
    }
    
}


void __interrupt(irq(default),high_priority) DefaultISR(unsigned char id){
    status.unhandledIntCount++;
    status.lastUnhandledInt = id;
    //printf("Unhandled interrupt!\n");
}
