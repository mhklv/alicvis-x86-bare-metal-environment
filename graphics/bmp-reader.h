#ifndef BMP_READER_H
#define BMP_READER_H

#include "types.h"



struct __attribute__((__packed__)) BMPImage {

    uint16 bf_type;
    uint32 bf_size;
    uint16 bf_reserved1;
    uint16 bf_reserved2;
    uint32 bf_offBits;

    uint32 bi_size;
    uint32 bi_width;
    uint32 bi_height;
    uint16 bi_planes;
    uint16 bi_bit_count;
    uint32 bi_compression;
    uint32 bi_size_image;
    uint32 bi_x_pels_per_meter;
    uint32 bi_y_pels_per_meter;
    uint32 bi_clr_used;
    uint32 bi_clr_important;

    uint32 bytes_per_scanline;
    uint32 scale;
    uint8* bitmap;
};


uint8 read_bmp(void* file_buffer, struct BMPImage* bmp_image);


#endif // BMP_READER_H
