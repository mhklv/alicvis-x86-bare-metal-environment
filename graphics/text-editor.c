#include "text-editor.h"
#include "graphics/command-line.h"
#include "misc/string.h"
#include "types.h"
#include "drivers/display.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "misc/resource-manager.h"
#include "graphics/psf1-reader.h"
#include "misc/allocator.h"



static void redraw_buffer_screen_region();
static void draw_text_area();
static void draw_message_line();
static void working_loop();
static void process_input(struct KeyPressPacket key_press_packet);
static void close_editor();

static void cursor_right();
static void cursor_left();
static void cursor_down();
static void cursor_up();
static void cursor_to_start_of_line();
static void cursor_to_end_of_line();
static void cursor_insert_char(uint16 character);
static void cursor_del_char();
static void cursor_insert_tab();

static uint32 get_last_pos_on_screen();
static uint32 get_logical_line_end(uint32 pos);
static uint32 get_logical_line_start(uint32 pos);
static uint32 get_logical_col(uint32 pos);
static uint32 get_logical_line_len(uint32 pos);

static void insert_char(uint32 pos, uint16 character);
static void erase_char(uint32 pos);

static void increase_font_size();
static void decrease_font_size();

static void save_file();

static uint32 edit_area_height();
static uint32 edit_area_width();

static void set_current_message(uint8 *new_message);
static uint32 mes_line_cursor_pos_str_gen(uint8 *res_str);


static uint8 *text_buffer, *opened_file_name, *message;
static uint8 *tmp_buf;
static uint8 need_screen_update, font_size, initial_font_size, tab_size = 4, exit_planned;
static uint32 cursor_pos, screen_start_pos, buffer_size, active_buffer_size;
static uint32 cursor_max_col;
static uint32 cursor_current_line_num;

static uint8 *welcome_message = "Weclome!";



void start_text_editor(uint8 *file_name) {
    uint8 result;

    if (file_name[0] == 0) {
        putstr(L"\nInvalid resource name");
        return;
    }
    
    buffer_size = 1 * 1024 * 1024;
    text_buffer = malloc(buffer_size);        
    
    if (resource_exists(file_name)) {
        result = load_resource(file_name, text_buffer);

        if (!result) {
            free(text_buffer);
            return;
        }

        active_buffer_size = resource_size(file_name);
    }
    else {
        text_buffer[0] = 0xA;
        active_buffer_size = 1;
    }
    
    uint32  i;

    // if (res_size == 0) {
    //     return;
    // }
    
    // active_buffer_size = res_size;
    cursor_pos = 0;
    cursor_max_col = 0;
    screen_start_pos = 0;
    need_screen_update = 1;
    font_size = 2;
    initial_font_size = text_mode_get_font_size();
    cursor_current_line_num = 0;
    exit_planned = 0;
    opened_file_name = file_name;
    tmp_buf = malloc(256);
    message = malloc(256);

    for (i = 0; i < 256; ++i) {
        message[i] = 0;
    }

    strcpy(welcome_message, message, 64);

    text_mode_set_font_size(font_size);

    working_loop();

    free(text_buffer);
    free(message);
    free(tmp_buf);
}


void close_editor() {
    text_mode_clear_screen();
    exit_planned = 1;
    text_mode_set_font_size(initial_font_size);
}


void working_loop() {
    struct KeyPressPacket key_press_packet;
    uint32 time_to_refresh, sleep_time, refresh_interval = 33;
    uint8 refresh_timer_id;
    
    for(;;) {
        refresh_timer_id = start_timer();
        
        while (!key_queue_empty()) {
            key_press_packet = dequeue_press_packet();
            process_input(key_press_packet);
        }

        if (exit_planned) {
            stop_timer(refresh_timer_id);
            break;
        }
        
        if (need_screen_update) {
            redraw_buffer_screen_region();
            update_screen();
            need_screen_update = 0;
        }

        time_to_refresh = stop_timer(refresh_timer_id);
        sleep_time = (time_to_refresh > refresh_interval) ? 0 : refresh_interval - time_to_refresh;

        sleep(sleep_time);      
    }
}


void draw_text_area() {
    uint32 screen_x = 0, screen_y = 0, i = screen_start_pos;
    uint8 cur_char, r, g, b;
    r = red(), g = green(), b = blue();

    while (i < active_buffer_size && screen_y < edit_area_height()) {
        cur_char = text_buffer[i];

        // draw cursor
        if (i == cursor_pos) {
            set_pixel_col(255, 255, 255);
            text_mode_draw_charplace_solid(screen_x, screen_y);
            
            set_pixel_col(0, 0, 0);
        }


        switch (cur_char) {
            case 0xA: {         // Line Feed
                ++screen_y;
                screen_x = 0;
                
                break;
            }
                
            default:
                text_mode_draw_char(screen_x, screen_y, cur_char);
                ++screen_x;
                break;
        }
        
        set_pixel_col(r, g, b);

        if (screen_x > text_mode_width() - 1) {
            ++screen_y;
            screen_x = 0;
        }

        ++i;
    }
}


void draw_message_line() {
    uint32 screen_x = 0, screen_y = 0, i = screen_start_pos;
    uint8 cur_char, r, g, b;
    r = red(), g = green(), b = blue();
    
    // draw background
    set_pixel_col(160, 160, 160);
    screen_y = text_mode_height() - 1;
    
    for (screen_x = 0; screen_x < text_mode_width(); ++screen_x) {
        text_mode_draw_charplace_solid(screen_x, screen_y);
    }

    
    // draw message (if it is available)
    set_pixel_col(0, 0, 250);
    i = 0, screen_x = 1;
    
    while(message[i] != 0 && screen_x < text_mode_width()) {
        text_mode_draw_char(screen_x, screen_y, message[i]);
        ++i, ++screen_x;
    }

    message[0] = 0;

    
    // draw cursor positition coordinates
    uint32 pos_descriptor_len = mes_line_cursor_pos_str_gen(tmp_buf);
    i = 0, screen_x = text_mode_width() - pos_descriptor_len - 1;

    while(tmp_buf[i] != 0 && screen_x < text_mode_width()) {
        text_mode_draw_char(screen_x, screen_y, tmp_buf[i]);
        ++i, ++screen_x;
    }


    uint32 file_name_len = strlen(opened_file_name);    

    if (text_mode_width() > file_name_len + pos_descriptor_len + 2) {
        // draw filename
        i = 0, screen_x = text_mode_width() - pos_descriptor_len - file_name_len - 3;
        
        while(opened_file_name[i] != 0 && screen_x < text_mode_width()) {
            text_mode_draw_char(screen_x, screen_y, opened_file_name[i]);
            ++i, ++screen_x;
        }
    }

    set_pixel_col(r, g, b);
}


void redraw_buffer_screen_region() {
    text_mode_clear_screen();

    draw_text_area();

    draw_message_line();
}


void process_input(struct KeyPressPacket key_press_packet) {
    if (!key_press_packet.ctrl_pressed && key_press_packet.character != 0) {
        switch (key_press_packet.character) {
            case 0x8: {         // Backspace
                cursor_del_char();                
                break;
            }

            case 0x9: {         // Horizontal Tab
                cursor_insert_tab();
                break;
            }
                
            default:
                cursor_insert_char(key_press_packet.character);
                break;
        }
    }
    else if (!key_press_packet.ctrl_pressed) {
        switch (key_press_packet.keycode) {
            case PRESSED_KEYCODE_R_ARROW: {
                cursor_right();
                break;
            }

            case PRESSED_KEYCODE_D_ARROW: {
                cursor_down();
                break;
            }

            case PRESSED_KEYCODE_U_ARROW: {
                cursor_up();
                break;
            }

            case PRESSED_KEYCODE_L_ARROW: {
                cursor_left();
                break;
            }

            case PRESSED_KEYCODE_HOME: {
                cursor_to_start_of_line();
                break;
            }

            case PRESSED_KEYCODE_END: {
                cursor_to_end_of_line();
                break;
            }

            default:
                break;
        }
    }
    else {
        switch (key_press_packet.keycode) {
            case PRESSED_KEYCODE_MINUS: {
                decrease_font_size();
                break;
            }

            case PRESSED_KEYCODE_EUQALS: {
                increase_font_size();
                break;
            }

            case PRESSED_KEYCODE_S: {
                save_file();
                break;
            }

            case PRESSED_KEYCODE_X: {
                close_editor();
                break;
            }
                
            default:
                break;
        }
    }
}


uint32 edit_area_height() { return text_mode_height() - 1; }


uint32 edit_area_width() { return text_mode_width(); }


uint32 get_last_pos_on_screen() {
    uint32 screen_x = 0, screen_y = 0, i = screen_start_pos;
    uint8 cur_char;

    while (i < active_buffer_size && screen_y < edit_area_height()) {
        cur_char = text_buffer[i];


        switch (cur_char) {
            case 0xA: {         // Line Feed
                ++screen_y;
                screen_x = 0;
                
                break;
            }
                
            default:
                ++screen_x;
                break;
        }

        if (screen_x > text_mode_width() - 1) {
            ++screen_y;
            screen_x = 0;
        }

        ++i;
    }

    return i - 1;
}


uint32 get_logical_line_end(uint32 pos) {
    uint32 i = pos;

    while (1) {
        if (i >= active_buffer_size - 1 || text_buffer[i] == 0xA) {
            return i;
        }

        ++i;
    }
}


uint32 get_logical_line_start(uint32 pos) {
    uint32 i = pos;

    if (i == 0) {
        return i;
    }

    while (1) {
        --i;
        
        if (text_buffer[i] == 0xA) {
            return i + 1;
        }
        else if (i == 0) {
            return i;
        }
    }
}


uint32 get_logical_col(uint32 pos) {
    return pos - get_logical_line_start(pos);
}


uint32 get_logical_line_len(uint32 pos) {
    return get_logical_line_end(pos) - get_logical_line_start(pos) + 1;
}


void cursor_right() {
    if (cursor_pos == active_buffer_size - 1) {
        set_current_message("End of buffer");
        return;
    }

    need_screen_update = 1;

    if (text_buffer[cursor_pos] == 0xA) {
        ++cursor_current_line_num;
    }
    
    ++cursor_pos;

    cursor_max_col = get_logical_col(cursor_pos);

    if (cursor_pos > get_last_pos_on_screen()) {
        screen_start_pos = get_logical_line_end(screen_start_pos) + 1;
    }
}


void cursor_left() {
    if (cursor_pos == 0) {
        set_current_message("Beginning of buffer");
        return;
    }
    
    need_screen_update = 1;
    --cursor_pos;

    if (text_buffer[cursor_pos] == 0xA) {
        --cursor_current_line_num;
    }

    cursor_max_col = get_logical_col(cursor_pos);

    if (cursor_pos < screen_start_pos) {
        screen_start_pos = get_logical_line_start(screen_start_pos - 1);
    }
}


void cursor_down() {
    uint32 cur_line_end_pos = get_logical_line_end(cursor_pos);
    
    if (cur_line_end_pos == active_buffer_size - 1) {
        set_current_message("End of buffer");
        return;
    }
    
    need_screen_update = 1;

    ++cursor_current_line_num;
    
    uint32 old_cursor_pos = cursor_pos;
    uint32 next_line_len = get_logical_line_len(cur_line_end_pos + 1);
    uint32 next_line_last_col = next_line_len - 1;
    uint32 new_cursor_col = (next_line_last_col < cursor_max_col) ? next_line_last_col : cursor_max_col;

    cursor_pos = cur_line_end_pos + 1 + new_cursor_col;

    while(cursor_pos > get_last_pos_on_screen()) {
        screen_start_pos = get_logical_line_end(screen_start_pos) + 1;
    }
}


void cursor_up() {
    uint32 cur_line_start_pos = get_logical_line_start(cursor_pos);
    
    if (cur_line_start_pos == 0) {
        set_current_message("Beginning of buffer");
        return;
    }
    
    need_screen_update = 1;

    --cursor_current_line_num;
   
    uint32 prev_line_len = get_logical_line_len(cur_line_start_pos - 1);
    uint32 prev_line_last_col = prev_line_len - 1;
    uint32 new_cursor_col = (prev_line_last_col < cursor_max_col) ? prev_line_last_col : cursor_max_col;

    cursor_pos = get_logical_line_start(cur_line_start_pos - 1) + new_cursor_col;
    
    if (cursor_pos < screen_start_pos) {
        screen_start_pos = cur_line_start_pos - prev_line_len;
    }
}


void cursor_to_start_of_line() {
    need_screen_update = 1;
    cursor_pos = get_logical_line_start(cursor_pos);
    cursor_max_col = get_logical_col(cursor_pos);
}


void cursor_to_end_of_line() {
    need_screen_update = 1;
    cursor_pos = get_logical_line_end(cursor_pos);
    cursor_max_col = get_logical_col(cursor_pos);
}


void cursor_insert_char(uint16 character) {
    need_screen_update = 1;
    insert_char(cursor_pos, character);
    ++cursor_pos;

    if (character == 0xA) {
        ++cursor_current_line_num;
    }
    
    while(cursor_pos > get_last_pos_on_screen()) {
        screen_start_pos = get_logical_line_end(screen_start_pos) + 1;
    }
}


void cursor_del_char() {
    if (cursor_pos != 0) {
        need_screen_update = 1;

        if (text_buffer[cursor_pos - 1] == 0xA) {
            --cursor_current_line_num;
        }
        
        erase_char(--cursor_pos);

        if (cursor_pos < screen_start_pos) {
            screen_start_pos = get_logical_line_start(screen_start_pos - 1);
        }
    }
    else {
        set_current_message("Beginning of buffer");
    }
}


void cursor_insert_tab() {
    uint32 i;

    need_screen_update = 1;

    for (i = 0; i < tab_size; ++i) {
        cursor_insert_char(L' ');
    }
}


void insert_char(uint32 pos, uint16 character) {
    if (pos >= active_buffer_size) {
        return;
    }

    uint32 i;

    for (i = active_buffer_size; i-- > pos;) {
        text_buffer[i + 1] = text_buffer[i];        
    }    

    text_buffer[pos] = character;

    ++active_buffer_size;
}


void erase_char(uint32 pos) {
    if (pos >= active_buffer_size - 1) {
        return;
    }

    uint32 i;

    for (i = pos; i < active_buffer_size - 1; ++i) {
        text_buffer[i] = text_buffer[i + 1];
    }

    --active_buffer_size;
}


void increase_font_size() {
    if (font_size < 3) {
        need_screen_update = 1;
        ++font_size;
        text_mode_set_font_size(font_size);

        uint32 last_screen_pos = get_last_pos_on_screen();

        while(cursor_pos > last_screen_pos) {
            cursor_up();
        }
    }
}


void decrease_font_size() {
    if (font_size > 1) {
        need_screen_update = 1;
        --font_size;
        text_mode_set_font_size(font_size);
    }
}


void save_file() {
    need_screen_update = 1;
    
    delete_resource(opened_file_name);
    create_resource(opened_file_name, text_buffer, active_buffer_size);

    set_current_message("File saved");
}


void set_current_message(uint8 *new_message) {
    need_screen_update = 1;
    strcpy(new_message, message, 64);
}


uint32 mes_line_cursor_pos_str_gen(uint8 *res_str) {
    uint32 i;

    itostr(res_str, cursor_current_line_num + 1);

    for (i = 0; res_str[i] != 0; ++i)
        ;
    res_str[i++] = ',';
    res_str[i++] = ' ';

    itostr(res_str + i, get_logical_col(cursor_pos));

    for (i = 0; res_str[i] != 0; ++i);

    return i;
}
