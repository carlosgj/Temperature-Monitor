#ifndef MAIN_H
#define	MAIN_H

#include <xc.h> 
#include "common.h"
#include "SPI.h"
#include "Adafruit_RA8875.h"

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

void init(void);
void run(void);
void drawHomeScreen(void);
void drawTemplate(void);
void drawUtilScreen(void);
void drawGrid(void);
unsigned int tempToPixel(unsigned char temp);
void plotTemp(unsigned int x, unsigned char temp);
void atoi(unsigned char val, char* dest, unsigned char zeroPad);
#endif	/* XC_HEADER_TEMPLATE_H */

