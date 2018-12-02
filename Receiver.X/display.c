#include <xc.h>
#include "Adafruit_RA8875.h"
#include "common.h"
#include "display.h"
#include "pindef.h"

void displayInit(void) {
    DISP_RST_TRIS = OUTPUT;
    DISP_RST_LAT = TRUE;
    RA8875_reset();
    __delay_ms(200);
    if (!RA8875_begin(RA8875_800x480)) {
        while (TRUE) {

        }
    }
    RA8875_displayOn(TRUE);
    RA8875_SetGPIOX(TRUE); // Enable TFT - display enable tied to GPIOX
    RA8875_PWM1config(TRUE, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    RA8875_PWM1out(255);
    RA8875_fillScreen(RA8875_BLACK);
    RA8875_textMode();
    RA8875_textSetCursor(10, 10);
    RA8875_textTransparent(RA8875_WHITE);
    RA8875_textWrite("Display initialized...", sizeof ("Display initialized..."));
    __delay_ms(1000);
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
        atoi(i, number, FALSE);
        RA8875_textWrite(number, 3);
        vpos -= GRAPH_TICK_INTERVAL;
    }
    vpos = FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    for (i = MIN_GRAPH_TEMP; i <= MAX_GRAPH_TEMP; i += 10) {
        RA8875_textSetCursor(GRAPH_RIGHT + 4, vpos - 10);
        atoi(i, number, FALSE);
        RA8875_textWrite(number, 3);
        vpos -= GRAPH_TICK_INTERVAL;
    }

    RA8875_textMode();
    RA8875_textSetCursor(150, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    RA8875_textWrite("Temperature Monitor", sizeof ("Temperature Monitor"));

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
    drawTemplate();
    RA8875_textMode();
    RA8875_textSetCursor(250, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    RA8875_textWrite("Utils & Stats", sizeof ("Utils & Stats"));

    RA8875_textEnlarge(0);
    RA8875_textSetCursor(0, HEADER_LEVEL + 10);
    RA8875_textWrite("Current EEPROM page: ", sizeof ("Current EEPROM page: "));
}

void atoi(unsigned char val, char* dest, unsigned char zeroPad) {
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
    return (FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN)-(temp * scaling)-(isHalf * halfScaling);
}

void plotTemp(unsigned int x, unsigned char temp) {
    RA8875_fillCircle(x, tempToPixel(temp), 1, RA8875_RED);
}