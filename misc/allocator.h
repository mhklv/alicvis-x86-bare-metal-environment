#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "types.h"


void init_allocator(void* area_base, uint32 area_size, uint32 factor);
void* malloc(uint32 size);
void free(void* ptr);



#endif // ALLOCATOR_H