#ifndef PINDEF_H
#define	PINDEF_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define RFM69_CS_TRIS       TRISCbits.TRISC5
#define RFM69_CS_LAT        LATCbits.LATC5

#define SCK2_TRIS           TRISBbits.TRISB1
#define MOSI2_TRIS          TRISDbits.TRISD4
#define MISO2_TRIS          TRISBbits.TRISB2

#define MEM1_CS_TRIS         TRISDbits.TRISD6
#define MEM1_CS_LAT          LATDbits.LATD6

#define MEM2_CS_TRIS         TRISCbits.TRISC6
#define MEM2_CS_LAT          LATCbits.LATC6


#define RTC_CS_TRIS         TRISDbits.TRISD2
#define RTC_CS_LAT          LATDbits.LATD2

#endif	/* XC_HEADER_TEMPLATE_H */

