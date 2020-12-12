#ifndef MEM_H
#define MEM_H

#include "..//types.h"


inline void memcpy(void* src, void* dest, uint32 count) {
    void* dest_break = (uint8*) dest + count;

    while (dest < dest_break) {
        *((uint8*) dest++) = *((uint8*) src++);
    }


    // uint32 align_rem = ( (uint32) src % 4 );
    // align_rem = (align_rem) ? 4 - align_rem : align_rem;
    
    // if (align_rem != 0) {
    //     for (; --align_rem > 0; --count) {
    //         *((uint8*) dest++) = *((uint8*) src++);
    //     }
    // }

    // align_rem = (count % 4);
    // void* dest_break = (uint8*) dest + count - align_rem;

    // while (dest < dest_break) {
    //     *((uint32*) dest++) = *((uint32*) src++);
    // }

    // if (align_rem != 0) {
    //     for (; --align_rem > 0;) {
    //         *((uint8*) dest++) = *((uint8*) src++);
    //     }
    // }
}


#endif // MEM_H

