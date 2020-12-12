#ifndef CIRCQUEUE_H
#define CIRCQUEUE_H

#include "types.h"



struct CircQueue8 {
    uint8* front;
    uint8* rear;
    uint8* zone_start;
    uint8* zone_end;
    uint32 size;
};


void init_circqueue(struct CircQueue8* cqueue, uint32 size);
void destroy_circqueue(struct CircQueue8* cqueue);
uint8 queue_is_empty(struct CircQueue8* cqueue);
void flush_circueue(struct CircQueue8* cqueue);
void enqueue(struct CircQueue8* cqueue, uint8 val);
uint8 dequeue(struct CircQueue8* cqueue);







#endif // CIRCQUEUE_H