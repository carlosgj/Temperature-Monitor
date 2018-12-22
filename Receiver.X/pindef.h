#ifndef PINDEF_H
#define	PINDEF_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define RFM69_CS_TRIS       TRISCbits.TRISC5
#define RFM69_CS_LAT        LATCbits.LATC5

#define SCK1_TRIS           TRISCbits.TRISC3
#define MOSI1_TRIS          TRISBbits.TRISB3
#define MISO1_TRIS          TRISCbits.TRISC4

#define DISP_CS_TRIS        TRISAbits.TRISA4
#define DISP_CS_LAT         LATAbits.LATA4
#define DISP_RST_TRIS       TRISEbits.TRISE1
#define DISP_RST_LAT        LATEbits.LATE1

#define SCK2_TRIS           TRISBbits.TRISB1
#define MOSI2_TRIS          TRISDbits.TRISD4
#define MISO2_TRIS          TRISBbits.TRISB2

#define MEM1_CS_TRIS        TRISDbits.TRISD6
#define MEM1_CS_LAT         LATDbits.LATD6

#define MEM2_CS_TRIS        TRISCbits.TRISC6
#define MEM2_CS_LAT         LATCbits.LATC6


#define RTC_CS_TRIS         TRISDbits.TRISD2
#define RTC_CS_LAT          LATDbits.LATD2

#define BUTTON1_TRIS        TRISCbits.TRISC0
#define BUTTON2_TRIS        TRISCbits.TRISC1
#define BUTTON3_TRIS        TRISCbits.TRISC2
#define BUTTON4_TRIS        TRISDbits.TRISD0
#define BUTTON5_TRIS        TRISDbits.TRISD1
#define BUTTON6_TRIS        TRISDbits.TRISD3

#define BUTTON1_WPU         WPUCbits.WPUC0
#define BUTTON2_WPU         WPUCbits.WPUC1
#define BUTTON3_WPU         WPUCbits.WPUC2
#define BUTTON4_WPU         WPUDbits.WPUD0
#define BUTTON5_WPU         WPUDbits.WPUD1
#define BUTTON6_WPU         WPUDbits.WPUD3

#define BUTTON1_PORT        PORTCbits.RC0
#define BUTTON2_PORT        PORTCbits.RC1
#define BUTTON3_PORT        PORTCbits.RC2
#define BUTTON4_PORT        PORTDbits.RD0
#define BUTTON5_PORT        PORTDbits.RD1
#define BUTTON6_PORT        PORTDbits.RD3

#endif	/* XC_HEADER_TEMPLATE_H */

