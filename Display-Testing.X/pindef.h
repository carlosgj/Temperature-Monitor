#ifndef PINDEF_H
#define	PINDEF_H

#include <xc.h> 

#define SCK_TRIS TRISBbits.TRISB1
#define MOSI_TRIS TRISCbits.TRISC7
#define MISO_TRIS TRISBbits.TRISB0

#define CS_TRIS     TRISBbits.TRISB2
#define CS_LAT      LATBbits.LATB2

#define RST_TRIS    TRISBbits.TRISB3
#define RST_LAT     LATBbits.LATB3

#endif	/* XC_HEADER_TEMPLATE_H */

