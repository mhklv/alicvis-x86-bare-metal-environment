#include "types.h"
#include "misc/io.h"
#include "drivers/display.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "graphics/rectangle.h"
#include "misc/allocator.h"
#include "boot/interrupts.h"
#include "drivers/disk.h"
#include "misc/resource-manager.h"
#include "graphics/bmp-reader.h"
#include "graphics/psf1-reader.h"
#include "graphics/command-line.h"
#include "misc/string.h"


void rect_screensaver();
void rect_controllable();
void show_some_image();
void show_image_controllable();
void test_fonts();
void text_input_test();
void start_splashscreen();
void start_cursor_test();
void start_line_test();


void main() {
    init_allocator((void*) 0x01010000, 0x10000000, 128);
    init_keyboard();
    init_timer();
    init_video();
    
    if (!init_disk()) {
        return;
    }

    if (!init_resource_manager()) {
        return;
    }

    start_splashscreen();
    start_command_line();
}


// Some functions for trying things out.

void start_line_test() {
    struct Vec2f v1, v2;
    v1.x = 0, v1.y = 0;
    v2.x = 100, v2.y = 50;

    uint8 r = 0, g = 0, b = 0;

    uint32 i = 1, time_to_refresh, sleep_time, refresh_interval = 25;
    uint8 timer_id;
    int8 xd = 0, yd = 0, wd = 0, hd = 0;
    struct KeyPressPacket key_press_packet;

    set_pixel_col(255, 0, 0);

    for (;;) {
        timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();

            switch (key_press_packet.keycode) {
                case PRESSED_KEYCODE_A:
                    xd += -14;
                    break;
                
                case RELEASED_KEYCODE_A:
                    xd += 14;
                    break;

                case PRESSED_KEYCODE_W:
                    yd += -14;
                    break;

                case RELEASED_KEYCODE_W:
                    yd += 14;
                    break;
                
                case PRESSED_KEYCODE_D:
                    xd += 14;
		    
                    break;

                case RELEASED_KEYCODE_D:
                    xd += -14;
                    break;
                
                case PRESSED_KEYCODE_S:
                    yd += 14;
                    break;
                
                case RELEASED_KEYCODE_S:
                    yd += -14;
                    break;
                
                default:
                    break;
            }
        }

        v2.x += xd;
        v2.y += yd;

        refresh_screen();

        draw_line_segment(v1, v2);
	
        time_to_refresh = stop_timer(timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;
        ++i;

	

        sleep(sleep_time);
    }
}


void start_splashscreen() {
    uint32 n = 32, i;
    uint8** image_buffers = malloc(n * sizeof(uint8*));
    struct BMPImage* anim_images = malloc(n * sizeof(struct BMPImage));
    uint8 res_name_buffer[32] = "ring-an-";
    uint8 ok = 1;

    for (i = 0; i < n; ++i) {
        itostr(res_name_buffer + 8, i + 1);
        image_buffers[i] = malloc(resource_size(res_name_buffer));
        load_resource(res_name_buffer, image_buffers[i]);
        read_bmp(image_buffers[i], &(anim_images[i]));
        // anim_images[i].scale = 1;
    }

    uint8* alicvis_label_buf = malloc(resource_size("alicvis-label.bmp"));
    struct BMPImage* alicvis_label_bmp = malloc(sizeof(struct BMPImage));
    uint8* fse_label_buf = malloc(resource_size("fse-label.bmp"));
    struct BMPImage* fse_label_bmp = malloc(sizeof(struct BMPImage));

    load_resource("alicvis-label.bmp", alicvis_label_buf);
    read_bmp(alicvis_label_buf, alicvis_label_bmp);

    load_resource("fse-label.bmp", fse_label_buf);
    read_bmp(fse_label_buf, fse_label_bmp);

    uint32 row, col;
    uint8 r = 0, g = 0, b = 0;

    i = 0;
    uint32 time_to_refresh, sleep_time, refresh_interval = 45;
    uint8 screen_update_timer_id, finish_timer_id;
    uint8 done = 0;
    int8 xd = 0, yd = 0, wd = 0, hd = 0;
    uint32 anim_x = (screen_width() - anim_images[0].bi_width) / 2;
    uint32 anim_y = (4 * screen_height() - 3 * anim_images[0].bi_height) / 6;
    uint32 alicvis_label_x = (screen_width() - alicvis_label_bmp->bi_width) / 2;
    // uint32 alicvis_label_x = 150;
    // uint32 alicvis_label_y = (screen_height() - alicvis_label_bmp->bi_height) / 2;
    uint32 alicvis_label_y = 100;
    uint32 fse_label_x = (screen_width() - fse_label_bmp->bi_width) / 2;
    uint32 fse_label_y = 200;
    struct KeyPressPacket key_press_packet;

    flush_key_queue();

    // anim_x = 120;
    // anim_y = 20;

    finish_timer_id = start_timer();

    for (;;) {
        screen_update_timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();
        }

        if (get_timer_val(finish_timer_id) >= 2500) {
            stop_timer(finish_timer_id);
            stop_timer(screen_update_timer_id);
            break;
        }

        refresh_screen();

        draw_bmp(alicvis_label_x, alicvis_label_y, alicvis_label_bmp);
        draw_bmp(fse_label_x, fse_label_y, fse_label_bmp);
        draw_bmp(anim_x, anim_y, &(anim_images[i]));

        time_to_refresh = stop_timer(screen_update_timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;

        if (i >= n - 1) {
            i = 0;
        }
        else {
            ++i;
        }

        sleep(sleep_time);
    }

    for (i = 0; i < n; ++i) {
        free(image_buffers[i]);
    }

    free(image_buffers);
    free(anim_images);
    free(alicvis_label_buf);
    free(fse_label_buf);
}


void text_input_test() {
    uint32 time_to_refresh, sleep_time, refresh_interval = 25;
    uint8 timer_id;
    uint32 x = 0, y = 0;
    struct KeyPressPacket key_press_packet;

    uint8* psf_buffer = malloc(resource_size("font2.psf"));
    struct PSF1File psf1_font;
    uint8 font_size = 8;

    load_resource("font2.psf", psf_buffer);

    read_psf1(psf_buffer, &psf1_font);
    
    set_font(&psf1_font, font_size);

    set_pixel_col(0, 255, 0);

    for (;;) {
        timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();

            if (key_press_packet.character != 0) {
                text_mode_draw_char(x, y, key_press_packet.character);

                ++x;

                if (x >= text_mode_width()) {
                    x = 0;
                    ++y;
                }

                if (y >= 4) {
                    text_mode_scroll_down(2);
                    --y;
                }

                update_screen();
            }
        }

        time_to_refresh = stop_timer(timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;

        sleep(sleep_time);
    }
}


void test_fonts() {
    uint8* psf_buffer = malloc(resource_size("font2.psf"));
    struct PSF1File psf1_font;
    uint8 font_size = 5;
    set_pixel_col(0, 255, 0);

    load_resource("font2.psf", psf_buffer);

    read_psf1(psf_buffer, &psf1_font);
    
    set_font(&psf1_font, font_size);


    // draw_char(0, 0, 0x410);
    uint16* str = L"asd123ПрИвЕт:D)))HELLO456789abc aaaaa";
    uint32 i, j, cnt = 0;
    uint16 x = 0, y = 0;
    
    // for (j = 0; j < 4; ++j) {
    //     x = 0;
    //     for (i = 0; i < 64; ++i) {
    //         draw_char(x, y, cnt++);
    //         x += 8 * font_size;
    //     }
    //     y += psf1_font.charsize * font_size;
    // }

    x = 0;
    for (i = 0; i < 11; ++i) {
        draw_char(x, psf1_font.charsize, str[i]);
        x += 8 * font_size;
    }
    
    refresh_screen();
}


void start_cursor_test() {
    uint8* image_buffer = malloc(resource_size("cursor1.bmp"));
    struct BMPImage cursor_image;

    load_resource("cursor1.bmp", image_buffer);
    read_bmp(image_buffer, &cursor_image);

    uint8* img2_buffer = malloc(resource_size("cat.bmp"));
    struct BMPImage cat_image;

    load_resource("cat.bmp", img2_buffer);
    read_bmp(img2_buffer, &cat_image);

    uint32 row, col;
    uint8 r = 0, g = 0, b = 0;


    uint32 i = 1, time_to_refresh, sleep_time, refresh_interval = 25;
    uint8 timer_id, cursor_entered_image = 0, cursor_left_image = 0, cursor_is_inside_image = 0;
    int8 xd = 0, yd = 0, wd = 0, hd = 0;
    int32 cursor_x = 0, cursor_y = 0;
    uint16 cat_image_x = 100, cat_image_y = 100;
    struct KeyPressPacket key_press_packet;

    for (;;) {
        timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();

            switch (key_press_packet.keycode) {
                case PRESSED_KEYCODE_L_ARROW:
                    xd += -14;
                    break;
                
                case RELEASED_KEYCODE_L_ARROW:
                    xd += 14;
                    break;

                case PRESSED_KEYCODE_U_ARROW:
                    yd += -14;
                    break;

                case RELEASED_KEYCODE_U_ARROW:
                    yd += 14;
                    break;
                
                case PRESSED_KEYCODE_R_ARROW:
                    xd += 14;
                    break;

                case RELEASED_KEYCODE_R_ARROW:
                    xd += -14;
                    break;
                
                case PRESSED_KEYCODE_D_ARROW:
                    yd += 14;
                    break;
                
                case RELEASED_KEYCODE_D_ARROW:
                    yd += -14;
                    break;

                case PRESSED_KEYCODE_ENTER:
                    if (cursor_x >= cat_image_x && cursor_x <= cat_image_x + cat_image.bi_width &&
                        cursor_y >= cat_image_y && cursor_y <= cat_image_y + cat_image.bi_height) {
                            
                        // rect_screensaver();
                        start_line_test();
                    }
                    break;
                
                default:
                    break;
            }
        }

        cursor_x += xd;
        cursor_y += yd;

        refresh_screen();

        if (cursor_x < 0) {
            cursor_x = 0;
        }

        if (cursor_x > screen_width()) {
            cursor_x = screen_width() - 2;
        }

        if (cursor_y < 0) {
            cursor_y = 0;
        }

        if (cursor_y > screen_height()) {
            cursor_y = screen_height() - 2;
        }

        if (cursor_x >= cat_image_x && cursor_x <= cat_image_x + cat_image.bi_width &&
            cursor_y >= cat_image_y && cursor_y <= cat_image_y + cat_image.bi_height) {
            
            if (!cursor_is_inside_image) {
                cursor_entered_image = 1;
            }

            cursor_is_inside_image = 1;
        }
        else if (cursor_is_inside_image) {
            cursor_left_image = 1;
            cursor_is_inside_image = 0;
        }

        if (cursor_entered_image) {
            bmp_change_brightness(&cat_image, 1.5);
            cursor_entered_image = 0;
        }

        if (cursor_left_image) {
            bmp_change_brightness(&cat_image, 0.66667);
            cursor_left_image = 0;
        }


        draw_bmp(cat_image_x, cat_image_y, &cat_image);

        draw_cursor_bmp(cursor_x, cursor_y, &cursor_image);

        time_to_refresh = stop_timer(timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;
        ++i;

        sleep(sleep_time);
    }
}

void show_image_controllable() {
    uint8* image_buffer = malloc(resource_size("fireball.bmp"));
    struct BMPImage bmp_image;

    load_resource("fireball.bmp", image_buffer);
    read_bmp(image_buffer, &bmp_image);

    uint32 row, col;
    uint8 r = 0, g = 0, b = 0;


    uint32 i = 1, time_to_refresh, sleep_time, refresh_interval = 25;
    uint8 timer_id;
    int8 xd = 0, yd = 0, wd = 0, hd = 0;
    uint32 image_x = 0, image_y = 0;
    struct KeyPressPacket key_press_packet;

    for (;;) {
        timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();

            switch (key_press_packet.keycode) {
                case PRESSED_KEYCODE_A:
                    xd += -14;
                    break;
                
                case RELEASED_KEYCODE_A:
                    xd += 14;
                    break;

                case PRESSED_KEYCODE_W:
                    yd += -14;
                    break;

                case RELEASED_KEYCODE_W:
                    yd += 14;
                    break;
                
                case PRESSED_KEYCODE_D:
                    xd += 14;
                    break;

                case RELEASED_KEYCODE_D:
                    xd += -14;
                    break;
                
                case PRESSED_KEYCODE_S:
                    yd += 14;
                    break;
                
                case RELEASED_KEYCODE_S:
                    yd += -14;
                    break;
                
                default:
                    break;
            }
        }

        image_x += xd;
        image_y += yd;

        refresh_screen();

        draw_bmp(image_x, image_y, &bmp_image);

        time_to_refresh = stop_timer(timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;
        ++i;

        sleep(sleep_time);
    }
}


void show_some_image() {
    uint8* buffer = malloc(resource_size("cat.bmp"));
    load_resource("cat.bmp", buffer);
    uint32 row, col;
    uint8 r, g, b;


    for (row = 0; row < 340; ++row) {
        for (col = 0; col < 283; ++col) {
            b = *(buffer + 0x36 + row*(283*3 + 3) + col*3);
            g = *(buffer + 0x36 + row*(283*3 + 3) + col*3 + 1);
            r = *(buffer + 0x36 + row*(283*3 + 3) + col*3 + 2);

            set_pixel_col(r, g, b);
            
            plot_pixel(col, 340 - row);
        }
    }
    refresh_screen();
}


void rect_screensaver() {
    struct Rectangle rect1;
    rect1.active = 1;
    rect1.x = 0;
    rect1.y = 200;
    rect1.height = 100;
    rect1.width = 100;

    draw_rect(&rect1);

    uint32 i = 1, time_to_refresh, sleep_time, refresh_interval = 25;
    uint8 r = 100, g = 200, b = 0;
    uint8 timer_id;
    int8 xd = 10, yd = 10, wd = 5, hd = 5;

    for (;;) {
        timer_id = start_timer();

        if (rect1.x <= 0) {
            xd = 10;
        }

        if (rect1.x + rect1.width >= screen_width() - 20) {
            xd = -10;
        }

        if (rect1.y <= 0) {
            yd = 10;
        }

        if (rect1.y + rect1.height >= screen_height() - 20) {
            yd = -10;
        }

        if (i % 50 == 0) {
            wd = -wd;
            hd = -hd;
        }

        refresh_screen();
        move_rect(&rect1, xd, yd, wd, hd, r, g, b);
        ++r, ++g, ++b;

        time_to_refresh = stop_timer(timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;
        ++i;

        sleep(sleep_time);
    }
}


void rect_controllable() {
    struct Rectangle rect1;
    rect1.active = 1;
    rect1.x = 0;
    rect1.y = 200;
    rect1.height = 100;
    rect1.width = 100;

    draw_rect(&rect1);
    

    uint32 i = 1, time_to_refresh, sleep_time, refresh_interval = 33;
    uint8 r = 200, g = 0, b = 0;
    uint8 timer_id;
    int8 xd = 0, yd = 0, wd = 0, hd = 0;
    struct KeyPressPacket key_press_packet;

    for (;;) {
        timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();

            if (key_press_packet.character == '+') {
                rect1.height += 20;
            }

            switch (key_press_packet.keycode) {
                case PRESSED_KEYCODE_A:
                    xd = -14;
                    break;
                
                case RELEASED_KEYCODE_A:
                    xd = 0;
                    break;

                case PRESSED_KEYCODE_W:
                    yd = -14;
                    break;

                case RELEASED_KEYCODE_W:
                    yd = 0;
                    break;
                
                case PRESSED_KEYCODE_D:
                    xd = 14;
                    break;

                case RELEASED_KEYCODE_D:
                    xd = 0;
                    break;
                
                case PRESSED_KEYCODE_S:
                    yd = 14;
                    break;
                
                case RELEASED_KEYCODE_S:
                    yd = 0;
                    break;
                
                case PRESSED_KEYCODE_PGUP:
                    wd = 8;
                    hd = 8;
                    break;
                
                case RELEASED_KEYCODE_PGUP:
                    wd = 0;
                    hd = 0;
                    break;

                case PRESSED_KEYCODE_PGDN:
                    wd = -8;
                    hd = -8;
                    break;
                
                case RELEASED_KEYCODE_PGDN:
                    wd = 0;
                    hd = 0;
                    break;
                
                default:
                    break;
            }
        }

        if (rect1.width <= 16 && wd < 0) {
            wd = 0;
        }
        if (rect1.height <= 16 && hd < 0) {
            hd = 0;
        }
        rect1.x += xd;
        rect1.y += yd;
        rect1.height += hd;
        rect1.width += wd;

        // move_rect(&rect1, xd, yd, wd, hd, r, g, b);
        refresh_screen();
        draw_rect(&rect1);
        
        // ++r, ++g, ++b;

        time_to_refresh = stop_timer(timer_id);

        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;
        ++i;

        sleep(sleep_time);
    }
}
