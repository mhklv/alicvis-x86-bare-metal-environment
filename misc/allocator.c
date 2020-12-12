#include "allocator.h"


static uint32 map_factor;
static uint32 blocks_count;
static uint8* bitmap_base;
static void* arena_base;

enum Bitmap_state {
    FREE_BLOCK,
    USED_BLOCK,
    BOUNDARY_BLOCK
};


void init_allocator(void* area_base, uint32 area_size, uint32 factor) {
    map_factor = factor;
    blocks_count = area_size / (1 + factor);
    bitmap_base = (uint8*) ( (uint32) area_base + blocks_count * factor );
    arena_base = area_base;

    uint32 i;

    for (i = 0; i < blocks_count; ++i) {
        bitmap_base[i] = FREE_BLOCK;
    }
}


void* malloc(uint32 size) {
    uint32 blocks_required = size / map_factor + (size % map_factor != 0);
    uint32 i, cur_free_interval_size = 0, start_block;

    for (i = 0; i < blocks_count; ++i) {
        if (bitmap_base[i] == FREE_BLOCK) {
            ++cur_free_interval_size;
        }
        else {
            cur_free_interval_size = 0;
        }

        if (cur_free_interval_size >= blocks_required) {
            start_block = i + 1 - cur_free_interval_size;

            bitmap_base[start_block] = BOUNDARY_BLOCK;
            
            for (i = start_block + 1; i < start_block + cur_free_interval_size; ++i) {
                bitmap_base[i] = USED_BLOCK;
            }

            return (void*) ( (uint32) arena_base + map_factor * start_block );
        }
    }

    return 0;
}


void free(void* ptr) {
    if (!ptr ||
        ptr < arena_base ||
        ptr > (void*) ( (uint32) arena_base + map_factor * blocks_count ) ||
        ( (uint32) ptr - (uint32) arena_base ) % map_factor) {

        return;
    }

    uint32 i, start_block = ( (uint32) ptr - (uint32) arena_base ) / map_factor;

    if (bitmap_base[start_block] != BOUNDARY_BLOCK) {
        return;
    }

    bitmap_base[start_block] = FREE_BLOCK;

    for (i = start_block + 1; i < blocks_count; ++i) {
        if (bitmap_base[i] == USED_BLOCK) {
            bitmap_base[i] = FREE_BLOCK;    
        }
        else {
            break;
        }
    }
}


