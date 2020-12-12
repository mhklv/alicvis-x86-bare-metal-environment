#include "psf1-reader.h"
#include "misc/allocator.h"



uint8 read_psf1(void* file_buffer, struct PSF1File* psf1_file) {
    struct PSF1File* tmp_psf1_file = file_buffer;

    if (tmp_psf1_file->magic[0] != PSF1_MAGIC0 ||
        tmp_psf1_file->magic[1] != PSF1_MAGIC1 ||
        tmp_psf1_file->mode != PSF1_MODEHASTAB) {
        return 0;
    }

    *psf1_file = *tmp_psf1_file;
    psf1_file->bitmaps = file_buffer + 0x4;

    psf1_file->glyph_pos = malloc(0x10000);
    uint32 i;

    for (i = 0; i < 0x10000; ++i) {
        psf1_file->glyph_pos[i] = 1;
    }

    uint16* uni_info = (uint16*) ( psf1_file->bitmaps + psf1_file->charsize * 256 );
    uint32 cnt;

    for (cnt = 0, i = 0; cnt < 256; ++i) {
        if (uni_info[i] == PSF1_SEPARATOR) {
            ++cnt;
        }
        else {
            psf1_file->glyph_pos[uni_info[i]] = cnt;
        }
    }

    return 1;
}


// PSF1_STARTSEQ remains to be unused
uint8* get_glyph(struct PSF1File* psf1_file, uint16 character) {
    return psf1_file->bitmaps + psf1_file->charsize * psf1_file->glyph_pos[character];
    // return psf1_file->bitmaps + psf1_file->charsize * character;
}



