#ifndef PSF1_READER_H
#define PSF1_READER_H

#include "types.h"



#define PSF1_MAGIC0     0x36
#define PSF1_MAGIC1     0x04

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

#define PSF1_GLYPH_WIDTH 8
#define PSF1_LENGTH 256


struct __attribute__((__packed__)) PSF1File {
    uint8 magic[2];     // Magic number
    uint8 mode;         // PSF font mode
    uint8 charsize;     // Character size
    
    uint8* bitmaps;
    uint8* glyph_pos;
};


uint8 read_psf1(void* file_buffer, struct PSF1File* psf1_file);
uint8* get_glyph(struct PSF1File* psf1_file, uint16 character);   // Returns pointer to character's
                                                                  // bitmap of the specifyied Unicode value


#endif // PSF1_READER_H
