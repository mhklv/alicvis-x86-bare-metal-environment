#ifndef DISPLAY_H
#define DISPLAY_H

#include "..//types.h"
#include "graphics/bmp-reader.h"
#include "graphics/psf1-reader.h"




void init_video();
void plot_pixel(uint32 x, uint32 y);
void set_pixel_col(uint8 r, uint8 g, uint8 b);
void refresh_screen();
void update_screen();
void clear_screen_buffer();

uint8 red();
uint8 green();
uint8 blue();

uint32 screen_width();
uint32 screen_height();

uint32 text_mode_width();
uint32 text_mode_height();
void text_mode_draw_char(uint16 x, uint16 y, uint16 character);
void text_mode_draw_charplace_solid(uint16 x, uint16 y);
uint16 text_mode_get_char(uint16 x, uint16 y);
void text_mode_del_char(uint16 x, uint16 y);
void text_mode_clear_screen();
void text_mode_dump_screen(uint16 *dump_buffer);
void text_mode_extract_screen_dump(uint16 *dump_buffer);

void set_font(struct PSF1File* psf1_file, uint8 size);
void text_mode_set_font_size(uint8 size);
uint8 text_mode_get_font_size();

void text_mode_scroll_down(uint8 lines);



struct Rectangle {
    uint32 x, y, width, height;
    uint8 active;
};



struct Vec2f {
    float x, y;
};



void draw_rect(struct Rectangle* rect);
void move_rect(struct Rectangle* rect, int32 delta_x,
               int32 delta_y, int32 delta_width, int32 delta_height,
               uint8 r, uint8 g, uint8 b);

void draw_bmp(uint16 x, uint16 y, struct BMPImage* bmp_image);
void bmp_change_brightness(struct BMPImage* bmp_image, float mult);
void draw_cursor_bmp(uint16 x, uint16 y, struct BMPImage* bmp_image);

void draw_char(uint16 x, uint16 y, uint16 character);

void draw_line_segment(struct Vec2f v1, struct Vec2f v2);



#endif // DISPLAY_H
