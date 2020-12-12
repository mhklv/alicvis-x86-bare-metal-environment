#include "types.h"
#include "circqueue.h"
#include "misc/allocator.h"



// void inline inc_front(struct CircQueue8* cqueue) {
//     if (cqueue->front >= cqueue->zone_end) {
//         cqueue->front = cqueue->zone_start;
//     }
//     else {
//         cqueue->front += 1;
//     }
// }


// void inline inc_rear(struct CircQueue8* cqueue) {
//     if (cqueue->rear >= cqueue->zone_end) {
//         cqueue->rear = cqueue->zone_start;
//     }
//     else {
//         cqueue->rear += 1;
//     }
// }


void inline static dec_front(struct CircQueue8* cqueue) {
    if (cqueue->front <= cqueue->zone_start) {
        cqueue->front = cqueue->zone_end;
    }
    else {
        cqueue->front -= 1;
    }
}


void inline static dec_rear(struct CircQueue8* cqueue) {
    if (cqueue->rear <= cqueue->zone_start) {
        cqueue->rear = cqueue->zone_end;
    }
    else {
        cqueue->rear -= 1;
    }
}



void init_circqueue(struct CircQueue8* cqueue, uint32 size) {
    cqueue->zone_start = malloc(size);
    cqueue->zone_end = cqueue->zone_start + size - 1;
    cqueue->front = cqueue->zone_start;
    cqueue->rear = cqueue->zone_start;
    cqueue->size = 0;
}


void destroy_circqueue(struct CircQueue8* cqueue) {
    free(cqueue->zone_start);
}


uint8 queue_is_empty(struct CircQueue8* cqueue) {
    return (cqueue->size == 0) ? 1 : 0;
    // return (cqueue->rear == cqueue->front);
}


void flush_circueue(struct CircQueue8* cqueue) {
    while (cqueue->size > 0) {
        dec_rear(cqueue);
        cqueue->size -= 1;
    }
}


void enqueue(struct CircQueue8* cqueue, uint8 val) {
    dec_front(cqueue);

    if (cqueue->front == cqueue->rear) {
        dec_rear(cqueue);
    }
    else {
        cqueue->size += 1;
    }

    *cqueue->front = val;
}


uint8 dequeue(struct CircQueue8* cqueue) {
    if (cqueue->size == 0) {
        return 0;
    }
    
    dec_rear(cqueue);

    cqueue->size -= 1;

    return *cqueue->rear;
}


