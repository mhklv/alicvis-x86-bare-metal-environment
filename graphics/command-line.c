#include "command-line.h"
#include "drivers/display.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "misc/resource-manager.h"
#include "graphics/psf1-reader.h"
#include "misc/allocator.h"
#include "graphics/text-editor.h"
#include "misc/string.h"
#include "misc/io.h"


extern void switch_to_real_mode_and_shutdown();


static int16 cursor_x = 0, cursor_y = 0;
static struct PSF1File cmdl_font;
static uint8 cmdl_font_size = 2;
static uint8 bckgrnd_red = 0, bckgrnd_green = 0, bckgrnd_blue = 0;
static uint8 cursor_lightened = 1, cursor_blink_timer_id;
static uint8 cur_comm_start_line = 0;
static uint32 cur_comm_len = 0;

static uint16 *help_string =
    L"\nSupported commands:\n"
    "  help - display this help message;\n"
    "  edit <resource name> - text editor;\n"
    "  shutdown - shutdown the computer.";



static void draw_cursor();
static void erase_cursor();
static void process_input(struct KeyPressPacket key_press_packet);
static uint16 get_abs_coord_val(uint16 x, uint16 y);
static void shift_comm_left(uint16 start_x, uint16 start_y);
static void shift_comm_right(uint16 start_x, uint16 start_y);
static void insert_char(uint16 character);
static void process_command();


void start_command_line() {
    clear_screen_buffer();
    update_screen();

    uint32 time_to_refresh, sleep_time, refresh_interval = 33;
    uint8 refresh_timer_id;
    struct KeyPressPacket key_press_packet;
    uint8* psf_buffer = malloc(resource_size("font2.psf"));
    uint16 character;
    uint8 r, g, b;

    load_resource("font2.psf", psf_buffer);

    read_psf1(psf_buffer, &cmdl_font);
    
    set_font(&cmdl_font, cmdl_font_size);

    set_pixel_col(0, 255, 0);

    // start_text_editor("test.txt");

    cursor_blink_timer_id = start_timer();

    putstr(L"Welcome to Alicvis!\nType 'help' to display avilable commands.\n");
    
    for (;;) {
        refresh_timer_id = start_timer();

        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();
            process_input(key_press_packet);
        }

        if (get_timer_val(cursor_blink_timer_id) > 600) {
            if (cursor_lightened) {
                erase_cursor();
            }
            else {
                draw_cursor();
            }

            update_screen();
        }

        time_to_refresh = stop_timer(refresh_timer_id);
        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;

        sleep(sleep_time);
    }

    free(psf_buffer);
}



void process_input(struct KeyPressPacket key_press_packet) {
    erase_cursor();
    
    switch (key_press_packet.keycode) {
        case PRESSED_KEYCODE_L_ARROW:
            if (cursor_x == 0 && cur_comm_len > text_mode_width() && cursor_y != cur_comm_start_line) {
                cursor_x = text_mode_width() - 1;
                --cursor_y;
            }
            else if (cursor_x != 0) {
                --cursor_x;
            }

            break;
        
        case PRESSED_KEYCODE_R_ARROW:
            if (get_abs_coord_val(cursor_x, cursor_y) <
                cur_comm_start_line * text_mode_width() + cur_comm_len) {
                    
                ++cursor_x;
            }

            break;

        default:
            break;
    }
    
    switch (key_press_packet.character) {
        case 0xA:       // Line Feed

            process_command();
            // cursor_x = 0;
            // cur_comm_start_line += cur_comm_len / text_mode_width() +
            //                        ((cur_comm_len % text_mode_width() == 0) ? 0 : 1) +
            //                        ((cur_comm_len == 0) ? 1 : 0);
            // cursor_y = cur_comm_start_line;
            cur_comm_len = 0;

            break;
        
        case 0x08:      // Backspace
            if (cur_comm_len > 0) {
                if (cursor_y == cur_comm_start_line && cursor_x == 0) {
                    break;
                }

                if (cursor_x == 0 && cursor_y != cur_comm_start_line) {
                    cursor_x = text_mode_width() - 1;
                    --cursor_y;
                }
                else {
                    --cursor_x;
                }

                text_mode_del_char(cursor_x, cursor_y);
                shift_comm_left(cursor_x, cursor_y);

                --cur_comm_len;
            }
            break;
        
        default:
            if (key_press_packet.character != 0) {
                insert_char(key_press_packet.character);
                ++cursor_x;                
                ++cur_comm_len;
            }
            break;
    }

    uint16 lines;

    if (cursor_x >= text_mode_width()) {
        cursor_x = 0;
        ++cursor_y;
    }

    

    if (cursor_y >= text_mode_height()) {
        // lines = cursor_y - text_mode_height() + 1;
        lines = 1;
        text_mode_scroll_down(lines);
        cursor_y -= lines;
        cur_comm_start_line -= lines;
        cursor_x = 0;
    }

    draw_cursor();

    update_screen();
}


void draw_cursor() {
    uint8 r, g, b;
    uint16 character = text_mode_get_char(cursor_x, cursor_y);

    text_mode_del_char(cursor_x, cursor_y);
    r = red(), g = green(), b = blue();
    set_pixel_col(255 - bckgrnd_red, 255 - bckgrnd_green, 255 - bckgrnd_blue);
    text_mode_draw_charplace_solid(cursor_x, cursor_y);

    if (character != 0) {
        set_pixel_col(bckgrnd_red, bckgrnd_green, bckgrnd_blue);
        text_mode_draw_char(cursor_x, cursor_y, character);
    }

    set_pixel_col(r, g, b);

    cursor_lightened = 1;

    stop_timer(cursor_blink_timer_id);
    cursor_blink_timer_id = start_timer();
}


void erase_cursor() {
    uint16 character = text_mode_get_char(cursor_x, cursor_y);

    text_mode_del_char(cursor_x, cursor_y);
    
    if (character != 0) {
        text_mode_draw_char(cursor_x, cursor_y, character);
    }

    cursor_lightened = 0;

    stop_timer(cursor_blink_timer_id);
    cursor_blink_timer_id = start_timer();
}


uint16 get_abs_coord_val(uint16 x, uint16 y) {
    return text_mode_width() * y + x; 
}


void insert_char(uint16 character) {
    shift_comm_right(cursor_x, cursor_y);
    text_mode_draw_char(cursor_x, cursor_y, character);
}


void shift_comm_left(uint16 start_x, uint16 start_y) {
    if (cur_comm_len == 0) {
        return;
    }
    
    uint32 i;
    uint16 x, y, ch;

    for (i = get_abs_coord_val(start_x, start_y);
         i < get_abs_coord_val(0, cur_comm_start_line) + cur_comm_len - 1;
         ++i) {

        y = (i + 1) / text_mode_width();
        x = (i + 1) % text_mode_width();
        ch = text_mode_get_char(x, y);

        y = i / text_mode_width();
        x = i % text_mode_width();
        text_mode_del_char(x, y);
        text_mode_draw_char(x, y, ch);
    }

    y = i / text_mode_width();
    x = i % text_mode_width();
    text_mode_del_char(x, y);
}


void shift_comm_right(uint16 start_x, uint16 start_y) {
    uint32 i;
    uint16 x, y, ch;

    for (i = get_abs_coord_val(0, cur_comm_start_line) + cur_comm_len;
         i > get_abs_coord_val(start_x, start_y);
         --i) {

        y = (i - 1) / text_mode_width();
        x = (i - 1) % text_mode_width();
        ch = text_mode_get_char(x, y);
        text_mode_del_char(x, y);

        y = i / text_mode_width();
        x = i % text_mode_width();
        text_mode_draw_char(x, y, ch);
    }
}


void putchar(uint16 character) {
    // insert_char(character);
    uint16 lines;

    if (character == 0xA) {
        ++cursor_y;
        cursor_x = 0;
    }
    else {
        text_mode_draw_char(cursor_x, cursor_y, character);
        ++cursor_x;
    }
    

    if (cursor_x == text_mode_width()) {
        cursor_x = 0;
        ++cursor_y;
    }

    if (cursor_y >= text_mode_height()) {
        // lines = cursor_y - text_mode_height() + 1;
        lines = 1;
        text_mode_scroll_down(lines);
        cursor_y -= lines;
        cur_comm_start_line -= lines;
        cursor_x = 0;
    }

    cur_comm_start_line = cursor_y;
}


void putstr(uint16* str) {
    uint32 i = 0;

    while (str[i] != '\0') {
        putchar(str[i]);
        ++i;
    }

    ++cur_comm_start_line;
    cursor_y = cur_comm_start_line;
    cursor_x = 0;
}


void process_command() {

    uint16 *cur_command = malloc(2 * cur_comm_len + 2);
    uint16 *param;
    uint32 i;
    uint16 col = 0, row = cur_comm_start_line;
    
    for (i = 0; i < cur_comm_len; ++i) {
        cur_command[i] = text_mode_get_char(col, row);
        ++col;
    }
    cur_command[i] = L'\0';

    for (i = 0; i < cur_comm_len; ++i) {
        if (cur_command[i] == L' ') {
            cur_command[i] = L'\0';
            param = cur_command + i + 1;
            break;
        }
    }

    if (cur_command[0] == L'\0') {
        putstr(L"");        
    }
    else if (wstrcmp(cur_command, L"help") == 0) {
        putstr(help_string);
    }
    else if (wstrcmp(cur_command, L"edit") == 0) {
        uint8 *ascii_res_name = malloc(128);
        uint16 *screen_dump_buffer = malloc(2 * text_mode_width() * text_mode_height());
        text_mode_dump_screen(screen_dump_buffer);
        
        i = 0;

        while (param[i] != L'\0') {
            ascii_res_name[i] = (uint8) param[i];
            ++i;
        }
        ascii_res_name[i] = 0;
        
        start_text_editor(ascii_res_name);
        
        text_mode_set_font_size(cmdl_font_size);
        text_mode_extract_screen_dump(screen_dump_buffer);
        
        free(ascii_res_name);
        free(screen_dump_buffer);
        putstr(L"");
    }
    else if (wstrcmp(cur_command, L"shutdown") == 0) {
        switch_to_real_mode_and_shutdown();
    }
    else {
        putstr(L"\nCommand not found");
    }
    
    free(cur_command);
}
