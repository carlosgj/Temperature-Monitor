#ifndef PINDEF_H
#define	PINDEF_H

#include <xc.h> 

#define SCK_TRIS TRISBbits.TRISB1
#define MOSI_TRIS TRISDbits.TRISD4
#define MISO_TRIS TRISBbits.TRISB2

#define MEM1CS_TRIS TRISDbits.TRISD6
#define MEM1CS_LAT LATDbits.LATD6

#define RADIOCS_TRIS TRISCbits.TRISC5
#define RADIOCS_LAT LATCbits.LATC5

#define MEM2CS_TRIS TRISCbits.TRISC6
#define MEM2CS_LAT  LATCbits.LATC6


#define CS_TRIS     TRISDbits.TRISD2
#define CS_LAT      LATDbits.LATD2

#endif	/* XC_HEADER_TEMPLATE_H */

