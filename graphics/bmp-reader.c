#include "bmp-reader.h"



uint8 read_bmp(void* file_buffer, struct BMPImage* bmp_image) {
    struct BMPImage* tmp_bmp_image = file_buffer;

    if (tmp_bmp_image->bf_type != 0x4D42 ||
        tmp_bmp_image->bi_planes != 1 ||
        tmp_bmp_image->bi_bit_count != 24 ||
        tmp_bmp_image->bi_compression != 0) {
            return 0;
    }

    *bmp_image = *tmp_bmp_image;

    bmp_image->bitmap = (uint8*) file_buffer + tmp_bmp_image->bf_offBits;
    bmp_image->bytes_per_scanline = 4 * ( (3 * tmp_bmp_image->bi_width / 4) +
                                        ( (tmp_bmp_image->bi_width % 4 == 0)? 0 : 1 ) );
    bmp_image->scale = 1;


    return 1;
}

