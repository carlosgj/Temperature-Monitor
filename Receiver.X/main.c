// PIC18F47K40 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config FEXTOSC = ECH    // External Oscillator mode Selection bits (EC (external clock) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = HFINTOSC_64MHZ// Power-up default value for COSC bits (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG1H
#pragma config CLKOUTEN = ON    // Clock Out Enable bit (CLKOUT function is enabled)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // Master Clear Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (Power up timer disabled)
#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown Out Reset Voltage selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)
#pragma config WRT4 = OFF       // Write Protection Block 3 (Block 4 (010000-013FFFh) not write-protected)
#pragma config WRT5 = OFF       // Write Protection Block 3 (Block 5 (014000-017FFFh) not write-protected)
#pragma config WRT6 = OFF       // Write Protection Block 3 (Block 6 (018000-01BFFFh) not write-protected)
#pragma config WRT7 = OFF       // Write Protection Block 3 (Block 7 (01C000-01FFFFh) not write-protected)

// CONFIG4H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-30000Bh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config SCANE = ON       // Scanner Enable bit (Scanner module is available for use, SCANMD bit can control the module)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // UserNVM Program Memory Code Protection bit (UserNVM code protection disabled)
#pragma config CPD = OFF        // DataNVM Memory Code Protection bit (DataNVM code protection disabled)

// CONFIG5H

// CONFIG6L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR4 = OFF      // Table Read Protection Block 4 (Block 4 (010000-013FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR5 = OFF      // Table Read Protection Block 5 (Block 5 (014000-017FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR6 = OFF      // Table Read Protection Block 6 (Block 6 (018000-01BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR7 = OFF      // Table Read Protection Block 7 (Block 7 (01C000-01FFFFh) not protected from table reads executed in other blocks)

// CONFIG6H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

#include <xc.h>
#include "main.h"
#include "RFM69.h"
#include "common.h"
#include "DS3234.h"
#include "RFM69registers.h"
#include "Adafruit_RA8875.h"
#include "display.h"
#include "memory.h"
#include "therm.h"

void main(void) {
    if (!init()) {
        setDisplayMode(DISP_MODE_HOME);
        setGraphMode(GRAPH_MODE_DAY);
        while (1) {
            run();
        }
    } else {
        PRINT("Initialization error. System halted.");
        while (TRUE) {

        }
    }
}

unsigned char init(void) {
    unsigned char initError = FALSE;

    //Setup basic pin functions
    TRISBbits.TRISB5 = OUTPUT;
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;

    //Setup timing & clocks
    OSCCON1bits.NDIV = 0b0000;
    //Setup ms counter
    //Osc speed = 64 MHz
    //Instruction speed = 16 MHz
    //Instruction cycle = 0.0625 us
    //1:128 prescaler gives 8 us timer tick
    //125 * 8 us = 1 ms
    //Set timer initial value to 255-125=130 to roll over once per ms
    T0CON1bits.T0CS = 0b010; //Increment Timer0 on instruction cycle
    T0CON1bits.T0CKPS = 0b0111; //1:128 prescaler
    TMR0 = 130;
    PIE0bits.TMR0IE = TRUE;
    T0CON0bits.T0EN = TRUE; //Turn on timer

    INTCONbits.PEIE = TRUE; //Enable peripheral interrupts
    INTCONbits.GIE = TRUE; //Enable interrupts

    SPIInit();

    if (displayInit()) {
        initError = TRUE;
    }

    //If we can't even initialize the display, no debug information can be output.
    //So, hang
    if (initError) {
        while (TRUE) {

        }
    }

    //Initialize ADC
    if(thermInit()){
        initError = TRUE;
    }
    
    //Initialize RTC & get current timestamp
    //DEBUG CODE!
    //currentYear = 18;
    //currentMonth = 12;
    //currentDay = 18;
    //currentHour = 18;
    getTime();

    //Verify that current date has been loaded
    if (currentYear == 0xff || currentMonth == 0xff ||
            currentDay == 0xff || currentHour == 0xff) {
        PRINT("Current date not correctly loaded from RTC!\n");
        initError = TRUE;
    }
    else{
        char datetime[2];
        PRINT("Date: 0x");
        itoh8(years_reg.all, datetime);
        RA8875_textWrite(datetime, 2);
        PRINT("-0x");
        itoh8(month_reg.all, datetime);
        RA8875_textWrite(datetime, 2);
        PRINT("-0x");
        itoh8(date_reg.all, datetime);
        RA8875_textWrite(datetime, 2);
        PRINT(" 0x");
        itoh8(hours_reg.all, datetime);
        RA8875_textWrite(datetime, 2);
        PRINT(":0x");
        itoh8(minutes_reg.all, datetime);
        RA8875_textWrite(datetime, 2);
        PRINT(":0x");
        itoh8(seconds_reg.all, datetime);
        RA8875_textWrite(datetime, 2);
        PRINT("\n");
    }

    if(currentYear < 18){
        //Clock was reset
        PRINT("Time not set! Starting up in safe mode...\n");
        safeMode = TRUE;
    }
    else{
        safeMode = FALSE;
    }

    //Bootstrap memory
    if (!initError) {
        if (loadEEPROMPageIndex()) {
            PRINT("Data memory setup failed!\n");
            initError = TRUE;
        }
    }
    //setupEEPROMPage(0, currentYear, currentMonth, currentDay);
    //fillExtMemPage(0);
    //ext_mem_write(0x000005, 0xaa);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
    if(currentEEPROMPage != 0xffff){
    dumpExtMemPage(currentEEPROMPage);
    }
    else{
        PRINT("No valid EEPROM page found!\n");
    }
    
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

    return initError;
}

void run(void) {
    unsigned char oldMinuteOfDay = minuteOfDay;
    unsigned char oldDay = currentDay;
    if (fast_tasks_timer == 0) {
        fast_tasks_timer = FAST_TASKS_RATE;
        getTime();
        drawTime();
        updateButtons();
        handleButtonActions();
        if(safeMode){
            drawSafeMode();
        }
    }
    //drawHomeScreen();
    if(slow_tasks_timer == 0){
        slow_tasks_timer = SLOW_TASKS_RATE;
        getTemperature();
        drawTemp();
        if (currentDisplayMode == DISP_MODE_HOME) {
            switch(currentGraphMode){
                case GRAPH_MODE_DAY:
                    clearPlotXLabels();
                    drawPlotDayXLabels();
                    collectDayData();
                    plotTemp(240);
                    break;
                case GRAPH_MODE_WEEK:
                    clearPlotXLabels();
                    drawPlotWeekXLabels();
                    collectWeekData();
                    plotTemp(336);
                    break;
                case GRAPH_MODE_MONTH:
                    clearPlotXLabels();
                    drawPlotMonthXLabels();
                    collectMonthData();
                    plotTemp(360);
                    break;
                case GRAPH_MODE_YEAR:
                    clearPlotXLabels();
                    drawPlotYearXLabels();
                    collectYearData();
                    plotTemp(365);
                    break;    
                    
            }
    }
    }
    
    fast_tasks_timer--;
    slow_tasks_timer--;
    
    if(oldMinuteOfDay != minuteOfDay){
        if((minuteOfDay % 6 == 0)){
            //Write temperature to memory
            unsigned char sampleIndex = minuteOfDay / 6;
            ext_mem_write((currentEEPROMPage<<8)+2+sampleIndex , currentTemperatureByte);
        }
    }
    if(oldDay != currentDay){
        incrementEEPROMPageIndex();
    }
    
    //pros_seconds.all = 0;
    //pros_minutes.all = 0;
    //pros_hours.all = 0;
    //pros_date.all = 0;
    //pros_month.all = 0;
    //pros_years.all = 0;
    //setTime();
    unsigned char foo = readRTCReg(REG_CONTROL);
    //foo = readRTCReg(REG_STAT);
    //readAll();
    //writeRTCReg(REG_CONTROL, 0b10000100); //Enable write
    //foo = readRTCReg(REG_CONTROL);
    //writeRTCReg(REG_CONTROL, 0b00000100); //Enable oscillator
    //foo = readRTCReg(REG_CONTROL);
    __delay_ms(1);
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
    //If any button is pressed when asleep, wake up
    if (buttonPressed.all != 0 && isSleep) {
        setSleep(FALSE);
        __delay_ms(250);
        return;
    }
    if (buttonPressed.B1) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                setGraphMode(GRAPH_MODE_DAY);
                break;
            case DISP_MODE_UTIL:
                //Switch to home
                setDisplayMode(DISP_MODE_HOME);
                break;
            case DISP_MODE_SETTIME:
                //Switch to home
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B2) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Switch to past-week plot
                setGraphMode(GRAPH_MODE_WEEK);
                break;
            case DISP_MODE_UTIL:
                //Switch to time-set screen
                setDisplayMode(DISP_MODE_SETTIME);
                getTime();
                pros_seconds.all = seconds_reg.all;
                pros_minutes.all = minutes_reg.all;
                pros_hours.all = hours_reg.all;
                pros_date.all = date_reg.all;
                pros_month.all = month_reg.all;
                pros_years.all = years_reg.all;
                activeTimeChar = ACTIVE_TIME_10YR;
                drawProspectiveTime();
                break;
            case DISP_MODE_SETTIME:
                setTime();
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B3) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Switch to past-month plot
                setGraphMode(GRAPH_MODE_MONTH);
                break;
            case DISP_MODE_UTIL:
                //Force start RTC
                RTCOscRestart();
                break;
            case DISP_MODE_SETTIME:
                //Move cursor left
                if (activeTimeChar == 0) {
                    activeTimeChar = ACTIVE_TIME_SECOND;
                } else {
                    activeTimeChar--;
                }
                drawProspectiveTime();
                break;
            case DISP_MODE_VERIFY_RESET:
                //"No" selected
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B4) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Switch to past-year plot
                setGraphMode(GRAPH_MODE_YEAR);
                break;
            case DISP_MODE_UTIL:
                //Reset
                RESET();
                break;
            case DISP_MODE_SETTIME:
                //Move cursor right
                if (activeTimeChar == ACTIVE_TIME_SECOND) {
                    activeTimeChar = 0;
                } else {
                    activeTimeChar++;
                }
                drawProspectiveTime();
                break;
            case DISP_MODE_VERIFY_RESET:
                //"Yes" selected
                resetEEPROMPageIndex();
                setDisplayMode(DISP_MODE_HOME);
                break;
        }
        return;
    }

    if (buttonPressed.B5) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Go to util screen
                setDisplayMode(DISP_MODE_UTIL);
                break;
            case DISP_MODE_UTIL:
                break;
            case DISP_MODE_SETTIME:
                //Increment
                incrementActiveChar();
                drawProspectiveTime();
                break;
        }
        return;
    }

    if (buttonPressed.B6) {
        switch (currentDisplayMode) {
            case DISP_MODE_HOME:
                //Turn off screen
                setSleep(TRUE);
                break;
            case DISP_MODE_UTIL:
                setDisplayMode(DISP_MODE_VERIFY_RESET);
                break;
            case DISP_MODE_SETTIME:
                //Decrement
                decrementActiveChar();
                drawProspectiveTime();
                break;
        }
        return;
    }

    //If a button was pressed, do the action, then delay to ignore bounces
    if (buttonPressed.all != 0) {
        __delay_ms(300);
    }
}

void interrupt ISR(void) {
    if (PIR0bits.TMR0IF) {
        LATBbits.LATB5 = !LATBbits.LATB5;
        PIR0bits.TMR0IF = FALSE;
        ms_count++;
        TMR0 = 130;
    }
}