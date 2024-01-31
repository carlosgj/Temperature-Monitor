/**************************************************************************/
/*!
    @file     Adafruit_RA8875.cpp
    @author   Limor Friend/Ladyada, K.Townsend/KTOWN for Adafruit Industries
    @license  BSD license, all text above and below must be included in
              any redistribution

 This is the library for the Adafruit RA8875 Driver board for TFT displays
 ---------------> http://www.adafruit.com/products/1590
 The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
 It is tested to work with displays in the Adafruit shop. Other displays
 may need timing adjustments and are not guanteed to work.
 
 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!
 
 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

    @section  HISTORY
    
    v1.0 - First release
 */
/**************************************************************************/
#include "SPI.h"
#include "Adafruit_RA8875.h"
#include "common.h"
#include "pindef.h"
/**************************************************************************/
/*!
      Initialises the LCD driver and any HW required by the display
      
      @args s[in] The display size, which can be either:
                  'RA8875_480x272' (4.3" displays) r
                  'RA8875_800x480' (5" and 7" displays)
 */

/**************************************************************************/
unsigned char RA8875_begin(enum RA8875sizes s) {
    _size = s;

    if (_size == RA8875_480x272) {
        _width = 480;
        _height = 272;
    }
    else if (_size == RA8875_800x480) {
        _width = 800;
        _height = 480;
    } else {
        return FALSE;
    }

    DISP_RST_LAT = FALSE;
    __delay_ms(100);
    DISP_RST_LAT = TRUE;
    __delay_ms(100);


    unsigned char x = RA8875_readReg(0);
    if (x != 0x75) {
        return FALSE;
    }

    RA8875_initialize();


    return TRUE;
}


void RA8875_reset(void){
    DISP_RST_LAT = FALSE;
    __delay_ms(200);
    DISP_RST_LAT = TRUE;
}

/**************************************************************************/
/*!
      Performs a SW-based reset of the RA8875
 */

/**************************************************************************/
void RA8875_softReset(void) {
    RA8875_writeCommand(RA8875_PWRR);
    RA8875_writeData(RA8875_PWRR_SOFTRESET);
    RA8875_writeData(RA8875_PWRR_NORMAL);
    __delay_ms(1);
}

/**************************************************************************/
/*!
      Initialise the PLL
 */

/**************************************************************************/
void RA8875_PLLinit(void) {
    if (_size == RA8875_480x272) {
        RA8875_writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
        __delay_ms(1);
        RA8875_writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
        __delay_ms(1);
    } else /* (_size == RA8875_800x480) */ {
        RA8875_writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
        __delay_ms(1);
        RA8875_writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
        __delay_ms(1);
    }
}

/**************************************************************************/
/*!
      Initialises the driver IC (clock setup, etc.)
 */

/**************************************************************************/
void RA8875_initialize(void) {
    RA8875_PLLinit();
    RA8875_writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

    /* Timing values */
    unsigned char pixclk;
    unsigned char hsync_start;
    unsigned char hsync_pw;
    unsigned char hsync_finetune;
    unsigned char hsync_nondisp;
    unsigned char vsync_pw;
    uint16_t vsync_nondisp;
    uint16_t vsync_start;

    /* Set the correct values for the display being used */
    if (_size == RA8875_480x272) {
        pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
        hsync_nondisp = 10;
        hsync_start = 8;
        hsync_pw = 48;
        hsync_finetune = 0;
        vsync_nondisp = 3;
        vsync_start = 8;
        vsync_pw = 10;
    }
    else // (_size == RA8875_800x480)
    {
        pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
        hsync_nondisp = 26;
        hsync_start = 32;
        hsync_pw = 96;
        hsync_finetune = 0;
        vsync_nondisp = 32;
        vsync_start = 23;
        vsync_pw = 2;
    }

    RA8875_writeReg(RA8875_PCSR, pixclk);
    __delay_ms(1);

    /* Horizontal settings registers */
    RA8875_writeReg(RA8875_HDWR, (uint8_t)(_width / 8) - 1); // H width: (HDWR + 1) * 8 = 480
    RA8875_writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
    RA8875_writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) / 8); // H non-display: HNDR * 8 + HNDFTR + 2 = 10
    RA8875_writeReg(RA8875_HSTR, hsync_start / 8 - 1); // Hsync start: (HSTR + 1)*8 
    RA8875_writeReg(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw / 8 - 1)); // HSync pulse width = (HPWR+1) * 8

    /* Vertical settings registers */
    RA8875_writeReg(RA8875_VDHR0, (uint16_t) (_height - 1) & 0xFF);
    RA8875_writeReg(RA8875_VDHR1, (uint16_t) (_height - 1) >> 8);
    RA8875_writeReg(RA8875_VNDR0, (uint8_t)(vsync_nondisp - 1)); // V non-display period = VNDR + 1
    RA8875_writeReg(RA8875_VNDR1, (uint8_t)(vsync_nondisp >> 8));
    RA8875_writeReg(RA8875_VSTR0, (uint8_t)(vsync_start - 1)); // Vsync start position = VSTR + 1
    RA8875_writeReg(RA8875_VSTR1, (uint8_t)(vsync_start >> 8));
    RA8875_writeReg(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1); // Vsync pulse width = VPWR + 1

    /* Set active window X */
    RA8875_writeReg(RA8875_HSAW0, 0); // horizontal start point
    RA8875_writeReg(RA8875_HSAW1, 0);
    RA8875_writeReg(RA8875_HEAW0, (uint16_t) (_width - 1) & 0xFF); // horizontal end point
    RA8875_writeReg(RA8875_HEAW1, (uint16_t) (_width - 1) >> 8);

    /* Set active window Y */
    RA8875_writeReg(RA8875_VSAW0, 0); // vertical start point
    RA8875_writeReg(RA8875_VSAW1, 0);
    RA8875_writeReg(RA8875_VEAW0, (uint16_t) (_height - 1) & 0xFF); // horizontal end point
    RA8875_writeReg(RA8875_VEAW1, (uint16_t) (_height - 1) >> 8);

    /* ToDo: Setup touch panel? */

    /* Clear the entire window */
    RA8875_writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
    __delay_ms(500);
}

/**************************************************************************/
/*!
      Returns the display width in pixels
      
      @returns  The 1-based display width in pixels
 */

/**************************************************************************/
uint16_t RA8875_width(void) {
    return _width;
}

/**************************************************************************/
/*!
      Returns the display height in pixels

      @returns  The 1-based display height in pixels
 */

/**************************************************************************/
uint16_t RA8875_height(void) {
    return _height;
}

/************************* Text Mode ***********************************/

/**************************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)
 */

/**************************************************************************/
void RA8875_textMode(void) {
    /* Set text mode */
    RA8875_writeCommand(RA8875_MWCR0);
    unsigned char temp = RA8875_readData();
    temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
    RA8875_writeData(temp);

    /* Select the internal (ROM) font */
    RA8875_writeCommand(0x21);
    temp = RA8875_readData();
    temp &= ~((1 << 7) | (1 << 5)); // Clear bits 7 and 5
    RA8875_writeData(temp);
}

/**************************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)
      
      @args x[in] The x position of the cursor (in pixels, 0..1023)
      @args y[in] The y position of the cursor (in pixels, 0..511)
 */

/**************************************************************************/
void RA8875_textSetCursor(uint16_t x, uint16_t y) {
    /* Set cursor location */
    RA8875_writeCommand(0x2A);
    RA8875_writeData(x & 0xFF);
    RA8875_writeCommand(0x2B);
    RA8875_writeData(x >> 8);
    RA8875_writeCommand(0x2C);
    RA8875_writeData(y & 0xFF);
    RA8875_writeCommand(0x2D);
    RA8875_writeData(y >> 8);
}

/**************************************************************************/
/*!
      Sets the fore and background color when rendering text
      
      @args foreColor[in] The RGB565 color to use when rendering the text
      @args bgColor[in]   The RGB565 colot to use for the background
 */

/**************************************************************************/
void RA8875_textColor(uint16_t foreColor, uint16_t bgColor) {
    /* Set Fore Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((foreColor & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((foreColor & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((foreColor & 0x001f));

    /* Set Background Color */
    RA8875_writeCommand(0x60);
    RA8875_writeData((bgColor & 0xf800) >> 11);
    RA8875_writeCommand(0x61);
    RA8875_writeData((bgColor & 0x07e0) >> 5);
    RA8875_writeCommand(0x62);
    RA8875_writeData((bgColor & 0x001f));

    /* Clear transparency flag */
    RA8875_writeCommand(0x22);
    unsigned char temp = RA8875_readData();
    temp &= ~(1 << 6); // Clear bit 6
    RA8875_writeData(temp);
}

/**************************************************************************/
/*!
      Sets the fore color when rendering text with a transparent bg
      
      @args foreColor[in] The RGB565 color to use when rendering the text
 */

/**************************************************************************/
void RA8875_textTransparent(uint16_t foreColor) {
    /* Set Fore Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((foreColor & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((foreColor & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((foreColor & 0x001f));

    /* Set transparency flag */
    RA8875_writeCommand(0x22);
    unsigned char temp = RA8875_readData();
    temp |= (1 << 6); // Set bit 6
    RA8875_writeData(temp);
}

/**************************************************************************/
/*!
      Sets the text enlarge settings, using one of the following values:
      
      0 = 1x zoom
      1 = 2x zoom
      2 = 3x zoom
      3 = 4x zoom
      
      @args scale[in]   The zoom factor (0..3 for 1-4x zoom)
 */

/**************************************************************************/
void RA8875_textEnlarge(unsigned char scale) {
    if (scale > 3) scale = 3;

    /* Set font size flags */
    RA8875_writeCommand(0x22);
    unsigned char temp = RA8875_readData();
    temp &= ~(0xF); // Clears bits 0..3
    temp |= scale << 2;
    temp |= scale;
    RA8875_writeData(temp);

    _textScale = scale;
}

/**************************************************************************/
/*!
      Renders some text on the screen when in text mode
      
      @args buffer[in]    The buffer containing the characters to render
      @args len[in]       The size of the buffer in bytes
 */

/**************************************************************************/
void RA8875_textWrite(const char* buffer, uint16_t len) {
    RA8875_writeCommand(RA8875_MRWC);
    for (uint16_t i = 0; i < len; i++) {
        if(buffer[i] == '\n'){
            switch(_textScale){
                case 0:
                    RA8875_textSetCursor(10, (++textLine)*14);
                    break;
                case 1:
                    RA8875_textSetCursor(10, (++textLine)*24);
                    break;
                    case 2:
                    RA8875_textSetCursor(10, (++textLine)*44);
                    break;
                    case 3:
                    RA8875_textSetCursor(10, (++textLine)*84);
                    break;
            }
        }
        else{
            RA8875_writeData(buffer[i]);
        }
        __delay_ms(1);
    }
}

/************************* Graphics ***********************************/

/**************************************************************************/
/*!
      Sets the display in graphics mode (as opposed to text mode)
 */

/**************************************************************************/
void RA8875_graphicsMode(void) {
    RA8875_writeCommand(RA8875_MWCR0);
    unsigned char temp = RA8875_readData();
    temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
    RA8875_writeData(temp);
}

/**************************************************************************/
/*!
      Waits for screen to finish by polling the status!
 */

/**************************************************************************/
unsigned char RA8875_waitPoll(unsigned char regname, unsigned char waitflag) {
    /* Wait for the command to finish */
    while (1) {
        unsigned char temp = RA8875_readReg(regname);
        if (!(temp & waitflag))
            return TRUE;
    }
    return FALSE; // MEMEFIX: yeah i know, unreached! - add timeout?
}


/**************************************************************************/
/*!
      Sets the current X/Y position on the display before drawing
      
      @args x[in] The 0-based x location
      @args y[in] The 0-base y location
 */

/**************************************************************************/
void RA8875_setXY(uint16_t x, uint16_t y) {
    RA8875_writeReg(RA8875_CURH0, (uint8_t)x);
    RA8875_writeReg(RA8875_CURH1, (uint8_t)(x >> 8));
    RA8875_writeReg(RA8875_CURV0, (uint8_t)y);
    RA8875_writeReg(RA8875_CURV1, (uint8_t)(y >> 8));
}

/**************************************************************************/
/*!
      HW accelerated function to push a chunk of raw pixel data
      
      @args num[in] The number of pixels to push
      @args p[in]   The pixel color to use
 */

/**************************************************************************/
void RA8875_pushPixels(unsigned long num, uint16_t p) {
    DISP_CS_LAT = FALSE;
    SPI1Transfer(RA8875_DATAWRITE);
    while (num--) {
        SPI1Transfer((uint8_t)(p >> 8));
        SPI1Transfer((uint8_t)p);
    }
    DISP_CS_LAT = TRUE;
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_fillRect(void) {
    RA8875_writeCommand(RA8875_DCR);
    RA8875_writeData(RA8875_DCR_LINESQUTRI_STOP | RA8875_DCR_DRAWSQUARE);
    RA8875_writeData(RA8875_DCR_LINESQUTRI_START | RA8875_DCR_FILL | RA8875_DCR_DRAWSQUARE);
}

/**************************************************************************/
/*!
      Draws a single pixel at the specified location

      @args x[in]     The 0-based x location
      @args y[in]     The 0-base y location
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawPixel(int16_t x, int16_t y, uint16_t color) {
    RA8875_writeReg(RA8875_CURH0, (uint8_t)x);
    RA8875_writeReg(RA8875_CURH1, (uint8_t)(x >> 8));
    RA8875_writeReg(RA8875_CURV0, (uint8_t)y);
    RA8875_writeReg(RA8875_CURV1, (uint8_t)(y >> 8));
    RA8875_writeCommand(RA8875_MRWC);
    DISP_CS_LAT = FALSE;
    SPI1Transfer(RA8875_DATAWRITE);
    SPI1Transfer((uint8_t)(color >> 8));
    SPI1Transfer((uint8_t)color);
    DISP_CS_LAT = TRUE;
}

/**************************************************************************/
/*!
      Draws a HW accelerated line on the display
    
      @args x0[in]    The 0-based starting x location
      @args y0[in]    The 0-base starting y location
      @args x1[in]    The 0-based ending x location
      @args y1[in]    The 0-base ending y location
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    /* Set X */
    RA8875_writeCommand(0x91);
    RA8875_writeData((uint8_t)x0);
    RA8875_writeCommand(0x92);
    RA8875_writeData((uint8_t)(x0 >> 8));

    /* Set Y */
    RA8875_writeCommand(0x93);
    RA8875_writeData((uint8_t)y0);
    RA8875_writeCommand(0x94);
    RA8875_writeData((uint8_t)(y0 >> 8));

    /* Set X1 */
    RA8875_writeCommand(0x95);
    RA8875_writeData((uint8_t)x1);
    RA8875_writeCommand(0x96);
    RA8875_writeData((uint8_t)(x1 >> 8));

    /* Set Y1 */
    RA8875_writeCommand(0x97);
    RA8875_writeData((uint8_t)y1);
    RA8875_writeCommand(0x98);
    RA8875_writeData((uint8_t)(y1 >> 8));

    /* Set Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((color & 0x001f));

    /* Draw! */
    RA8875_writeCommand(RA8875_DCR);
    RA8875_writeData(0x80);

    /* Wait for the command to finish */
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    RA8875_drawLine(x, y, x, y + h, color);
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    RA8875_drawLine(x, y, x + w, y, color);
}

/**************************************************************************/
/*!
      Draws a HW accelerated rectangle on the display

      @args x[in]     The 0-based x location of the top-right corner
      @args y[in]     The 0-based y location of the top-right corner
      @args w[in]     The rectangle width
      @args h[in]     The rectangle height
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    RA8875_rectHelper(x, y, x + w, y + h, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled rectangle on the display

      @args x[in]     The 0-based x location of the top-right corner
      @args y[in]     The 0-based y location of the top-right corner
      @args w[in]     The rectangle width
      @args h[in]     The rectangle height
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_HWfillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    RA8875_rectHelper(x, y, x + w, y + h, color, TRUE);
}

/**************************************************************************/
/*!
      Fills the screen with the spefied RGB565 color

      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_fillScreen(uint16_t color) {
    RA8875_rectHelper(0, 0, _width - 1, _height - 1, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated circle on the display

      @args x[in]     The 0-based x location of the center of the circle
      @args y[in]     The 0-based y location of the center of the circle
      @args w[in]     The circle's radius
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    RA8875_circleHelper(x0, y0, r, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled circle on the display

      @args x[in]     The 0-based x location of the center of the circle
      @args y[in]     The 0-based y location of the center of the circle
      @args w[in]     The circle's radius
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    RA8875_circleHelper(x0, y0, r, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated triangle on the display

      @args x0[in]    The 0-based x location of point 0 on the triangle
      @args y0[in]    The 0-based y location of point 0 on the triangle
      @args x1[in]    The 0-based x location of point 1 on the triangle
      @args y1[in]    The 0-based y location of point 1 on the triangle
      @args x2[in]    The 0-based x location of point 2 on the triangle
      @args y2[in]    The 0-based y location of point 2 on the triangle
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    RA8875_triangleHelper(x0, y0, x1, y1, x2, y2, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled triangle on the display

      @args x0[in]    The 0-based x location of point 0 on the triangle
      @args y0[in]    The 0-based y location of point 0 on the triangle
      @args x1[in]    The 0-based x location of point 1 on the triangle
      @args y1[in]    The 0-based y location of point 1 on the triangle
      @args x2[in]    The 0-based x location of point 2 on the triangle
      @args y2[in]    The 0-based y location of point 2 on the triangle
      @args color[in] The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    RA8875_triangleHelper(x0, y0, x1, y1, x2, y2, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated ellipse on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args color[in]     The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawEllipse(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) {
    RA8875_ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled ellipse on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args color[in]     The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_fillEllipse(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) {
    RA8875_ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated curve on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args curvePart[in] The corner to draw, where in clock-wise motion:
                            0 = 180-270°
                            1 = 270-0°
                            2 = 0-90°
                            3 = 90-180°
      @args color[in]     The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_drawCurve(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, unsigned char curvePart, uint16_t color) {
    RA8875_curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled curve on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args curvePart[in] The corner to draw, where in clock-wise motion:
                            0 = 180-270°
                            1 = 270-0°
                            2 = 0-90°
                            3 = 90-180°
      @args color[in]     The RGB565 color to use when drawing the pixel
 */

/**************************************************************************/
void RA8875_fillCurve(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, unsigned char curvePart, uint16_t color) {
    RA8875_curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, TRUE);
}

/**************************************************************************/
/*!
      Helper function for higher level circle drawing code
 */

/**************************************************************************/
void RA8875_circleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color, unsigned char filled) {
    /* Set X */
    RA8875_writeCommand(0x99);
    RA8875_writeData((uint8_t)x0);
    RA8875_writeCommand(0x9a);
    RA8875_writeData((uint8_t)(x0 >> 8));

    /* Set Y */
    RA8875_writeCommand(0x9b);
    RA8875_writeData((uint8_t)y0);
    RA8875_writeCommand(0x9c);
    RA8875_writeData((uint8_t)(y0 >> 8));

    /* Set Radius */
    RA8875_writeCommand(0x9d);
    RA8875_writeData((uint8_t)r);

    /* Set Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((color & 0x001f));

    /* Draw! */
    RA8875_writeCommand(RA8875_DCR);
    if (filled) {
        RA8875_writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
    } else {
        RA8875_writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
    }

    /* Wait for the command to finish */
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level rectangle drawing code
 */

/**************************************************************************/
void RA8875_rectHelper(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, unsigned char filled) {
    /* Set X */
    RA8875_writeCommand(0x91);
    RA8875_writeData((uint8_t)x);
    RA8875_writeCommand(0x92);
    RA8875_writeData((uint8_t)(x >> 8));

    /* Set Y */
    RA8875_writeCommand(0x93);
    RA8875_writeData((uint8_t)y);
    RA8875_writeCommand(0x94);
    RA8875_writeData((uint8_t)(y >> 8));

    /* Set X1 */
    RA8875_writeCommand(0x95);
    RA8875_writeData((uint8_t)w);
    RA8875_writeCommand(0x96);
    RA8875_writeData((uint8_t)(w >> 8));

    /* Set Y1 */
    RA8875_writeCommand(0x97);
    RA8875_writeData((uint8_t)h);
    RA8875_writeCommand(0x98);
    RA8875_writeData((uint8_t)(h >> 8));

    /* Set Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((color & 0x001f));

    /* Draw! */
    RA8875_writeCommand(RA8875_DCR);
    if (filled) {
        RA8875_writeData(0xB0);
    } else {
        RA8875_writeData(0x90);
    }

    /* Wait for the command to finish */
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level triangle drawing code
 */

/**************************************************************************/
void RA8875_triangleHelper(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, unsigned char filled) {
    /* Set Point 0 */
    RA8875_writeCommand(0x91);
    RA8875_writeData((uint8_t)x0);
    RA8875_writeCommand(0x92);
    RA8875_writeData((uint8_t)(x0 >> 8));
    RA8875_writeCommand(0x93);
    RA8875_writeData((uint8_t)y0);
    RA8875_writeCommand(0x94);
    RA8875_writeData((uint8_t)(y0 >> 8));

    /* Set Point 1 */
    RA8875_writeCommand(0x95);
    RA8875_writeData((uint8_t)x1);
    RA8875_writeCommand(0x96);
    RA8875_writeData((uint8_t)(x1 >> 8));
    RA8875_writeCommand(0x97);
    RA8875_writeData((uint8_t)y1);
    RA8875_writeCommand(0x98);
    RA8875_writeData((uint8_t)(y1 >> 8));

    /* Set Point 2 */
    RA8875_writeCommand(0xA9);
    RA8875_writeData((uint8_t)x2);
    RA8875_writeCommand(0xAA);
    RA8875_writeData((uint8_t)(x2 >> 8));
    RA8875_writeCommand(0xAB);
    RA8875_writeData((uint8_t)y2);
    RA8875_writeCommand(0xAC);
    RA8875_writeData((uint8_t)(y2 >> 8));

    /* Set Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((color & 0x001f));

    /* Draw! */
    RA8875_writeCommand(RA8875_DCR);
    if (filled) {
        RA8875_writeData(0xA1);
    } else {
        RA8875_writeData(0x81);
    }

    /* Wait for the command to finish */
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level ellipse drawing code
 */

/**************************************************************************/
void RA8875_ellipseHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color, unsigned char filled) {
    /* Set Center Point */
    RA8875_writeCommand(0xA5);
    RA8875_writeData((uint8_t)xCenter);
    RA8875_writeCommand(0xA6);
    RA8875_writeData((uint8_t)(xCenter >> 8));
    RA8875_writeCommand(0xA7);
    RA8875_writeData((uint8_t)yCenter);
    RA8875_writeCommand(0xA8);
    RA8875_writeData((uint8_t)(yCenter >> 8));

    /* Set Long and Short Axis */
    RA8875_writeCommand(0xA1);
    RA8875_writeData((uint8_t)longAxis);
    RA8875_writeCommand(0xA2);
    RA8875_writeData((uint8_t)(longAxis >> 8));
    RA8875_writeCommand(0xA3);
    RA8875_writeData((uint8_t)shortAxis);
    RA8875_writeCommand(0xA4);
    RA8875_writeData((uint8_t)(shortAxis >> 8));

    /* Set Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((color & 0x001f));

    /* Draw! */
    RA8875_writeCommand(0xA0);
    if (filled) {
        RA8875_writeData(0xC0);
    } else {
        RA8875_writeData(0x80);
    }

    /* Wait for the command to finish */
    RA8875_waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level curve drawing code
 */

/**************************************************************************/
void RA8875_curveHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, unsigned char curvePart, uint16_t color, unsigned char filled) {
    /* Set Center Point */
    RA8875_writeCommand(0xA5);
    RA8875_writeData((uint8_t)xCenter);
    RA8875_writeCommand(0xA6);
    RA8875_writeData((uint8_t)(xCenter >> 8));
    RA8875_writeCommand(0xA7);
    RA8875_writeData((uint8_t)yCenter);
    RA8875_writeCommand(0xA8);
    RA8875_writeData((uint8_t)(yCenter >> 8));

    /* Set Long and Short Axis */
    RA8875_writeCommand(0xA1);
    RA8875_writeData((uint8_t)longAxis);
    RA8875_writeCommand(0xA2);
    RA8875_writeData((uint8_t)(longAxis >> 8));
    RA8875_writeCommand(0xA3);
    RA8875_writeData((uint8_t)shortAxis);
    RA8875_writeCommand(0xA4);
    RA8875_writeData((uint8_t)(shortAxis >> 8));

    /* Set Color */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xf800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07e0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData((color & 0x001f));

    /* Draw! */
    RA8875_writeCommand(0xA0);
    if (filled) {
        RA8875_writeData(0xD0 | (curvePart & 0x03));
    } else {
        RA8875_writeData(0x90 | (curvePart & 0x03));
    }

    /* Wait for the command to finish */
    RA8875_waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/************************* Mid Level ***********************************/

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_SetGPIOX(unsigned char on) {
    if (on)
        RA8875_writeReg(RA8875_GPIOX, 1);
    else
        RA8875_writeReg(RA8875_GPIOX, 0);
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_PWM1out(unsigned char p) {
    RA8875_writeReg(RA8875_P1DCR, p);
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_PWM2out(unsigned char p) {
    RA8875_writeReg(RA8875_P2DCR, p);
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_PWM1config(unsigned char on, unsigned char clock) {
    if (on) {
        RA8875_writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
    } else {
        RA8875_writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
    }
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_PWM2config(unsigned char on, unsigned char clock) {
    if (on) {
        RA8875_writeReg(RA8875_P2CR, RA8875_P2CR_ENABLE | (clock & 0xF));
    } else {
        RA8875_writeReg(RA8875_P2CR, RA8875_P2CR_DISABLE | (clock & 0xF));
    }
}

/**************************************************************************/
/*!
      Turns the display on or off
 */

/**************************************************************************/
void RA8875_displayOn(unsigned char on) {
    if (on)
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
    else
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

/**************************************************************************/
/*!
    Puts the display in sleep mode, or disables sleep mode if enabled
 */

/**************************************************************************/
void RA8875_sleep(unsigned char sleep) {
    if (sleep)
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
    else
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF);
}

/************************* Low Level ***********************************/

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_writeReg(unsigned char reg, unsigned char val) {
    RA8875_writeCommand(reg);
    RA8875_writeData(val);
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
unsigned char RA8875_readReg(unsigned char reg) {
    RA8875_writeCommand(reg);
    return RA8875_readData();
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_writeData(unsigned char d) {
    DISP_CS_LAT = FALSE;
    SPI1Transfer(RA8875_DATAWRITE);
    SPI1Transfer(d);
    DISP_CS_LAT = TRUE;
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
unsigned char RA8875_readData(void) {
    DISP_CS_LAT = FALSE;

    SPI1Transfer(RA8875_DATAREAD);
    unsigned char x = SPI1Transfer(0x0);

    DISP_CS_LAT = TRUE;
    return x;
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
void RA8875_writeCommand(unsigned char d) {
    DISP_CS_LAT = FALSE;

    SPI1Transfer(RA8875_CMDWRITE);
    SPI1Transfer(d);

    DISP_CS_LAT = TRUE;
}

/**************************************************************************/
/*!

 */

/**************************************************************************/
unsigned char RA8875_readStatus(void) {
    DISP_CS_LAT = FALSE;
    SPI1Transfer(RA8875_CMDREAD);
    unsigned char x = SPI1Transfer(0x0);
    DISP_CS_LAT = TRUE;
    return x;
}
