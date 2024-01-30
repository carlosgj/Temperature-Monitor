#include <xc.h>
#include "FIFO.h"

void FIFOInit(FIFO* fifo){
    fifo->read = 0;
    fifo->write = 0;
    fifo->overflowCount = 0;
}

unsigned char FIFOPush(FIFO* fifo, unsigned char value){
    if(fifo->used < 255){
#ifdef FIFO_DEBUG
        printf("-> 0x%02x r:%d w:%d u:%d\n", value, fifo->read, fifo->write, fifo->used);
#endif
        fifo->buffer[fifo->write++] = value;
        fifo->used++;
        return 0;
    }
    else{
#ifdef FIFO_DEBUG
        printf("FIFO Overflow!\n");
#endif
        fifo->overflowCount++;
        return 1;
    }
}

unsigned char FIFOPop(FIFO* fifo, unsigned char *value){
    if(fifo->used > 0){
        *value = fifo->buffer[fifo->read++];
#ifdef FIFO_DEBUG
        printf("<- 0x%02x r:%d w:%d u:%d\n", *value, (fifo->read)-1, fifo->write, fifo->used);
#endif
        fifo->used--;
        return 0;
    }
    else{
        return 1;
    }
}

unsigned char FIFOGetUsed(FIFO* fifo){
    return fifo->used;
}