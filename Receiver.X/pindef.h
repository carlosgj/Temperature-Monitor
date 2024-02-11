#ifndef PINDEF_H
#define	PINDEF_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define THERM_TRIS          TRISCbits.TRISC5

#define SCK1_TRIS           TRISCbits.TRISC3
#define MOSI1_TRIS          TRISCbits.TRISC1
#define MISO1_TRIS          TRISCbits.TRISC2

#define DISP_CS_TRIS        TRISDbits.TRISD7
#define DISP_CS_LAT         LATDbits.LATD7
#define DISP_RST_TRIS       TRISDbits.TRISD6
#define DISP_RST_LAT        LATDbits.LATD6

//#define I2C_SCK_TRIS        TRISBbits.TRISB5
//#define I2C_SDA_TRIS        TRISBbits.TRISB4
//#define RTC_RST_TRIS        TRISEbits.TRISE0
//#define RTC_RST_LAT         LATEbits.LATE0

#define SCK2_TRIS           TRISDbits.TRISD1
#define MOSI2_TRIS          TRISDbits.TRISD3
#define MISO2_TRIS          TRISDbits.TRISD2
#define SD_CS_TRIS          TRISDbits.TRISD4
#define SD_CS_LAT           LATDbits.LATD4
#define RFM69_CS_TRIS       TRISCbits.TRISC5
#define RFM69_CS_LAT        LATCbits.LATC5

#define RFM69_RST_TRIS      TRISCbits.TRISC6
#define RFM69_RST_LAT       LATCbits.LATC6

#define BUTTON1_TRIS        TRISAbits.TRISA0
#define BUTTON2_TRIS        TRISAbits.TRISA1
#define BUTTON3_TRIS        TRISAbits.TRISA2
#define BUTTON4_TRIS        TRISAbits.TRISA3
#define BUTTON5_TRIS        TRISAbits.TRISA4
#define BUTTON6_TRIS        TRISAbits.TRISA5

#define BUTTON1_WPU         WPUAbits.WPUA0
#define BUTTON2_WPU         WPUAbits.WPUA1
#define BUTTON3_WPU         WPUAbits.WPUA2
#define BUTTON4_WPU         WPUAbits.WPUA3
#define BUTTON5_WPU         WPUAbits.WPUA4
#define BUTTON6_WPU         WPUAbits.WPUA5

#endif	/* XC_HEADER_TEMPLATE_H */

