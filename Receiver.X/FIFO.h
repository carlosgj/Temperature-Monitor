#ifndef FIFO_H
#define	FIFO_H

#include <xc.h> 

//#define FIFO_DEBUG

#ifdef FIFO_DEBUG
#include <stdio.h>
#endif

typedef struct FIFO_t {
    unsigned char buffer[256];
    unsigned char read;
    unsigned char write;
    unsigned char overflowCount;
    unsigned char used;
} FIFO;

void FIFOInit(FIFO* fifo);
unsigned char FIFOPush(FIFO* fifo, unsigned char value);
unsigned char FIFOPop(FIFO* fifo, unsigned char *value);
unsigned char FIFOGetFree(FIFO* fifo);
unsigned char FIFOGetUsed(FIFO* fifo);

#endif

