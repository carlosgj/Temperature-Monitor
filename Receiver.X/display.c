#include <xc.h>
#include "Adafruit_RA8875.h"
#include "common.h"
#include "display.h"
#include "pindef.h"
#include "DS3234.h"
#include "memory.h"

unsigned char displayInit(void) {
    DISP_RST_TRIS = OUTPUT;
    DISP_RST_LAT = TRUE;
    RA8875_reset();
    __delay_ms(200);
    if (!RA8875_begin(RA8875_800x480)) {
        return TRUE;
    }
    RA8875_displayOn(TRUE);
    RA8875_SetGPIOX(TRUE); // Enable TFT - display enable tied to GPIOX
    RA8875_PWM1config(TRUE, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    RA8875_PWM1out(255);
    RA8875_fillScreen(RA8875_BLACK);
    RA8875_textMode();
    textLine = 1;
    RA8875_textEnlarge(0);
    RA8875_textSetCursor(10, 10);
    RA8875_textTransparent(RA8875_WHITE);
    PRINT("Display initialized...\n");
    __delay_ms(500);
    return 0;
}

void drawTemplate(void) {
    RA8875_graphicsMode();
    RA8875_fillScreen(RA8875_BLACK);
    RA8875_drawFastHLine(0, HEADER_LEVEL, WIDTH, RA8875_WHITE);
    RA8875_drawFastHLine(0, FOOTER_LEVEL, WIDTH, RA8875_WHITE);
    RA8875_drawLine(133, HEIGHT, 133, FOOTER_LEVEL, RA8875_WHITE);
    RA8875_drawLine(267, HEIGHT, 267, FOOTER_LEVEL, RA8875_WHITE);
    RA8875_drawLine(400, HEIGHT, 400, FOOTER_LEVEL, RA8875_WHITE);
    RA8875_drawLine(533, HEIGHT, 533, FOOTER_LEVEL, RA8875_WHITE);
    RA8875_drawLine(667, HEIGHT, 667, FOOTER_LEVEL, RA8875_WHITE);
}

void drawHomeScreen(void) {
    drawTemplate();
    RA8875_drawLine(GRAPH_H_MARGIN, FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN, GRAPH_RIGHT, FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN, RA8875_WHITE);
    RA8875_drawLine(GRAPH_H_MARGIN, FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN, GRAPH_H_MARGIN, HEADER_LEVEL + 10, RA8875_WHITE);
    RA8875_drawLine(WIDTH - GRAPH_H_MARGIN, FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN, WIDTH - GRAPH_H_MARGIN, HEADER_LEVEL + 10, RA8875_WHITE);

    //Draw ticks
    unsigned char i;
    unsigned int vpos = FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    for (i = MIN_GRAPH_TEMP; i <= MAX_GRAPH_TEMP; i += 10) {
        RA8875_drawFastHLine(GRAPH_H_MARGIN - 3, vpos, 3, RA8875_WHITE);
        vpos -= GRAPH_TICK_INTERVAL;
    }
    vpos = FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    for (i = MIN_GRAPH_TEMP; i <= MAX_GRAPH_TEMP; i += 10) {
        RA8875_drawFastHLine(GRAPH_RIGHT, vpos, 3, RA8875_WHITE);
        vpos -= GRAPH_TICK_INTERVAL;
    }

    RA8875_textMode();
    RA8875_textTransparent(RA8875_WHITE);
    RA8875_textEnlarge(0);
    char number[3];
    vpos = FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    for (i = MIN_GRAPH_TEMP; i <= MAX_GRAPH_TEMP; i += 10) {
        RA8875_textSetCursor(0, vpos - 10);
        itoa(i, number, FALSE);
        RA8875_textWrite(number, 3);
        vpos -= GRAPH_TICK_INTERVAL;
    }
    vpos = FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    for (i = MIN_GRAPH_TEMP; i <= MAX_GRAPH_TEMP; i += 10) {
        RA8875_textSetCursor(GRAPH_RIGHT + 4, vpos - 10);
        itoa(i, number, FALSE);
        RA8875_textWrite(number, 3);
        vpos -= GRAPH_TICK_INTERVAL;
    }

    RA8875_textMode();
    RA8875_textSetCursor(170, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    PRINT("Temperature Monitor");

    RA8875_textEnlarge(1);
    RA8875_textSetCursor(45, MENU_TEXT_Y);
    RA8875_textWrite("Day", sizeof ("Day"));
    RA8875_textSetCursor(175, MENU_TEXT_Y);
    RA8875_textWrite("Week", sizeof ("Week"));
    RA8875_textSetCursor(300, MENU_TEXT_Y);
    RA8875_textWrite("Month", sizeof ("Month"));
    RA8875_textSetCursor(435, MENU_TEXT_Y);
    RA8875_textWrite("Year", sizeof ("Year"));
    RA8875_textSetCursor(570, MENU_TEXT_Y);
    RA8875_textWrite("Util", sizeof ("Util"));
    RA8875_textSetCursor(695, MENU_TEXT_Y);
    RA8875_textWrite("Sleep", sizeof ("Sleep"));
}

void drawUtilScreen(void) {
    char uintStr[4];
    drawTemplate();
    RA8875_textMode();
    RA8875_textSetCursor(250, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    RA8875_textWrite("Utils & Stats", sizeof ("Utils & Stats"));

    RA8875_textEnlarge(1);
    RA8875_textSetCursor(45, MENU_TEXT_Y);
    PRINT("Home");
    RA8875_textSetCursor(175, MENU_TEXT_Y);
    PRINT("Time");
    RA8875_textSetCursor(280, MENU_TEXT_Y);
    PRINT("RTC FS");
    RA8875_textSetCursor(435, MENU_TEXT_Y);
    PRINT("Null");
    RA8875_textSetCursor(570, MENU_TEXT_Y);
    PRINT("Null");
    RA8875_textSetCursor(695, MENU_TEXT_Y);
    PRINT("RESET");

    RA8875_textEnlarge(0);
    RA8875_textSetCursor(0, HEADER_LEVEL + 10);
    PRINT("Current EEPROM page: 0x");
    itoh16(currentEEPROMPage, uintStr);
    RA8875_textWrite(uintStr, 4);
    PRINT("\n");
    PRINT("RTC control register: 0x");
    itoh8(readRTCReg(REG_CONTROL), uintStr);
    RA8875_textWrite(uintStr, 2);
    PRINT("\n");
    
}

void drawVerifyResetScreen(){
    drawTemplate();
    RA8875_textMode();
    RA8875_textSetCursor(250, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    PRINT("Verify Reset");

    RA8875_textEnlarge(1);
    RA8875_textSetCursor(300, MENU_TEXT_Y);
    PRINT("No");
    RA8875_textSetCursor(435, MENU_TEXT_Y);
    PRINT("Yes");

    RA8875_textSetCursor(1, HEADER_LEVEL + 10);
    PRINT("WARNING! This will reset EEPROM page counter.");
    PRINT("Stored data will be lost.");
    PRINT("Continue?");
}

void drawSetTimeScreen() {
    drawTemplate();
    RA8875_textMode();
    RA8875_textSetCursor(250, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    PRINT("Set Time");

    RA8875_textEnlarge(1);
    RA8875_textSetCursor(45, MENU_TEXT_Y);
    PRINT("Home");
    RA8875_textSetCursor(175, MENU_TEXT_Y);
    PRINT("Set");
    RA8875_textSetCursor(300, MENU_TEXT_Y);
    PRINT("\x1b");
    RA8875_textSetCursor(435, MENU_TEXT_Y);
    PRINT("\x1a");
    RA8875_textSetCursor(570, MENU_TEXT_Y);
    PRINT("\x18");
    RA8875_textSetCursor(695, MENU_TEXT_Y);
    PRINT("\x19");
}


void itoh8(unsigned char val, char* dest) {
    unsigned char thisDigit;
    thisDigit = val / 0x10;
    if(thisDigit < 10){
        dest[0] = (char) (thisDigit + 48);
    }
    else{
        dest[0] = (char) (thisDigit + 55);
    }
    
    thisDigit = val % 0x10;
    if(thisDigit < 10){
        dest[1] = (char) (thisDigit + 48);
    }
    else{
        dest[1] = (char) (thisDigit + 55);
    }
}

void itoh16(unsigned int val, char* dest) {
    unsigned int thisDigit;
    thisDigit = val / 0x1000;
    if(thisDigit < 10){
        dest[0] = (char) (thisDigit + 48);
    }
    else{
        dest[0] = (char) (thisDigit + 55);
    }
    
    thisDigit = (val / 0x100) % 0x10;
    if(thisDigit < 10){
        dest[1] = (char) (thisDigit + 48);
    }
    else{
        dest[1] = (char) (thisDigit + 55);
    }
    
    thisDigit = (val / 0x10) % 0x10;
    if(thisDigit < 10){
        dest[2] = (char) (thisDigit + 48);
    }
    else{
        dest[2] = (char) (thisDigit + 55);
    }
    
    thisDigit = val % 0x10;
    if(thisDigit < 10){
        dest[3] = (char) (thisDigit + 48);
    }
    else{
        dest[3] = (char) (thisDigit + 55);
    }
}

void itoa(unsigned char val, char* dest, unsigned char zeroPad) {
    unsigned char thisDigit;
    thisDigit = val / 100;
    if (thisDigit == 0 && !zeroPad) {
        dest[0] = ' ';
    } else {
        dest[0] = (char) (thisDigit + 48);
    }

    thisDigit = (val % 100) / 10;
    if (thisDigit == 0 && !zeroPad && (val / 100) == 0) {
        dest[1] = ' ';
    } else {
        dest[1] = (char) (thisDigit + 48);
    }

    thisDigit = val % 10;
    if (thisDigit == 0 && !zeroPad && (val / 100) == 0 && ((val % 100) / 10) == 0) {
        dest[2] = ' ';
    } else {
        dest[2] = (char) (thisDigit + 48);
    }
}

void drawGrid(void) {
    RA8875_graphicsMode();
    unsigned int i;
    for (i = 0; i <= WIDTH; i += 20) {
        RA8875_drawFastVLine(i, 0, HEIGHT, RA8875_BLUE);
    }
    for (i = 0; i <= HEIGHT; i += 20) {
        RA8875_drawFastHLine(0, i, WIDTH, RA8875_BLUE);
    }
}

unsigned int tempToPixel(unsigned char temp) {
    unsigned char isHalf = temp & 1;
    temp >>= 1;
    temp &= 0b01111111;
    if (temp < MIN_GRAPH_TEMP) {
        return FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    }
    if (temp > MAX_GRAPH_TEMP) {
        return (FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN) -GRAPH_MAX_PIXEL;
    }
    temp -= MIN_GRAPH_TEMP;
    unsigned char scaling = (GRAPH_MAX_PIXEL / (MAX_GRAPH_TEMP - MIN_GRAPH_TEMP));
    unsigned char halfScaling = scaling / 2;
    return (FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN - 2)-(temp * scaling)-(isHalf * halfScaling);
}

void plotTempPoint(unsigned int x, unsigned char temp) {
    if(temp != 0xff){
    RA8875_fillCircle(x, tempToPixel(temp), 1, RA8875_RED);
    }
}

void clearPlot(void){
    RA8875_HWfillRect(GRAPH_H_MARGIN+1, HEADER_LEVEL+10, GRAPH_WIDTH-2, GRAPH_V_PIXELS-1, RA8875_BLACK);
}

void clearPlotXLabels(void){
    RA8875_HWfillRect(GRAPH_H_MARGIN-10, (FOOTER_LEVEL-GRAPH_BOTTOM_MARGIN)+1, GRAPH_WIDTH+20, 30, RA8875_BLACK);
}

void drawPlotDayXLabels(void){
    unsigned int X = GRAPH_RIGHT-((currentHour % 6)*20);
    unsigned char hour = (currentHour / 6)*6;
    unsigned char hourCount = 0;
    //2px per sample, 10 samples per hour
    for(hourCount = 0; hourCount < 24; hourCount += 6){
        RA8875_drawFastVLine(X, FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN, 3, RA8875_WHITE); //Tick
        
        RA8875_textMode();
        RA8875_textTransparent(RA8875_WHITE);
        RA8875_textEnlarge(0);
        char number[3];
        RA8875_textSetCursor(X-10, (FOOTER_LEVEL-GRAPH_BOTTOM_MARGIN)+14);
        itoa(hour, number, TRUE);
        RA8875_textWrite(number+1, 2);
        
        X -= 120;
        if(hour >=6){
            hour -= 6;
        }
        else{
            hour = 18;
        }
    }
}

void drawPlotWeekXLabels(void){
    
}

void drawPlotMonthXLabels(void){
    
}

void drawPlotYearXLabels(void){
    unsigned int X = GRAPH_RIGHT-(currentDay*2);
    unsigned char month = currentMonth;
    unsigned char monthCount = 0;
    //2px per sample, 10 samples per hour
    for(monthCount = 0; monthCount < 12; monthCount ++){
        RA8875_drawFastVLine(X, FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN, 3, RA8875_WHITE); //Tick
        
        RA8875_textMode();
        RA8875_textTransparent(RA8875_WHITE);
        RA8875_textEnlarge(0);
        char number[3];
        RA8875_textSetCursor(X-10, (FOOTER_LEVEL-GRAPH_BOTTOM_MARGIN)+14);
        itoa(month, number, TRUE);
        RA8875_textWrite(number+1, 2);
        
        X -= 60;
        if(month >1){
            month--;
        }
        else{
            month=12;
        }
    }
}

void setDisplayMode(unsigned char newMode) {
    switch (newMode) {
        case DISP_MODE_HOME:
            drawHomeScreen();
            currentDisplayMode = newMode;
            break;
        case DISP_MODE_UTIL:
            drawUtilScreen();
            currentDisplayMode = newMode;
            break;
        case DISP_MODE_SETTIME:
            drawSetTimeScreen();
            currentDisplayMode = newMode;
            break;
        case DISP_MODE_VERIFY_RESET:
            drawVerifyResetScreen();
            currentDisplayMode = newMode;
            break;
        default:
            break;
    }

}

void setGraphMode(unsigned char newMode) {
    switch (newMode) {
        case GRAPH_MODE_DAY:
            clearPlotXLabels();
            drawPlotDayXLabels();
            collectDayData();
            plotTemp(240);
            currentGraphMode = newMode;
            break;
        case GRAPH_MODE_WEEK:
            clearPlotXLabels();
            drawPlotWeekXLabels();
            collectWeekData();
            plotTemp(240);
            currentGraphMode = newMode;
            break;
        case GRAPH_MODE_MONTH:
            clearPlotXLabels();
            drawPlotMonthXLabels();
            collectMonthData();
            plotTemp(240);
            currentGraphMode = newMode;
            break;
        case GRAPH_MODE_YEAR:
            clearPlotXLabels();
            drawPlotYearXLabels();
            collectYearData();
            plotTemp(240);
            currentGraphMode = newMode;
            break;
        default:
            break;
    }

}

void setSleep(unsigned char sleep) {
    if (sleep) {
        RA8875_PWM1out(0);
        isSleep = TRUE;
    } else {
        RA8875_PWM1out(255);
        isSleep = FALSE;
    }
}

void drawTime(void) {
    char temp[3];
    RA8875_textMode();
    RA8875_textSetCursor(620, 10);
    RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    RA8875_textEnlarge(0);
    PRINT("20");
    itoa(years_reg.tens, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    itoa(years_reg.ones, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    PRINT("/");
    itoa(month_reg.tens, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    itoa(month_reg.ones, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    PRINT("/");
    itoa(date_reg.tens, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    itoa(date_reg.ones, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    PRINT(" ");
    itoa(hours_reg.tens, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    itoa(hours_reg.ones, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    PRINT(":");
    itoa(minutes_reg.tens, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    itoa(minutes_reg.ones, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    PRINT(":");
    itoa(seconds_reg.tens, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);
    itoa(seconds_reg.ones, temp, TRUE);
    RA8875_textWrite(temp + 2, 1);

}

void drawProspectiveTime(void) {
    char temp[3];
    RA8875_textMode();
    RA8875_textSetCursor(100, 200);
    RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    RA8875_textEnlarge(2);
    PRINT("20");
    itoa(pros_years.tens, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_10YR) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    itoa(pros_years.ones, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_YEAR) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    PRINT("/");
    itoa(pros_month.tens, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_10MNTH) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    itoa(pros_month.ones, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_MONTH) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    PRINT("/");
    itoa(pros_date.tens, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_10DAY) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    itoa(pros_date.ones, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_DAY) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    PRINT(" ");
    itoa(pros_hours.tens, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_10HR) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    itoa(pros_hours.ones, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_HOUR) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    PRINT(":");
    itoa(pros_minutes.tens, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_10MIN) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    itoa(pros_minutes.ones, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_MINUTE) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    PRINT(":");
    itoa(pros_seconds.tens, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_10SEC) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
    itoa(pros_seconds.ones, temp, TRUE);
    if (activeTimeChar == ACTIVE_TIME_SECOND) {
        RA8875_textColor(RA8875_BLACK, RA8875_WHITE);
        RA8875_textWrite(temp + 2, 1);
        RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    } else {
        RA8875_textWrite(temp + 2, 1);
    }
}

void incrementActiveChar(void) {
    switch (activeTimeChar) {
        case ACTIVE_TIME_10YR:
            if (pros_years.tens == 9) {
                pros_years.tens = 0;
            } else {
                pros_years.tens++;
            }
            break;
        case ACTIVE_TIME_YEAR:
            if (pros_years.ones == 9) {
                pros_years.ones = 0;
            } else {
                pros_years.ones++;
            }
            break;
        case ACTIVE_TIME_10MNTH:
            if (pros_month.ones < 3) {
                if (pros_month.tens == 1) {
                    pros_month.tens = 0;
                } else {
                    pros_month.tens++;
                }
            }
            break;
        case ACTIVE_TIME_MONTH:
            if (pros_month.tens == 0) {
                if (pros_month.ones == 9) {
                    pros_month.ones = 0;
                } else {
                    pros_month.ones++;
                }
            } else {
                if (pros_month.ones == 2) {
                    pros_month.ones = 0;
                } else {
                    pros_month.ones++;
                }
            }
            break;
        case ACTIVE_TIME_10DAY:
            if (pros_date.ones < 2) {
                if (pros_date.tens == 3) {
                    pros_date.tens = 0;
                } else {
                    pros_date.tens++;
                }
            } else {
                if (pros_date.tens == 2) {
                    pros_date.tens = 0;
                } else {
                    pros_date.tens++;
                }
            }
            break;
        case ACTIVE_TIME_DAY:
            if (pros_date.tens < 3) {
                if (pros_date.ones == 9) {
                    pros_date.ones = 0;
                } else {
                    pros_date.ones++;
                }
            } else {
                if (pros_date.ones == 1) {
                    pros_date.ones = 0;
                } else {
                    pros_date.ones++;
                }
            }
            break;
        case ACTIVE_TIME_10HR:
            if (pros_hours.ones < 5) {
                if (pros_hours.tens == 2) {
                    pros_hours.tens = 0;
                } else {
                    pros_hours.tens++;
                }
            } else {
                if (pros_hours.tens == 1) {
                    pros_hours.tens = 0;
                } else {
                    pros_hours.tens++;
                }
            }
            break;
        case ACTIVE_TIME_HOUR:
            if (pros_hours.tens < 2) {
                if (pros_hours.ones == 9) {
                    pros_hours.ones = 0;
                } else {
                    pros_hours.ones++;
                }
            } else {
                if (pros_hours.ones == 4) {
                    pros_hours.ones = 0;
                } else {
                    pros_hours.ones++;
                }
            }
            break;
        case ACTIVE_TIME_10MIN:
            if (pros_minutes.tens == 5) {
                pros_minutes.tens = 0;
            } else {
                pros_minutes.tens++;
            }
            break;
        case ACTIVE_TIME_MINUTE:
            if (pros_minutes.ones == 9) {
                pros_minutes.ones = 0;
            } else {
                pros_minutes.ones++;
            }
            break;
        case ACTIVE_TIME_10SEC:
            if (pros_seconds.tens == 5) {
                pros_seconds.tens = 0;
            } else {
                pros_seconds.tens++;
            }
            break;
        case ACTIVE_TIME_SECOND:
            if (pros_seconds.ones == 9) {
                pros_seconds.ones = 0;
            } else {
                pros_seconds.ones++;
            }
            break;
    }
}

void decrementActiveChar(void) {
    switch (activeTimeChar) {
        case ACTIVE_TIME_10YR:
            if (pros_years.tens == 0) {
                pros_years.tens = 9;
            } else {
                pros_years.tens--;
            }
            break;
        case ACTIVE_TIME_YEAR:
            if (pros_years.ones == 0) {
                pros_years.ones = 9;
            } else {
                pros_years.ones--;
            }
            break;
        case ACTIVE_TIME_10MNTH:
            if (pros_month.ones < 3) {
                if (pros_month.tens == 0) {
                    pros_month.tens = 1;
                } else {
                    pros_month.tens--;
                }
            }
            break;
        case ACTIVE_TIME_MONTH:
            if (pros_month.tens == 0) {
                if (pros_month.ones == 0) {
                    pros_month.ones = 9;
                } else {
                    pros_month.ones--;
                }
            } else {
                if (pros_month.ones == 0) {
                    pros_month.ones = 2;
                } else {
                    pros_month.ones--;
                }
            }
            break;
        case ACTIVE_TIME_10DAY:
            if (pros_date.ones < 2) {
                if (pros_date.tens == 0) {
                    pros_date.tens = 3;
                } else {
                    pros_date.tens--;
                }
            } else {
                if (pros_date.tens == 0) {
                    pros_date.tens = 2;
                } else {
                    pros_date.tens--;
                }
            }
            break;
        case ACTIVE_TIME_DAY:
            if (pros_date.tens < 3) {
                if (pros_date.ones == 0) {
                    pros_date.ones = 9;
                } else {
                    pros_date.ones--;
                }
            } else {
                if (pros_date.ones == 0) {
                    pros_date.ones = 1;
                } else {
                    pros_date.ones--;
                }
            }
            break;
        case ACTIVE_TIME_10HR:
            if (pros_hours.ones < 5) {
                if (pros_hours.tens == 0) {
                    pros_hours.tens = 2;
                } else {
                    pros_hours.tens--;
                }
            } else {
                if (pros_hours.tens == 0) {
                    pros_hours.tens = 1;
                } else {
                    pros_hours.tens--;
                }
            }
            break;
        case ACTIVE_TIME_HOUR:
            if (pros_hours.tens < 2) {
                if (pros_hours.ones == 0) {
                    pros_hours.ones = 9;
                } else {
                    pros_hours.ones--;
                }
            } else {
                if (pros_hours.ones == 0) {
                    pros_hours.ones = 4;
                } else {
                    pros_hours.ones--;
                }
            }
            break;
        case ACTIVE_TIME_10MIN:
            if (pros_minutes.tens == 0) {
                pros_minutes.tens = 5;
            } else {
                pros_minutes.tens--;
            }
            break;
        case ACTIVE_TIME_MINUTE:
            if (pros_minutes.ones == 0) {
                pros_minutes.ones = 9;
            } else {
                pros_minutes.ones--;
            }
            break;
        case ACTIVE_TIME_10SEC:
            if (pros_seconds.tens == 0) {
                pros_seconds.tens = 5;
            } else {
                pros_seconds.tens--;
            }
            break;
        case ACTIVE_TIME_SECOND:
            if (pros_seconds.ones == 0) {
                pros_seconds.ones = 9;
            } else {
                pros_seconds.ones--;
            }
            break;
    }
}

void drawTemp(void){
    char temp[3];
    RA8875_textMode();
    RA8875_textSetCursor(10, 10);
    RA8875_textColor(RA8875_WHITE, RA8875_BLACK);
    RA8875_textEnlarge(0);
    itoa(currentTemperatureByte>>1, temp, FALSE);
    RA8875_textWrite(temp, 3);
    RA8875_textWrite(".", 1);
    if(currentTemperatureByte & 1){
        RA8875_textWrite("5", 1);
    }
    else{
        RA8875_textWrite("0", 1);
    }
}

void plotTemp(unsigned int count) {
    clearPlot();
    unsigned int dataIndex = count-1;
    unsigned int pixelIndex = GRAPH_RIGHT-2;
    do{
        if(pixelIndex <= (GRAPH_H_MARGIN+2)){
            break;
        }
        plotTempPoint(pixelIndex, tempData[dataIndex]);
        pixelIndex -= 2;
        dataIndex--;
    }while(dataIndex >0);
}