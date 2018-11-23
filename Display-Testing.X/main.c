// PIC18F4550 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSC_EC // Oscillator Selection bits (Internal oscillator, CLKO function on RA6, EC used by USB (INTCKO))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#include <xc.h>
#include "main.h"

void main(void) {
    init();
    while (TRUE) {
        run();
    }
}

void init(void) {
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 0b10;

    TRISCbits.TRISC0 = OUTPUT;

    //Start up display
    SPIInit();
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

    
    drawHomeScreen();
    //drawGrid();
    RA8875_graphicsMode();
    unsigned char i;
    unsigned int hpos = GRAPH_H_MARGIN;
    for(i=0; i<=0xf0; i++){
        plotTemp(hpos, i);
        hpos++;
    }
}

void run(void) {
    LATCbits.LATC0 = !LATCbits.LATC0;
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
        RA8875_textSetCursor(GRAPH_RIGHT+4, vpos - 10);
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

void drawUtilScreen(void){
    drawTemplate();
    RA8875_textMode();
    RA8875_textSetCursor(250, 10);
    RA8875_textEnlarge(2);
    RA8875_textTransparent(RA8875_WHITE);
    RA8875_textWrite("Utils & Stats", sizeof ("Utils & Stats"));
    
    RA8875_textEnlarge(0);
    RA8875_textSetCursor(0, HEADER_LEVEL+10);
    RA8875_textWrite("Current EEPROM page: ", sizeof("Current EEPROM page: "));
}

void atoi(unsigned char val, char* dest, unsigned char zeroPad) {
    unsigned char thisDigit;
    thisDigit = val / 100;
    if (thisDigit == 0 && !zeroPad) {
        dest[0] = ' ';
    } else {
        dest[0] = (char) (thisDigit + 48);
    }
    
    thisDigit = (val%100)/10;
    if (thisDigit == 0 && !zeroPad && (val / 100)==0) {
        dest[1] = ' ';
    } else {
        dest[1] = (char) (thisDigit + 48);
    }
    
    thisDigit = val % 10;
    if (thisDigit == 0 && !zeroPad && (val / 100)==0 && ((val%100)/10)==0 ) {
        dest[2] = ' ';
    } else {
        dest[2] = (char) (thisDigit + 48);
    }
}

void drawGrid(void){
    RA8875_graphicsMode();
    unsigned int i;
    for(i=0; i<=WIDTH; i+=20){
        RA8875_drawFastVLine(i, 0, HEIGHT, RA8875_BLUE);
    }
    for(i=0; i<=HEIGHT; i+=20){
        RA8875_drawFastHLine(0, i, WIDTH, RA8875_BLUE);
    }
}

unsigned int tempToPixel(unsigned char temp){
    unsigned char isHalf = temp & 1;
    temp >>= 1;
    temp &= 0b01111111;
    if(temp < MIN_GRAPH_TEMP){
        return FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN;
    }
    if(temp > MAX_GRAPH_TEMP){
        return (FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN) - GRAPH_MAX_PIXEL;
    }
    temp -= MIN_GRAPH_TEMP;
    unsigned char scaling = (GRAPH_MAX_PIXEL / (MAX_GRAPH_TEMP - MIN_GRAPH_TEMP));
    unsigned char halfScaling = scaling / 2;
    return (FOOTER_LEVEL - GRAPH_BOTTOM_MARGIN)-(temp * scaling)-(isHalf*halfScaling);
}

void plotTemp(unsigned int x, unsigned char temp){
    RA8875_fillCircle(x, tempToPixel(temp), 1, RA8875_RED);
}