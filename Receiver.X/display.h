#ifndef DISPLAY_H
#define	DISPLAY_H

#include <xc.h> 

#define WIDTH 800
#define HEIGHT 480
#define HEADER_LEVEL 70
#define FOOTER_LEVEL 445
#define MIN_GRAPH_TEMP 30
#define MAX_GRAPH_TEMP 110
#define GRAPH_H_MARGIN 30
#define GRAPH_BOTTOM_MARGIN 40
#define GRAPH_V_PIXELS ((FOOTER_LEVEL-GRAPH_BOTTOM_MARGIN)-(HEADER_LEVEL+10))
#define GRAPH_RIGHT (WIDTH - GRAPH_H_MARGIN)
#define MENU_TEXT_Y (HEIGHT-34)

//10-degree interval on graph
#define GRAPH_TICK_INTERVAL (GRAPH_V_PIXELS / ((MAX_GRAPH_TEMP - MIN_GRAPH_TEMP) / 10))

//Number of pixels off the bottom of the graph of the max temperature
#define GRAPH_MAX_PIXEL (GRAPH_TICK_INTERVAL*((MAX_GRAPH_TEMP - MIN_GRAPH_TEMP) / 10))

#define PRINT(X) RA8875_textWrite(X, sizeof(X)-1)

#define DISP_MODE_HOME      0
#define DISP_MODE_UTIL      1
#define DISP_MODE_SETTIME   2

#define ACTIVE_TIME_10YR    0
#define ACTIVE_TIME_YEAR    1
#define ACTIVE_TIME_10MNTH  2
#define ACTIVE_TIME_MONTH   3
#define ACTIVE_TIME_10DAY   4
#define ACTIVE_TIME_DAY     5
#define ACTIVE_TIME_10HR    6
#define ACTIVE_TIME_HOUR    7
#define ACTIVE_TIME_10MIN   8
#define ACTIVE_TIME_MINUTE  9
#define ACTIVE_TIME_10SEC   10
#define ACTIVE_TIME_SECOND  11

unsigned char activeTimeChar = ACTIVE_TIME_10YR; 

unsigned char displayInit(void);
void drawHomeScreen(void);
void drawTemplate(void);
void drawUtilScreen(void);
void drawGrid(void);
unsigned int tempToPixel(unsigned char temp);
void plotTemp(unsigned int x, unsigned char temp);
void itoa(unsigned char val, char* dest, unsigned char zeroPad);
void setDisplayMode(unsigned char newMode);
void setSleep(unsigned char sleep);
void drawProspectiveTime(void);
void drawTime(void);
void incrementActiveChar(void);
void decrementActiveChar(void);

unsigned char currentDisplayMode = DISP_MODE_HOME;
unsigned char isSleep = FALSE;

#endif	/* XC_HEADER_TEMPLATE_H */

