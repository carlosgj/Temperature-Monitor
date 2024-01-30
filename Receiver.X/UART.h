#ifndef UART_H
#define	UART_H
 
#include "common.h"
//#include "commConstants.h"
#include "FIFO.h"

void debug_UART_init(void);
void putch(unsigned char theByte);

#endif

