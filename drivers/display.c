#include "types.h"
#include "drivers/display.h"
#include "misc/mem.h"
#include "misc/allocator.h"
#include "graphics/bmp-reader.h"



extern uint32 _FRAMEBUFFER_ADDRESS;
extern uint32 _FRAMEBUFFER_WIDTH;
extern uint32 _FRAMEBUFFER_HEIGHT;
extern uint32 _FRAMEBUFFER_DEPTH;
extern uint32 _FRAMEBUFFER_BYTES_PER_LINE;

static uint32 FRAMEBUFFER_ADDRESS = 0;
static uint32 WIDTH = 0;
static uint32 HEIGHT = 0;
static uint32 DEPTH = 0;
static uint32 BYTES_PER_LINE = 0;

static uint32 cursor_x = 0, cursor_y = 0;

static uint8 red_brush = 255;
static uint8 green_brush = 255;
static uint8 blue_brush = 255;

static uint32 BACKBUFFER = 0;

static struct PSF1File* font_file;
static uint8 font_size;

static uint16* text_mode_char_table;


extern inline void memcpy(void* src, void* dest, uint32 count);



uint32 screen_width() {
    return WIDTH;
}

uint32 screen_height() {
    return HEIGHT;
}


void init_video() {
    FRAMEBUFFER_ADDRESS = *(&_FRAMEBUFFER_ADDRESS);
    WIDTH = *(&_FRAMEBUFFER_WIDTH);
    HEIGHT = *(&_FRAMEBUFFER_HEIGHT);
    DEPTH = *(&_FRAMEBUFFER_DEPTH);
    BYTES_PER_LINE = *(&_FRAMEBUFFER_BYTES_PER_LINE);

    BACKBUFFER = (uint32) malloc(HEIGHT * BYTES_PER_LINE);
    
    uint32 i;
    
    for (i = 0; i < BYTES_PER_LINE * HEIGHT / 4; ++i) {
        *((uint32*) BACKBUFFER + i) = 0;
    }

    text_mode_char_table = malloc(HEIGHT * WIDTH / 8);

    for (i = 0; i < HEIGHT * WIDTH / 8; ++i) {
        text_mode_char_table[i] = 0;
    }
}


void inline plot_pixel(uint32 x, uint32 y) {

    // uint8 fourth_byte = *( (uint8*) BACKBUFFER + (DEPTH/8)*(x+WIDTH*y) + 3);
    
    // uint32 pixel = (uint32) blue_brush | ( (uint32) green_brush << 8 ) |
    //                ( (uint32) red_brush << 16 ) | ( (uint32) fourth_byte << 24 );

    // *( (uint32*) (BACKBUFFER + (DEPTH/8)*(x+WIDTH*y)) ) = pixel;

    uint8 fourth_byte = *( (uint8*) BACKBUFFER + 3*x + BYTES_PER_LINE*y + 3);
    
    uint32 pixel = (uint32) blue_brush | ( (uint32) green_brush << 8 ) |
                   ( (uint32) red_brush << 16 ) | ( (uint32) fourth_byte << 24 );

    *( (uint32*) (BACKBUFFER + 3*x + BYTES_PER_LINE*y) ) = pixel;



    // *( (uint8*) BACKBUFFER + (DEPTH/8)*(x+WIDTH*y) ) = blue_brush;
    // *( (uint8*) BACKBUFFER + (DEPTH/8)*(x+WIDTH*y) + 1) = green_brush;
    // *( (uint8*) BACKBUFFER + (DEPTH/8)*(x+WIDTH*y) + 2) = red_brush;
}


void set_pixel_col(uint8 r, uint8 g, uint8 b) {
    red_brush = r;
    green_brush = g;
    blue_brush = b;
}


uint8 red() {
    return red_brush;
}

uint8 green() {
    return green_brush;
}

uint8 blue() {
    return blue_brush;
}


void refresh_screen()
{
    // memcpy((void*) BACKBUFFER, (void*) FRAMEBUFFER_ADDRESS, 3 * HEIGHT * WIDTH);

    // uint32* dst_break = (uint32*) (FRAMEBUFFER_ADDRESS + HEIGHT * BYTES_PER_LINE);
    // uint32* i_src = (uint32*) BACKBUFFER;
    // uint32* i_dst = (uint32*) FRAMEBUFFER_ADDRESS;

    // for (i_src, i_dst; i_dst < dst_break; ++i_src, ++i_dst) {
    //     *i_dst = *i_src;
    // }


    uint32* base_src = (uint32*) BACKBUFFER;
    uint32* base_dst = (uint32*) FRAMEBUFFER_ADDRESS;
    uint32 i, j;

    for (i = 0; i < HEIGHT; ++i) {
        base_src = (uint32*) (BACKBUFFER + i*BYTES_PER_LINE);
        base_dst = (uint32*) (FRAMEBUFFER_ADDRESS + i*BYTES_PER_LINE);
        for (j = 0; j < 3 * WIDTH / 4; ++j) {
            base_dst[j] = base_src[j];    
        }
    }

    
    for (i = 0; i < BYTES_PER_LINE * HEIGHT / 4; ++i) {
        *((uint32*) BACKBUFFER + i) = 0;
    }


    // asm volatile (
    //     "1:"
    //     "movl (%[i_src]), %%eax;"
    //     "movl 4(%[i_src]), %%ebx;"
    //     "movl 8(%[i_src]), %%edx;"
    //     "movl %%eax, (%[i_dst]);"
    //     "movl %%ebx, 4(%[i_dst]);"
    //     "movl %%edx, 8(%[i_dst]);"
    //     "addl $12, %[i_dst];"
    //     "addl $12, %[i_src];"
    //     "cmpl %[i_dst], %[dst_break];"
    //     "jne 1b;"
    //     : [i_dst] "+r" (i_dst),
    //       [i_src] "+r" (i_src),
    //       [dst_break] "+r" (dst_break)
    //     :
    //     : "memory", "eax", "ebx", "edx"
    //     );
}


void update_screen() {

    uint32* base_src = (uint32*) BACKBUFFER;
    uint32* base_dst = (uint32*) FRAMEBUFFER_ADDRESS;
    uint32 i, j;

    for (i = 0; i < HEIGHT; ++i) {
        base_src = (uint32*) (BACKBUFFER + i*BYTES_PER_LINE);
        base_dst = (uint32*) (FRAMEBUFFER_ADDRESS + i*BYTES_PER_LINE);
        for (j = 0; j < 3 * WIDTH / 4; ++j) {
            base_dst[j] = base_src[j];    
        }
    }
}


void clear_screen_buffer() {
    uint32 i;
    
    for (i = 0; i < BYTES_PER_LINE * HEIGHT / 4; ++i) {
        *((uint32*) BACKBUFFER + i) = 0;
    }
}






void draw_rect(struct Rectangle* rect) {
    uint32 i, j;

    if (rect->x + rect->width > WIDTH || rect->x > WIDTH) {
        rect->x = 0;
    }

    if (rect->y + rect->height > screen_height() || rect->y > screen_height()) {
        rect->y = 0;
    }

    
    for (i = rect->y; i < rect->y + rect->height; ++i) {
        for (j = rect->x; j < rect->x + rect->width; ++j) {
            plot_pixel(j, i);
        }
    }
    // rect->active = 1;
}


void move_rect(struct Rectangle* rect, int32 delta_x,
               int32 delta_y, int32 delta_width, int32 delta_height,
               uint8 r, uint8 g, uint8 b) {
    if (rect->active) {
        set_pixel_col(0, 0, 0);
        draw_rect(rect);
        set_pixel_col(r, g, b);
    }
    rect->x += delta_x;
    rect->y += delta_y;
    rect->width += delta_width;
    rect->height += delta_height;
    if (rect->active) {
        draw_rect(rect);
    }
}


void draw_bmp(uint16 x, uint16 y, struct BMPImage* bmp_image) {
    uint8 r, g, b;
    uint16 row, col, display_row;
    uint16 dwords_per_scanline, remainder_bytes;
    uint32 i, j, img_scale;
    
    
    if ( x + bmp_image->bi_width * bmp_image->scale < WIDTH &&
         y + bmp_image->bi_height * bmp_image->scale < HEIGHT ) {
            for (row = bmp_image->bi_height; row-- > 0;) {
                // dwords_per_scanline = bmp_image->bytes_per_scanline / 4;
                // remainder_bytes = bmp_image->bytes_per_scanline - dwords_per_scanline;

                // for (i = 0; i < dwords_per_scanline; ++i) {
                //     *((uint32*) (BACKBUFFER + x + y * BYTES_PER_LINE + row * bmp_image->bytes_per_scanline + 4 * i)) =
                //     bmp_image->bitmap[row * bmp_image->bytes_per_scanline + 4 * i];
                // }

                // for (i = 0; i < remainder_bytes; ++i) {

                // }


                for (col = 0; col < bmp_image->bi_width; ++col) {                    
                    b = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3];
                    g = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 1];
                    r = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 2];

                    set_pixel_col(r, g, b);

                    img_scale = bmp_image->scale;
                    display_row = bmp_image->bi_height - row;

                    for (i = 0; i < img_scale; ++i) {
                        for (j = 0; j < img_scale; ++j) {
                            plot_pixel(x + col * img_scale + j,
                                       y + display_row * img_scale + i);

                        }
                    }
                    
                    // plot_pixel(x + col, y + bmp_image->bi_height - row);
                }
            }
        }
}


void bmp_change_brightness(struct BMPImage* bmp_image, float mult) {
    uint32 r, g, b;
    uint16 row, col;
    uint32 i, j;
    mult = (mult < 0) ? -mult : mult;
    
    for (row = bmp_image->bi_height; row-- > 0;) {
        for (col = 0; col < bmp_image->bi_width; ++col) {                    
            b = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3];
            g = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 1];
            r = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 2];

            b = (b * mult > 255) ? 255 : b * mult;
            g = (g * mult > 255) ? 255 : g * mult;
            r = (r * mult > 255) ? 255 : r * mult;

            bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3] = b; 
            bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 1] = g; 
            bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 2] = r; 
        }
    }
}


void draw_cursor_bmp(uint16 x, uint16 y, struct BMPImage* bmp_image) {
    uint8 r, g, b;
    uint16 row, col, display_row;
    uint16 dwords_per_scanline, remainder_bytes;
    uint32 i, j, img_scale;
    
    
    for (row = bmp_image->bi_height; row-- > 0;) {
        for (col = 0; col < bmp_image->bi_width; ++col) {
            if (y + bmp_image->bi_height - row - 1 < HEIGHT && x + col < WIDTH) {
                b = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3];
                g = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 1];
                r = bmp_image->bitmap[row * bmp_image->bytes_per_scanline + col * 3 + 2];

                if (r != 0 || g != 0 || b != 0) {
                    set_pixel_col(r, g, b);

                    img_scale = bmp_image->scale;
                    display_row = bmp_image->bi_height - row;

                    for (i = 0; i < img_scale; ++i) {
                        for (j = 0; j < img_scale; ++j) {
                            plot_pixel(x + col * img_scale + j,
                                    y + display_row * img_scale + i);

                        }
                    }
                }

            }
        }
    }
        
}



void set_font(struct PSF1File* psf1_file, uint8 size) {
    font_file = psf1_file;
    font_size = size;
}


void text_mode_set_font_size(uint8 size) {
    if (size > 16) {
        font_size = 16;
    }
    else {
        font_size = size;
    }

    text_mode_clear_screen();
    clear_screen_buffer();
}


uint8 text_mode_get_font_size() { return font_size; }


void draw_char(uint16 x, uint16 y, uint16 character) {
    uint8* bitmap = get_glyph(font_file, character);
    uint8 char_line;
    uint32 row;
    int8 i;
    struct Rectangle rect;
    

    for (row = 0; row < font_file->charsize; ++row) {
        char_line = bitmap[row];
        
        for (i = PSF1_GLYPH_WIDTH - 1; i >= 0; --i) {
            if ((char_line >> i) & 0x01) {
                // plot_pixel(x + font_size * (PSF1_GLYPH_WIDTH - i - 1), y + font_size * row);

                rect.x = x + font_size * (PSF1_GLYPH_WIDTH - i - 1);
                rect.y = y + font_size * row;
                rect.width = font_size;
                rect.height = font_size;
                draw_rect(&rect);

            }
        }
    }
}



uint32 text_mode_width() {
    return WIDTH / (PSF1_GLYPH_WIDTH * font_size);
}


uint32 text_mode_height() {
    return HEIGHT / (font_file->charsize * font_size);
}


void text_mode_dump_screen(uint16 *dump_buffer) {
    uint32 i;

    for (i = 0; i < text_mode_width() * text_mode_height(); ++i) {
        dump_buffer[i] = text_mode_char_table[i];
    }
}


void text_mode_extract_screen_dump(uint16 *dump_buffer) {
    uint32 x, y, i = 0;

    text_mode_clear_screen();

    for (y = 0; y < text_mode_height(); ++y) {
        for (x = 0; x < text_mode_width(); ++x) {
            if (dump_buffer[i] != 0) {
                text_mode_draw_char(x, y, dump_buffer[i]);
            }
            ++i;
        }
    }
}


void text_mode_draw_char(uint16 x, uint16 y, uint16 character) {
    uint16* char_table_pos = text_mode_char_table + y * text_mode_width() + x;

    if (*char_table_pos != 0) {
        text_mode_del_char(x, y);
    }
    
    draw_char(x * PSF1_GLYPH_WIDTH * font_size, y * font_file->charsize * font_size, character);
    *char_table_pos = character;
}


void text_mode_draw_charplace_solid(uint16 x, uint16 y) {
    struct Rectangle charplace_rect;

    charplace_rect.x = x * PSF1_GLYPH_WIDTH * font_size;
    charplace_rect.y = y * font_file->charsize * font_size;
    charplace_rect.width = PSF1_GLYPH_WIDTH * font_size;
    charplace_rect.height = font_file->charsize * font_size;

    draw_rect(&charplace_rect);
}


uint16 text_mode_get_char(uint16 x, uint16 y) {
    return *(text_mode_char_table + y * text_mode_width() + x);
}


void text_mode_del_char(uint16 x, uint16 y) {
    int8 r, g, b;
    
    struct Rectangle charplace_rect;

    charplace_rect.x = x * PSF1_GLYPH_WIDTH * font_size;
    charplace_rect.y = y * font_file->charsize * font_size;
    charplace_rect.width = PSF1_GLYPH_WIDTH * font_size;
    charplace_rect.height = font_file->charsize * font_size;

    r = red_brush, b = blue_brush, g = green_brush;
    set_pixel_col(0, 0, 0);

    draw_rect(&charplace_rect);
    set_pixel_col(r, g, b);

    *(text_mode_char_table + y * text_mode_width() + x) = 0;
}


void text_mode_scroll_down(uint8 lines) {
    if (lines == 0 || lines >= text_mode_height()) {
        clear_screen_buffer();
        return;
    }

    uint32 i, j;
    uint16 lower_character;
    
    for (i = 0; i < text_mode_height() - lines; ++i) {
        for (j = 0; j < text_mode_width(); ++j) {
            text_mode_del_char(j, i);
            lower_character = text_mode_get_char(j, i + lines);

            if (lower_character != 0) {
                text_mode_draw_char(j, i, lower_character);
            }
        }
    }

    for (i = text_mode_height() - lines; i < text_mode_height(); ++i) {
        for (j = 0; j < text_mode_width(); ++j) {
            text_mode_del_char(j, i);
        }
    }
}


void text_mode_clear_screen() {
    uint32 i, j;

    for (i = 0; i < text_mode_height(); ++i) {
	for (j = 0; j < text_mode_width(); ++j) {
	    text_mode_del_char(j, i);
	}
    }
}


void draw_line_segment(struct Vec2f v1, struct Vec2f v2) {
    // naive approach
    float delta_lambda = 1.0 / WIDTH, lambda;
    int i = 0;
    
    for (lambda = 0; lambda <= 1; lambda += delta_lambda) {
	    plot_pixel(lambda * (v2.x - v1.x) + v1.x + 0.5, lambda * (v2.y - v1.y) + v1.y + 0.5);
    }

    // DDA approach
    // float delta_lambda = 1.0 / WIDTH, lambda;
    // float cur_x = 0;
    // int i = 0;
    
    // for (lambda = 0; lambda <= 1; lambda += delta_lambda) {
	//     plot_pixel(lambda * (v2.x - v1.x) + v1.x + 0.5, lambda * (v2.y - v1.y) + v1.y + 0.5);
    // }
    
}

