#include "types.h"
#include "drivers/display.h"
#include "misc/io.h"
#include "misc/circqueue.h"
#include "keyboard.h"
#include "boot/interrupts.h"



static struct CircQueue8 keys_queue;
static uint8 FIRST_BYTE_SCANCODE_MAP[256];
static uint8 SECOND_BYTE_SCANCODE_MAP[256];
static uint8 FIRST_CHARACTER_MAP[128];
static uint8 SECOND_CHARACTER_MAP[128];
static uint16 CYR_FIRST_CHARACTER_MAP[128];
static uint16 CYR_SECOND_CHARACTER_MAP[128];


void init_keyboard() {
    init_circqueue(&keys_queue, 1024 * sizeof(struct KeyPressPacket));
    
    IRQ_clear_mask(1);
}


uint8 key_queue_empty() {
    return queue_is_empty(&keys_queue);
}


void flush_key_queue() {
    flush_circueue(&keys_queue);
}


uint8 dequeue_key() {
    return dequeue_key(&keys_queue);
}


struct KeyPressPacket dequeue_press_packet() {
    static uint8 is_shift_down = 0;
    static uint8 is_alt_down = 0;
    static uint8 is_alt_locked = 0;
    static uint8 is_ctrl_down = 0;
    static uint8 is_caps_locked = 0;
    
    struct KeyPressPacket key_press_packet;
    uint8 is_key_pressed;
    
    key_press_packet.keycode = dequeue(&keys_queue);

    switch (key_press_packet.keycode) {
        case PRESSED_KEYCODE_RCTRL:
        case PRESSED_KEYCODE_LCTRL:
            is_ctrl_down = 1;
            break;
        
        case PRESSED_KEYCODE_RALT:
        case PRESSED_KEYCODE_LALT:
            is_alt_down = 1;
            break;
        
        case PRESSED_KEYCODE_RSHFT:
        case PRESSED_KEYCODE_LSHFT:
            is_shift_down = 1;
            break;
        
        case RELEASED_KEYCODE_RCTRL:
        case RELEASED_KEYCODE_LCTRL:
            is_ctrl_down = 0;
            break;

        case RELEASED_KEYCODE_RALT:
        case RELEASED_KEYCODE_LALT:
            is_alt_down = 0;
            is_alt_locked = (is_alt_locked) ? 0 : 1;
            break;
        
        case RELEASED_KEYCODE_RSHFT:
        case RELEASED_KEYCODE_LSHFT:
            is_shift_down = 0;
            break;
        
        case PRESSED_KEYCODE_CAPS:
            is_caps_locked = (is_caps_locked) ? 0 : 1;
            break;
    }

    key_press_packet.alt_pressed = is_alt_down;
    key_press_packet.shift_pressed = is_shift_down;
    key_press_packet.ctrl_pressed = is_ctrl_down;
    key_press_packet.caps_locked = is_caps_locked;
    
    if (key_press_packet.keycode < 0x80) {
        if (is_alt_locked) {
            if (is_caps_locked ^ is_shift_down) {
                key_press_packet.character = CYR_SECOND_CHARACTER_MAP[key_press_packet.keycode];
            }
            else {
                key_press_packet.character = CYR_FIRST_CHARACTER_MAP[key_press_packet.keycode];
            }
        }
        else {
            if (is_caps_locked ^ is_shift_down) {
                key_press_packet.character = SECOND_CHARACTER_MAP[key_press_packet.keycode];
            }
            else {
                key_press_packet.character = FIRST_CHARACTER_MAP[key_press_packet.keycode];
            }
        }
    }
    else {
        key_press_packet.character = 0;
    }

    return key_press_packet;
}





__attribute__ ((interrupt))
void irq1_handler(struct interrupt_frame *frame) {
    static uint8 is_two_bytes_code = 0;
    static uint8 is_four_bytes_code = 0;
    static uint8 is_six_bytes_code = 0;
    
    uint8 status = inb(0x64);   // Read PS/2 Controller's status register
    
    if (status & 1) {       // Check if output buffer is not empty
        uint8 scan_code = inb(0x60);

        if (scan_code == 0xE1) {
            is_six_bytes_code = 1;
        }
        else if (scan_code == 0xE0) {
            if (!is_four_bytes_code) {
                is_two_bytes_code = 1;
            }
        }
        else {
            if (is_six_bytes_code) {
                if (scan_code == 0xC5) {
                    enqueue(&keys_queue, PRESSED_KEYCODE_PAUSE);
                    is_six_bytes_code = 0;
                }
            }
            else if (is_four_bytes_code) {
                if (scan_code == 0x37) {
                    enqueue(&keys_queue, PRESSED_KEYCODE_PRNTSCRN);
                }
                else {
                    enqueue(&keys_queue, RELEASED_KEYCODE_PRNTSCRN);
                }
                is_four_bytes_code = 0;
            }
            else if (is_two_bytes_code) {
                if (scan_code == 0x2A || scan_code == 0xB7) {
                    is_two_bytes_code = 0;
                    is_four_bytes_code = 1;
                }
                else {
                    enqueue(&keys_queue, SECOND_BYTE_SCANCODE_MAP[scan_code]);
                    is_two_bytes_code = 0;
                }
            }
            else {
                enqueue(&keys_queue, FIRST_BYTE_SCANCODE_MAP[scan_code]);
            }
        }
    }

    outb(PIC1_COMMAND, PIC_EOI);
}




static uint8 FIRST_BYTE_SCANCODE_MAP[256] = {
    [  0] = KEYCODE_INVALID,
    [  1] = PRESSED_KEYCODE_ESC,
    [  2] = PRESSED_KEYCODE_1,
    [  3] = PRESSED_KEYCODE_2,
    [  4] = PRESSED_KEYCODE_3,
    [  5] = PRESSED_KEYCODE_4,
    [  6] = PRESSED_KEYCODE_5,
    [  7] = PRESSED_KEYCODE_6,
    [  8] = PRESSED_KEYCODE_7,
    [  9] = PRESSED_KEYCODE_8,
    [ 10] = PRESSED_KEYCODE_9,
    [ 11] = PRESSED_KEYCODE_0,
    [ 12] = PRESSED_KEYCODE_MINUS,
    [ 13] = PRESSED_KEYCODE_EUQALS,
    [ 14] = PRESSED_KEYCODE_BKSP,
    [ 15] = PRESSED_KEYCODE_TAB,
    [ 16] = PRESSED_KEYCODE_Q,
    [ 17] = PRESSED_KEYCODE_W,
    [ 18] = PRESSED_KEYCODE_E,
    [ 19] = PRESSED_KEYCODE_R,
    [ 20] = PRESSED_KEYCODE_T,
    [ 21] = PRESSED_KEYCODE_Y,
    [ 22] = PRESSED_KEYCODE_U,
    [ 23] = PRESSED_KEYCODE_I,
    [ 24] = PRESSED_KEYCODE_O,
    [ 25] = PRESSED_KEYCODE_P,
    [ 26] = PRESSED_KEYCODE_L_SQUARE_BR,
    [ 27] = PRESSED_KEYCODE_R_SQUARE_BR,
    [ 28] = PRESSED_KEYCODE_ENTER,
    [ 29] = PRESSED_KEYCODE_LCTRL,
    [ 30] = PRESSED_KEYCODE_A,
    [ 31] = PRESSED_KEYCODE_S,
    [ 32] = PRESSED_KEYCODE_D,
    [ 33] = PRESSED_KEYCODE_F,
    [ 34] = PRESSED_KEYCODE_G,
    [ 35] = PRESSED_KEYCODE_H,
    [ 36] = PRESSED_KEYCODE_J,
    [ 37] = PRESSED_KEYCODE_K,
    [ 38] = PRESSED_KEYCODE_L,
    [ 39] = PRESSED_KEYCODE_SEMICOLON,
    [ 40] = PRESSED_KEYCODE_APOSTROPHE,
    [ 41] = PRESSED_KEYCODE_TICK,
    [ 42] = PRESSED_KEYCODE_LSHFT,
    [ 43] = PRESSED_KEYCODE_BACKSLASH,
    [ 44] = PRESSED_KEYCODE_Z,
    [ 45] = PRESSED_KEYCODE_X,
    [ 46] = PRESSED_KEYCODE_C,
    [ 47] = PRESSED_KEYCODE_V,
    [ 48] = PRESSED_KEYCODE_B,
    [ 49] = PRESSED_KEYCODE_N,
    [ 50] = PRESSED_KEYCODE_M,
    [ 51] = PRESSED_KEYCODE_COLON,
    [ 52] = PRESSED_KEYCODE_DOT,
    [ 53] = PRESSED_KEYCODE_SLASH,
    [ 54] = PRESSED_KEYCODE_RSHFT,
    [ 55] = PRESSED_KEYCODE_KP_STAR,
    [ 56] = PRESSED_KEYCODE_LALT,
    [ 57] = PRESSED_KEYCODE_SPACE,
    [ 58] = PRESSED_KEYCODE_CAPS,
    [ 59] = PRESSED_KEYCODE_F1,
    [ 60] = PRESSED_KEYCODE_F2,
    [ 61] = PRESSED_KEYCODE_F3,
    [ 62] = PRESSED_KEYCODE_F4,
    [ 63] = PRESSED_KEYCODE_F5,
    [ 64] = PRESSED_KEYCODE_F6,
    [ 65] = PRESSED_KEYCODE_F7,
    [ 66] = PRESSED_KEYCODE_F8,
    [ 67] = PRESSED_KEYCODE_F9,
    [ 68] = PRESSED_KEYCODE_F10,
    [ 69] = PRESSED_KEYCODE_NUM,
    [ 70] = PRESSED_KEYCODE_SCROLL,
    [ 71] = PRESSED_KEYCODE_KP7,
    [ 72] = PRESSED_KEYCODE_KP8,
    [ 73] = PRESSED_KEYCODE_KP9,
    [ 74] = PRESSED_KEYCODE_KP_MINUS,
    [ 75] = PRESSED_KEYCODE_KP4,
    [ 76] = PRESSED_KEYCODE_KP5,
    [ 77] = PRESSED_KEYCODE_KP6,
    [ 78] = PRESSED_KEYCODE_KP_PLUS,
    [ 79] = PRESSED_KEYCODE_KP1,
    [ 80] = PRESSED_KEYCODE_KP2,
    [ 81] = PRESSED_KEYCODE_KP3,
    [ 82] = PRESSED_KEYCODE_KP0,
    [ 83] = PRESSED_KEYCODE_KP_DOT,
    [ 84] = KEYCODE_INVALID,
    [ 85] = KEYCODE_INVALID,
    [ 86] = KEYCODE_INVALID,
    [ 87] = PRESSED_KEYCODE_F11,
    [ 88] = PRESSED_KEYCODE_F12,
    [ 89] = KEYCODE_INVALID,
    [ 90] = KEYCODE_INVALID,
    [ 91] = KEYCODE_INVALID,
    [ 92] = KEYCODE_INVALID,
    [ 93] = KEYCODE_INVALID,
    [ 94] = KEYCODE_INVALID,
    [ 95] = KEYCODE_INVALID,
    [ 96] = KEYCODE_INVALID,
    [ 97] = KEYCODE_INVALID,
    [ 98] = KEYCODE_INVALID,
    [ 99] = KEYCODE_INVALID,
    [100] = KEYCODE_INVALID,
    [101] = KEYCODE_INVALID,
    [102] = KEYCODE_INVALID,
    [103] = KEYCODE_INVALID,
    [104] = KEYCODE_INVALID,
    [105] = KEYCODE_INVALID,
    [106] = KEYCODE_INVALID,
    [107] = KEYCODE_INVALID,
    [108] = KEYCODE_INVALID,
    [109] = KEYCODE_INVALID,
    [110] = KEYCODE_INVALID,
    [111] = KEYCODE_INVALID,
    [112] = KEYCODE_INVALID,
    [113] = KEYCODE_INVALID,
    [114] = KEYCODE_INVALID,
    [115] = KEYCODE_INVALID,
    [116] = KEYCODE_INVALID,
    [117] = KEYCODE_INVALID,
    [118] = KEYCODE_INVALID,
    [119] = KEYCODE_INVALID,
    [120] = KEYCODE_INVALID,
    [121] = KEYCODE_INVALID,
    [122] = KEYCODE_INVALID,
    [123] = KEYCODE_INVALID,
    [124] = KEYCODE_INVALID,
    [125] = KEYCODE_INVALID,
    [126] = KEYCODE_INVALID,
    [127] = KEYCODE_INVALID,
    [128] = KEYCODE_INVALID,
    [129] = RELEASED_KEYCODE_ESC,
    [130] = RELEASED_KEYCODE_1,
    [131] = RELEASED_KEYCODE_2,
    [132] = RELEASED_KEYCODE_3,
    [133] = RELEASED_KEYCODE_4,
    [134] = RELEASED_KEYCODE_5,
    [135] = RELEASED_KEYCODE_6,
    [136] = RELEASED_KEYCODE_7,
    [137] = RELEASED_KEYCODE_8,
    [138] = RELEASED_KEYCODE_9,
    [139] = RELEASED_KEYCODE_0,
    [140] = RELEASED_KEYCODE_MINUS,
    [141] = RELEASED_KEYCODE_EUQALS,
    [142] = RELEASED_KEYCODE_BKSP,
    [143] = RELEASED_KEYCODE_TAB,
    [144] = RELEASED_KEYCODE_Q,
    [145] = RELEASED_KEYCODE_W,
    [146] = RELEASED_KEYCODE_E,
    [147] = RELEASED_KEYCODE_R,
    [148] = RELEASED_KEYCODE_T,
    [149] = RELEASED_KEYCODE_Y,
    [150] = RELEASED_KEYCODE_U,
    [151] = RELEASED_KEYCODE_I,
    [152] = RELEASED_KEYCODE_O,
    [153] = RELEASED_KEYCODE_P,
    [154] = RELEASED_KEYCODE_L_SQUARE_BR,
    [155] = RELEASED_KEYCODE_R_SQUARE_BR,
    [156] = RELEASED_KEYCODE_ENTER,
    [157] = RELEASED_KEYCODE_LCTRL,
    [158] = RELEASED_KEYCODE_A,
    [159] = RELEASED_KEYCODE_S,
    [160] = RELEASED_KEYCODE_D,
    [161] = RELEASED_KEYCODE_F,
    [162] = RELEASED_KEYCODE_G,
    [163] = RELEASED_KEYCODE_H,
    [164] = RELEASED_KEYCODE_J,
    [165] = RELEASED_KEYCODE_K,
    [166] = RELEASED_KEYCODE_L,
    [167] = RELEASED_KEYCODE_SEMICOLON,
    [168] = RELEASED_KEYCODE_APOSTROPHE,
    [169] = RELEASED_KEYCODE_TICK,
    [170] = RELEASED_KEYCODE_LSHFT,
    [171] = RELEASED_KEYCODE_BACKSLASH,
    [172] = RELEASED_KEYCODE_Z,
    [173] = RELEASED_KEYCODE_X,
    [174] = RELEASED_KEYCODE_C,
    [175] = RELEASED_KEYCODE_V,
    [176] = RELEASED_KEYCODE_B,
    [177] = RELEASED_KEYCODE_N,
    [178] = RELEASED_KEYCODE_M,
    [179] = RELEASED_KEYCODE_COLON,
    [180] = RELEASED_KEYCODE_DOT,
    [181] = RELEASED_KEYCODE_SLASH,
    [182] = RELEASED_KEYCODE_RSHFT,
    [183] = RELEASED_KEYCODE_KP_STAR,
    [184] = RELEASED_KEYCODE_LALT,
    [185] = RELEASED_KEYCODE_SPACE,
    [186] = RELEASED_KEYCODE_CAPS,
    [187] = RELEASED_KEYCODE_F1,
    [188] = RELEASED_KEYCODE_F2,
    [189] = RELEASED_KEYCODE_F3,
    [190] = RELEASED_KEYCODE_F4,
    [191] = RELEASED_KEYCODE_F5,
    [192] = RELEASED_KEYCODE_F6,
    [193] = RELEASED_KEYCODE_F7,
    [194] = RELEASED_KEYCODE_F8,
    [195] = RELEASED_KEYCODE_F9,
    [196] = RELEASED_KEYCODE_F10,
    [197] = RELEASED_KEYCODE_NUM,
    [198] = RELEASED_KEYCODE_SCROLL,
    [199] = RELEASED_KEYCODE_KP7,
    [200] = RELEASED_KEYCODE_KP8,
    [201] = RELEASED_KEYCODE_KP9,
    [202] = RELEASED_KEYCODE_KP_MINUS,
    [203] = RELEASED_KEYCODE_KP4,
    [204] = RELEASED_KEYCODE_KP5,
    [205] = RELEASED_KEYCODE_KP6,
    [206] = RELEASED_KEYCODE_KP_PLUS,
    [207] = RELEASED_KEYCODE_KP1,
    [208] = RELEASED_KEYCODE_KP2,
    [209] = RELEASED_KEYCODE_KP3,
    [210] = RELEASED_KEYCODE_KP0,
    [211] = RELEASED_KEYCODE_KP_DOT,
    [212] = KEYCODE_INVALID,
    [213] = KEYCODE_INVALID,
    [214] = KEYCODE_INVALID,
    [215] = RELEASED_KEYCODE_F11,
    [216] = RELEASED_KEYCODE_F12,
    [217] = KEYCODE_INVALID,
    [218] = KEYCODE_INVALID,
    [219] = KEYCODE_INVALID,
    [220] = KEYCODE_INVALID,
    [221] = KEYCODE_INVALID,
    [222] = KEYCODE_INVALID,
    [223] = KEYCODE_INVALID,
    [224] = KEYCODE_INVALID,
    [225] = KEYCODE_INVALID,
    [226] = KEYCODE_INVALID,
    [227] = KEYCODE_INVALID,
    [228] = KEYCODE_INVALID,
    [229] = KEYCODE_INVALID,
    [230] = KEYCODE_INVALID,
    [231] = KEYCODE_INVALID,
    [232] = KEYCODE_INVALID,
    [233] = KEYCODE_INVALID,
    [234] = KEYCODE_INVALID,
    [235] = KEYCODE_INVALID,
    [236] = KEYCODE_INVALID,
    [237] = KEYCODE_INVALID,
    [238] = KEYCODE_INVALID,
    [239] = KEYCODE_INVALID,
    [240] = KEYCODE_INVALID,
    [241] = KEYCODE_INVALID,
    [242] = KEYCODE_INVALID,
    [243] = KEYCODE_INVALID,
    [244] = KEYCODE_INVALID,
    [245] = KEYCODE_INVALID,
    [246] = KEYCODE_INVALID,
    [247] = KEYCODE_INVALID,
    [248] = KEYCODE_INVALID,
    [249] = KEYCODE_INVALID,
    [250] = KEYCODE_INVALID,
    [251] = KEYCODE_INVALID,
    [252] = KEYCODE_INVALID,
    [253] = KEYCODE_INVALID,
    [254] = KEYCODE_INVALID,
    [255] = KEYCODE_INVALID
};


static uint8 SECOND_BYTE_SCANCODE_MAP[256] = {
    [  0] = KEYCODE_INVALID,
    [  1] = KEYCODE_INVALID,
    [  2] = KEYCODE_INVALID,
    [  3] = KEYCODE_INVALID,
    [  4] = KEYCODE_INVALID,
    [  5] = KEYCODE_INVALID,
    [  6] = KEYCODE_INVALID,
    [  7] = KEYCODE_INVALID,
    [  8] = KEYCODE_INVALID,
    [  9] = KEYCODE_INVALID,
    [ 10] = KEYCODE_INVALID,
    [ 11] = KEYCODE_INVALID,
    [ 12] = KEYCODE_INVALID,
    [ 13] = KEYCODE_INVALID,
    [ 14] = KEYCODE_INVALID,
    [ 15] = KEYCODE_INVALID,
    [ 16] = KEYCODE_INVALID,
    [ 17] = KEYCODE_INVALID,
    [ 18] = KEYCODE_INVALID,
    [ 19] = KEYCODE_INVALID,
    [ 20] = KEYCODE_INVALID,
    [ 21] = KEYCODE_INVALID,
    [ 22] = KEYCODE_INVALID,
    [ 23] = KEYCODE_INVALID,
    [ 24] = KEYCODE_INVALID,
    [ 25] = KEYCODE_INVALID,
    [ 26] = KEYCODE_INVALID,
    [ 27] = KEYCODE_INVALID,
    [ 28] = PRESSED_KEYCODE_KPEN,
    [ 29] = PRESSED_KEYCODE_RCTRL,
    [ 30] = KEYCODE_INVALID,
    [ 31] = KEYCODE_INVALID,
    [ 32] = KEYCODE_INVALID,
    [ 33] = KEYCODE_INVALID,
    [ 34] = KEYCODE_INVALID,
    [ 35] = KEYCODE_INVALID,
    [ 36] = KEYCODE_INVALID,
    [ 37] = KEYCODE_INVALID,
    [ 38] = KEYCODE_INVALID,
    [ 39] = KEYCODE_INVALID,
    [ 40] = KEYCODE_INVALID,
    [ 41] = KEYCODE_INVALID,
    [ 42] = KEYCODE_INVALID,
    [ 43] = KEYCODE_INVALID,
    [ 44] = KEYCODE_INVALID,
    [ 45] = KEYCODE_INVALID,
    [ 46] = KEYCODE_INVALID,
    [ 47] = KEYCODE_INVALID,
    [ 48] = KEYCODE_INVALID,
    [ 49] = KEYCODE_INVALID,
    [ 50] = KEYCODE_INVALID,
    [ 51] = KEYCODE_INVALID,
    [ 52] = KEYCODE_INVALID,
    [ 53] = PRESSED_KEYCODE_KP_SLASH,
    [ 54] = KEYCODE_INVALID,
    [ 55] = KEYCODE_INVALID,
    [ 56] = PRESSED_KEYCODE_RALT,
    [ 57] = KEYCODE_INVALID,
    [ 58] = KEYCODE_INVALID,
    [ 59] = KEYCODE_INVALID,
    [ 60] = KEYCODE_INVALID,
    [ 61] = KEYCODE_INVALID,
    [ 62] = KEYCODE_INVALID,
    [ 63] = KEYCODE_INVALID,
    [ 64] = KEYCODE_INVALID,
    [ 65] = KEYCODE_INVALID,
    [ 66] = KEYCODE_INVALID,
    [ 67] = KEYCODE_INVALID,
    [ 68] = KEYCODE_INVALID,
    [ 69] = KEYCODE_INVALID,
    [ 70] = KEYCODE_INVALID,
    [ 71] = PRESSED_KEYCODE_HOME,
    [ 72] = PRESSED_KEYCODE_U_ARROW,
    [ 73] = PRESSED_KEYCODE_PGUP,
    [ 74] = KEYCODE_INVALID,
    [ 75] = PRESSED_KEYCODE_L_ARROW,
    [ 76] = KEYCODE_INVALID,
    [ 77] = PRESSED_KEYCODE_R_ARROW,
    [ 78] = KEYCODE_INVALID,
    [ 79] = PRESSED_KEYCODE_END,
    [ 80] = PRESSED_KEYCODE_D_ARROW,
    [ 81] = PRESSED_KEYCODE_PGDN,
    [ 82] = PRESSED_KEYCODE_INSERT,
    [ 83] = PRESSED_KEYCODE_DELETE,
    [ 84] = KEYCODE_INVALID,
    [ 85] = KEYCODE_INVALID,
    [ 86] = KEYCODE_INVALID,
    [ 87] = KEYCODE_INVALID,
    [ 88] = KEYCODE_INVALID,
    [ 89] = KEYCODE_INVALID,
    [ 90] = KEYCODE_INVALID,
    [ 91] = PRESSED_KEYCODE_LGUI,
    [ 92] = PRESSED_KEYCODE_RGUI,
    [ 93] = PRESSED_KEYCODE_APPS,
    [ 94] = KEYCODE_INVALID,
    [ 95] = KEYCODE_INVALID,
    [ 96] = KEYCODE_INVALID,
    [ 97] = KEYCODE_INVALID,
    [ 98] = KEYCODE_INVALID,
    [ 99] = KEYCODE_INVALID,
    [100] = KEYCODE_INVALID,
    [101] = KEYCODE_INVALID,
    [102] = KEYCODE_INVALID,
    [103] = KEYCODE_INVALID,
    [104] = KEYCODE_INVALID,
    [105] = KEYCODE_INVALID,
    [106] = KEYCODE_INVALID,
    [107] = KEYCODE_INVALID,
    [108] = KEYCODE_INVALID,
    [109] = KEYCODE_INVALID,
    [110] = KEYCODE_INVALID,
    [111] = KEYCODE_INVALID,
    [112] = KEYCODE_INVALID,
    [113] = KEYCODE_INVALID,
    [114] = KEYCODE_INVALID,
    [115] = KEYCODE_INVALID,
    [116] = KEYCODE_INVALID,
    [117] = KEYCODE_INVALID,
    [118] = KEYCODE_INVALID,
    [119] = KEYCODE_INVALID,
    [120] = KEYCODE_INVALID,
    [121] = KEYCODE_INVALID,
    [122] = KEYCODE_INVALID,
    [123] = KEYCODE_INVALID,
    [124] = KEYCODE_INVALID,
    [125] = KEYCODE_INVALID,
    [126] = KEYCODE_INVALID,
    [127] = KEYCODE_INVALID,
    [128] = KEYCODE_INVALID,
    [129] = KEYCODE_INVALID,
    [130] = KEYCODE_INVALID,
    [131] = KEYCODE_INVALID,
    [132] = KEYCODE_INVALID,
    [133] = KEYCODE_INVALID,
    [134] = KEYCODE_INVALID,
    [135] = KEYCODE_INVALID,
    [136] = KEYCODE_INVALID,
    [137] = KEYCODE_INVALID,
    [138] = KEYCODE_INVALID,
    [139] = KEYCODE_INVALID,
    [140] = KEYCODE_INVALID,
    [141] = KEYCODE_INVALID,
    [142] = KEYCODE_INVALID,
    [143] = KEYCODE_INVALID,
    [144] = KEYCODE_INVALID,
    [145] = KEYCODE_INVALID,
    [146] = KEYCODE_INVALID,
    [147] = KEYCODE_INVALID,
    [148] = KEYCODE_INVALID,
    [149] = KEYCODE_INVALID,
    [150] = KEYCODE_INVALID,
    [151] = KEYCODE_INVALID,
    [152] = KEYCODE_INVALID,
    [153] = KEYCODE_INVALID,
    [154] = KEYCODE_INVALID,
    [155] = KEYCODE_INVALID,
    [156] = RELEASED_KEYCODE_KPEN,
    [157] = RELEASED_KEYCODE_RCTRL,
    [158] = KEYCODE_INVALID,
    [159] = KEYCODE_INVALID,
    [160] = KEYCODE_INVALID,
    [161] = KEYCODE_INVALID,
    [162] = KEYCODE_INVALID,
    [163] = KEYCODE_INVALID,
    [164] = KEYCODE_INVALID,
    [165] = KEYCODE_INVALID,
    [166] = KEYCODE_INVALID,
    [167] = KEYCODE_INVALID,
    [168] = KEYCODE_INVALID,
    [169] = KEYCODE_INVALID,
    [170] = KEYCODE_INVALID,
    [171] = KEYCODE_INVALID,
    [172] = KEYCODE_INVALID,
    [173] = KEYCODE_INVALID,
    [174] = KEYCODE_INVALID,
    [175] = KEYCODE_INVALID,
    [176] = KEYCODE_INVALID,
    [177] = KEYCODE_INVALID,
    [178] = KEYCODE_INVALID,
    [179] = KEYCODE_INVALID,
    [180] = KEYCODE_INVALID,
    [181] = RELEASED_KEYCODE_KP_SLASH,
    [182] = KEYCODE_INVALID,
    [183] = KEYCODE_INVALID,
    [184] = RELEASED_KEYCODE_RALT,
    [185] = KEYCODE_INVALID,
    [186] = KEYCODE_INVALID,
    [187] = KEYCODE_INVALID,
    [188] = KEYCODE_INVALID,
    [189] = KEYCODE_INVALID,
    [190] = KEYCODE_INVALID,
    [191] = KEYCODE_INVALID,
    [192] = KEYCODE_INVALID,
    [193] = KEYCODE_INVALID,
    [194] = KEYCODE_INVALID,
    [195] = KEYCODE_INVALID,
    [196] = KEYCODE_INVALID,
    [197] = KEYCODE_INVALID,
    [198] = KEYCODE_INVALID,
    [199] = RELEASED_KEYCODE_HOME,
    [200] = RELEASED_KEYCODE_U_ARROW,
    [201] = RELEASED_KEYCODE_PGUP,
    [202] = KEYCODE_INVALID,
    [203] = RELEASED_KEYCODE_L_ARROW,
    [204] = KEYCODE_INVALID,
    [205] = RELEASED_KEYCODE_R_ARROW,
    [206] = KEYCODE_INVALID,
    [207] = RELEASED_KEYCODE_END,
    [208] = RELEASED_KEYCODE_D_ARROW,
    [209] = RELEASED_KEYCODE_PGDN,
    [210] = RELEASED_KEYCODE_INSERT,
    [211] = RELEASED_KEYCODE_DELETE,
    [212] = KEYCODE_INVALID,
    [213] = KEYCODE_INVALID,
    [214] = KEYCODE_INVALID,
    [215] = KEYCODE_INVALID,
    [216] = KEYCODE_INVALID,
    [217] = KEYCODE_INVALID,
    [218] = KEYCODE_INVALID,
    [219] = RELEASED_KEYCODE_LGUI,
    [220] = RELEASED_KEYCODE_RGUI,
    [221] = RELEASED_KEYCODE_APPS,
    [222] = KEYCODE_INVALID,
    [223] = KEYCODE_INVALID,
    [224] = KEYCODE_INVALID,
    [225] = KEYCODE_INVALID,
    [226] = KEYCODE_INVALID,
    [227] = KEYCODE_INVALID,
    [228] = KEYCODE_INVALID,
    [229] = KEYCODE_INVALID,
    [230] = KEYCODE_INVALID,
    [231] = KEYCODE_INVALID,
    [232] = KEYCODE_INVALID,
    [233] = KEYCODE_INVALID,
    [234] = KEYCODE_INVALID,
    [235] = KEYCODE_INVALID,
    [236] = KEYCODE_INVALID,
    [237] = KEYCODE_INVALID,
    [238] = KEYCODE_INVALID,
    [239] = KEYCODE_INVALID,
    [240] = KEYCODE_INVALID,
    [241] = KEYCODE_INVALID,
    [242] = KEYCODE_INVALID,
    [243] = KEYCODE_INVALID,
    [244] = KEYCODE_INVALID,
    [245] = KEYCODE_INVALID,
    [246] = KEYCODE_INVALID,
    [247] = KEYCODE_INVALID,
    [248] = KEYCODE_INVALID,
    [249] = KEYCODE_INVALID,
    [250] = KEYCODE_INVALID,
    [251] = KEYCODE_INVALID,
    [252] = KEYCODE_INVALID,
    [253] = KEYCODE_INVALID,
    [254] = KEYCODE_INVALID,
    [255] = KEYCODE_INVALID
};


static uint8 FIRST_CHARACTER_MAP[128] = {
    [PRESSED_KEYCODE_A] = 0x61, // a
    [PRESSED_KEYCODE_B] = 0x62, // b
    [PRESSED_KEYCODE_C] = 0x63, // c
    [PRESSED_KEYCODE_D] = 0x64, // d
    [PRESSED_KEYCODE_E] = 0x65, // e
    [PRESSED_KEYCODE_F] = 0x66, // f
    [PRESSED_KEYCODE_G] = 0x67, // g
    [PRESSED_KEYCODE_H] = 0x68, // h
    [PRESSED_KEYCODE_I] = 0x69, // i
    [PRESSED_KEYCODE_J] = 0x6A, // j
    [PRESSED_KEYCODE_K] = 0x6B, // k
    [PRESSED_KEYCODE_L] = 0x6C, // l
    [PRESSED_KEYCODE_M] = 0x6D, // m
    [PRESSED_KEYCODE_N] = 0x6E, // n
    [PRESSED_KEYCODE_O] = 0x6F, // o
    [PRESSED_KEYCODE_P] = 0x70, // p
    [PRESSED_KEYCODE_Q] = 0x71, // q
    [PRESSED_KEYCODE_R] = 0x72, // r
    [PRESSED_KEYCODE_S] = 0x73, // s
    [PRESSED_KEYCODE_T] = 0x74, // t
    [PRESSED_KEYCODE_U] = 0x75, // u
    [PRESSED_KEYCODE_V] = 0x76, // v
    [PRESSED_KEYCODE_W] = 0x77, // w
    [PRESSED_KEYCODE_X] = 0x78, // x
    [PRESSED_KEYCODE_Y] = 0x79, // y
    [PRESSED_KEYCODE_Z] = 0x7A, // z
    [PRESSED_KEYCODE_0] = 0x30, // 0
    [PRESSED_KEYCODE_1] = 0x31, // 1
    [PRESSED_KEYCODE_2] = 0x32, // 2
    [PRESSED_KEYCODE_3] = 0x33, // 3
    [PRESSED_KEYCODE_4] = 0x34, // 4
    [PRESSED_KEYCODE_5] = 0x35, // 5
    [PRESSED_KEYCODE_6] = 0x36, // 6
    [PRESSED_KEYCODE_7] = 0x37, // 7
    [PRESSED_KEYCODE_8] = 0x38, // 8
    [PRESSED_KEYCODE_9] = 0x39, // 9
    [PRESSED_KEYCODE_TICK] = 0x60, // `
    [PRESSED_KEYCODE_MINUS] = 0x2D, // -
    [PRESSED_KEYCODE_EUQALS] = 0x3D, // =
    [PRESSED_KEYCODE_BACKSLASH] = 0x5C, // backslash
    [PRESSED_KEYCODE_BKSP] = 0x8, // 
    [PRESSED_KEYCODE_SPACE] = 0x20, //  
    [PRESSED_KEYCODE_TAB] = 0x9, // 
    [PRESSED_KEYCODE_CAPS] = 0x0, // 
    [PRESSED_KEYCODE_LSHFT] = 0x0, // 
    [PRESSED_KEYCODE_LCTRL] = 0x0, // 
    [PRESSED_KEYCODE_LGUI] = 0x0, // 
    [PRESSED_KEYCODE_LALT] = 0x0, // 
    [PRESSED_KEYCODE_RSHFT] = 0x0, // 
    [PRESSED_KEYCODE_RCTRL] = 0x0, // 
    [PRESSED_KEYCODE_RGUI] = 0x0, // 
    [PRESSED_KEYCODE_RALT] = 0x0, // 
    [PRESSED_KEYCODE_APPS] = 0x0, // 
    [PRESSED_KEYCODE_ENTER] = 0xA, // newline
    [PRESSED_KEYCODE_ESC] = 0x1B, // escape
    [PRESSED_KEYCODE_F1] = 0x0, // 
    [PRESSED_KEYCODE_F2] = 0x0, // 
    [PRESSED_KEYCODE_F3] = 0x0, // 
    [PRESSED_KEYCODE_F4] = 0x0, // 
    [PRESSED_KEYCODE_F5] = 0x0, // 
    [PRESSED_KEYCODE_F6] = 0x0, // 
    [PRESSED_KEYCODE_F7] = 0x0, // 
    [PRESSED_KEYCODE_F8] = 0x0, // 
    [PRESSED_KEYCODE_F9] = 0x0, // 
    [PRESSED_KEYCODE_F10] = 0x0, // 
    [PRESSED_KEYCODE_F11] = 0x0, // 
    [PRESSED_KEYCODE_F12] = 0x0, // 
    [PRESSED_KEYCODE_PRNTSCRN] = 0x0, // 
    [PRESSED_KEYCODE_SCROLL] = 0x0, // 
    [PRESSED_KEYCODE_PAUSE] = 0x0, // 
    [PRESSED_KEYCODE_L_SQUARE_BR] = 0x5B, // [
    [PRESSED_KEYCODE_INSERT] = 0x0, // 
    [PRESSED_KEYCODE_HOME] = 0x0, // 
    [PRESSED_KEYCODE_PGUP] = 0x0, // 
    [PRESSED_KEYCODE_DELETE] = 0x7F, // del
    [PRESSED_KEYCODE_END] = 0x0, // 
    [PRESSED_KEYCODE_PGDN] = 0x0, // 
    [PRESSED_KEYCODE_U_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_L_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_D_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_R_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_NUM] = 0x0, // 
    [PRESSED_KEYCODE_KP_SLASH] = 0x2F, // /
    [PRESSED_KEYCODE_KP_STAR] = 0x2A, // *
    [PRESSED_KEYCODE_KP_MINUS] = 0x2D, // -
    [PRESSED_KEYCODE_KP_PLUS] = 0x2B, // +
    [PRESSED_KEYCODE_KPEN] = 0xA, // newline
    [PRESSED_KEYCODE_KP_DOT] = 0x2E, // .
    [PRESSED_KEYCODE_KP0] = 0x0, // 
    [PRESSED_KEYCODE_KP1] = 0x0, // 
    [PRESSED_KEYCODE_KP2] = 0x0, // 
    [PRESSED_KEYCODE_KP3] = 0x0, // 
    [PRESSED_KEYCODE_KP4] = 0x0, // 
    [PRESSED_KEYCODE_KP5] = 0x0, // 
    [PRESSED_KEYCODE_KP6] = 0x0, // 
    [PRESSED_KEYCODE_KP7] = 0x0, // 
    [PRESSED_KEYCODE_KP8] = 0x0, // 
    [PRESSED_KEYCODE_KP9] = 0x0, // 
    [PRESSED_KEYCODE_R_SQUARE_BR] = 0x5D, // ]
    [PRESSED_KEYCODE_SEMICOLON] = 0x3B, // ;
    [PRESSED_KEYCODE_APOSTROPHE] = 0x27, // '
    [PRESSED_KEYCODE_COLON] = 0x2C, // ,
    [PRESSED_KEYCODE_DOT] = 0x2E, // .
    [PRESSED_KEYCODE_SLASH] = 0x2F, // /
    [104] = 0x0, // 
    [105] = 0x0, // 
    [106] = 0x0, // 
    [107] = 0x0, // 
    [108] = 0x0, // 
    [109] = 0x0, // 
    [110] = 0x0, // 
    [111] = 0x0, // 
    [112] = 0x0, // 
    [113] = 0x0, // 
    [114] = 0x0, // 
    [115] = 0x0, // 
    [116] = 0x0, // 
    [117] = 0x0, // 
    [118] = 0x0, // 
    [119] = 0x0, // 
    [120] = 0x0, // 
    [121] = 0x0, // 
    [122] = 0x0, // 
    [123] = 0x0, // 
    [124] = 0x0, // 
    [125] = 0x0, // 
    [126] = 0x0, // 
    [127] = 0x0, // 
};


static uint8 SECOND_CHARACTER_MAP[128] = {
    [PRESSED_KEYCODE_A] = 0x41, // A
    [PRESSED_KEYCODE_B] = 0x42, // B
    [PRESSED_KEYCODE_C] = 0x43, // C
    [PRESSED_KEYCODE_D] = 0x44, // D
    [PRESSED_KEYCODE_E] = 0x45, // E
    [PRESSED_KEYCODE_F] = 0x46, // F
    [PRESSED_KEYCODE_G] = 0x47, // G
    [PRESSED_KEYCODE_H] = 0x48, // H
    [PRESSED_KEYCODE_I] = 0x49, // I
    [PRESSED_KEYCODE_J] = 0x4A, // J
    [PRESSED_KEYCODE_K] = 0x4B, // K
    [PRESSED_KEYCODE_L] = 0x4C, // L
    [PRESSED_KEYCODE_M] = 0x4D, // M
    [PRESSED_KEYCODE_N] = 0x4E, // N
    [PRESSED_KEYCODE_O] = 0x4F, // O
    [PRESSED_KEYCODE_P] = 0x50, // P
    [PRESSED_KEYCODE_Q] = 0x51, // Q
    [PRESSED_KEYCODE_R] = 0x52, // R
    [PRESSED_KEYCODE_S] = 0x53, // S
    [PRESSED_KEYCODE_T] = 0x54, // T
    [PRESSED_KEYCODE_U] = 0x55, // U
    [PRESSED_KEYCODE_V] = 0x56, // V
    [PRESSED_KEYCODE_W] = 0x57, // W
    [PRESSED_KEYCODE_X] = 0x58, // X
    [PRESSED_KEYCODE_Y] = 0x59, // Y
    [PRESSED_KEYCODE_Z] = 0x5A, // Z
    [PRESSED_KEYCODE_0] = 0x29, // )
    [PRESSED_KEYCODE_1] = 0x21, // !
    [PRESSED_KEYCODE_2] = 0x40, // @
    [PRESSED_KEYCODE_3] = 0x23, // #
    [PRESSED_KEYCODE_4] = 0x24, // $
    [PRESSED_KEYCODE_5] = 0x25, // %
    [PRESSED_KEYCODE_6] = 0x5E, // ^
    [PRESSED_KEYCODE_7] = 0x26, // &
    [PRESSED_KEYCODE_8] = 0x2A, // *
    [PRESSED_KEYCODE_9] = 0x28, // (
    [PRESSED_KEYCODE_TICK] = 0x7E, // ~
    [PRESSED_KEYCODE_MINUS] = 0x5F, // _
    [PRESSED_KEYCODE_EUQALS] = 0x2B, // +
    [PRESSED_KEYCODE_BACKSLASH] = 0x7C, // |
    [PRESSED_KEYCODE_BKSP] = 0x8, // 
    [PRESSED_KEYCODE_SPACE] = 0x20, //  
    [PRESSED_KEYCODE_TAB] = 0x9, // 
    [PRESSED_KEYCODE_CAPS] = 0x0, // 
    [PRESSED_KEYCODE_LSHFT] = 0x0, // 
    [PRESSED_KEYCODE_LCTRL] = 0x0, // 
    [PRESSED_KEYCODE_LGUI] = 0x0, // 
    [PRESSED_KEYCODE_LALT] = 0x0, // 
    [PRESSED_KEYCODE_RSHFT] = 0x0, // 
    [PRESSED_KEYCODE_RCTRL] = 0x0, // 
    [PRESSED_KEYCODE_RGUI] = 0x0, // 
    [PRESSED_KEYCODE_RALT] = 0x0, // 
    [PRESSED_KEYCODE_APPS] = 0x0, // 
    [PRESSED_KEYCODE_ENTER] = 0xA, // newline
    [PRESSED_KEYCODE_ESC] = 0x1B, // escape
    [PRESSED_KEYCODE_F1] = 0x0, // 
    [PRESSED_KEYCODE_F2] = 0x0, // 
    [PRESSED_KEYCODE_F3] = 0x0, // 
    [PRESSED_KEYCODE_F4] = 0x0, // 
    [PRESSED_KEYCODE_F5] = 0x0, // 
    [PRESSED_KEYCODE_F6] = 0x0, // 
    [PRESSED_KEYCODE_F7] = 0x0, // 
    [PRESSED_KEYCODE_F8] = 0x0, // 
    [PRESSED_KEYCODE_F9] = 0x0, // 
    [PRESSED_KEYCODE_F10] = 0x0, // 
    [PRESSED_KEYCODE_F11] = 0x0, // 
    [PRESSED_KEYCODE_F12] = 0x0, // 
    [PRESSED_KEYCODE_PRNTSCRN] = 0x0, // 
    [PRESSED_KEYCODE_SCROLL] = 0x0, // 
    [PRESSED_KEYCODE_PAUSE] = 0x0, // 
    [PRESSED_KEYCODE_L_SQUARE_BR] = 0x7B, // {
    [PRESSED_KEYCODE_INSERT] = 0x0, // 
    [PRESSED_KEYCODE_HOME] = 0x0, // 
    [PRESSED_KEYCODE_PGUP] = 0x0, // 
    [PRESSED_KEYCODE_DELETE] = 0x7F, // del
    [PRESSED_KEYCODE_END] = 0x0, // 
    [PRESSED_KEYCODE_PGDN] = 0x0, // 
    [PRESSED_KEYCODE_U_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_L_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_D_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_R_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_NUM] = 0x0, // 
    [PRESSED_KEYCODE_KP_SLASH] = 0x2F, // /
    [PRESSED_KEYCODE_KP_STAR] = 0x2A, // *
    [PRESSED_KEYCODE_KP_MINUS] = 0x2D, // -
    [PRESSED_KEYCODE_KP_PLUS] = 0x2B, // +
    [PRESSED_KEYCODE_KPEN] = 0xA, // newline
    [PRESSED_KEYCODE_KP_DOT] = 0x2E, // .
    [PRESSED_KEYCODE_KP0] = 0x0, // 
    [PRESSED_KEYCODE_KP1] = 0x0, // 
    [PRESSED_KEYCODE_KP2] = 0x0, // 
    [PRESSED_KEYCODE_KP3] = 0x0, // 
    [PRESSED_KEYCODE_KP4] = 0x0, // 
    [PRESSED_KEYCODE_KP5] = 0x0, // 
    [PRESSED_KEYCODE_KP6] = 0x0, // 
    [PRESSED_KEYCODE_KP7] = 0x0, // 
    [PRESSED_KEYCODE_KP8] = 0x0, // 
    [PRESSED_KEYCODE_KP9] = 0x0, // 
    [PRESSED_KEYCODE_R_SQUARE_BR] = 0x7D, // }
    [PRESSED_KEYCODE_SEMICOLON] = 0x3A, // :
    [PRESSED_KEYCODE_APOSTROPHE] = 0x22, // "
    [PRESSED_KEYCODE_COLON] = 0x3C, // <
    [PRESSED_KEYCODE_DOT] = 0x3E, // >
    [PRESSED_KEYCODE_SLASH] = 0x3F, // ?
    [104] = 0x0, // 
    [105] = 0x0, // 
    [106] = 0x0, // 
    [107] = 0x0, // 
    [108] = 0x0, // 
    [109] = 0x0, // 
    [110] = 0x0, // 
    [111] = 0x0, // 
    [112] = 0x0, // 
    [113] = 0x0, // 
    [114] = 0x0, // 
    [115] = 0x0, // 
    [116] = 0x0, // 
    [117] = 0x0, // 
    [118] = 0x0, // 
    [119] = 0x0, // 
    [120] = 0x0, // 
    [121] = 0x0, // 
    [122] = 0x0, // 
    [123] = 0x0, // 
    [124] = 0x0, // 
    [125] = 0x0, // 
    [126] = 0x0, // 
    [127] = 0x0, // 
};


static uint16 CYR_FIRST_CHARACTER_MAP[128] = {
    [PRESSED_KEYCODE_A] = L'ф',
    [PRESSED_KEYCODE_B] = L'и',
    [PRESSED_KEYCODE_C] = L'с',
    [PRESSED_KEYCODE_D] = L'в',
    [PRESSED_KEYCODE_E] = L'у',
    [PRESSED_KEYCODE_F] = L'а',
    [PRESSED_KEYCODE_G] = L'п',
    [PRESSED_KEYCODE_H] = L'р',
    [PRESSED_KEYCODE_I] = L'ш',
    [PRESSED_KEYCODE_J] = L'о',
    [PRESSED_KEYCODE_K] = L'л',
    [PRESSED_KEYCODE_L] = L'д',
    [PRESSED_KEYCODE_M] = L'ь',
    [PRESSED_KEYCODE_N] = L'т',
    [PRESSED_KEYCODE_O] = L'щ',
    [PRESSED_KEYCODE_P] = L'з',
    [PRESSED_KEYCODE_Q] = L'й',
    [PRESSED_KEYCODE_R] = L'к',
    [PRESSED_KEYCODE_S] = L'ы',
    [PRESSED_KEYCODE_T] = L'е',
    [PRESSED_KEYCODE_U] = L'г',
    [PRESSED_KEYCODE_V] = L'м',
    [PRESSED_KEYCODE_W] = L'ц',
    [PRESSED_KEYCODE_X] = L'ч',
    [PRESSED_KEYCODE_Y] = L'н',
    [PRESSED_KEYCODE_Z] = L'я',
    [PRESSED_KEYCODE_0] = L'0',
    [PRESSED_KEYCODE_1] = L'1',
    [PRESSED_KEYCODE_2] = L'2',
    [PRESSED_KEYCODE_3] = L'3',
    [PRESSED_KEYCODE_4] = L'4',
    [PRESSED_KEYCODE_5] = L'5',
    [PRESSED_KEYCODE_6] = L'6',
    [PRESSED_KEYCODE_7] = L'7',
    [PRESSED_KEYCODE_8] = L'8',
    [PRESSED_KEYCODE_9] = L'9',
    [PRESSED_KEYCODE_TICK] = L'`',
    [PRESSED_KEYCODE_MINUS] = L'-',
    [PRESSED_KEYCODE_EUQALS] = L'=',
    [PRESSED_KEYCODE_BACKSLASH] = L'\\',
    [PRESSED_KEYCODE_BKSP] = 0x8, // 
    [PRESSED_KEYCODE_SPACE] = 0x20, //  
    [PRESSED_KEYCODE_TAB] = 0x9, // 
    [PRESSED_KEYCODE_CAPS] = 0x0, // 
    [PRESSED_KEYCODE_LSHFT] = 0x0, // 
    [PRESSED_KEYCODE_LCTRL] = 0x0, // 
    [PRESSED_KEYCODE_LGUI] = 0x0, // 
    [PRESSED_KEYCODE_LALT] = 0x0, // 
    [PRESSED_KEYCODE_RSHFT] = 0x0, // 
    [PRESSED_KEYCODE_RCTRL] = 0x0, // 
    [PRESSED_KEYCODE_RGUI] = 0x0, // 
    [PRESSED_KEYCODE_RALT] = 0x0, // 
    [PRESSED_KEYCODE_APPS] = 0x0, // 
    [PRESSED_KEYCODE_ENTER] = 0xA, // 
    [PRESSED_KEYCODE_ESC] = 0x1B, // 
    [PRESSED_KEYCODE_F1] = 0x0, // 
    [PRESSED_KEYCODE_F2] = 0x0, // 
    [PRESSED_KEYCODE_F3] = 0x0, // 
    [PRESSED_KEYCODE_F4] = 0x0, // 
    [PRESSED_KEYCODE_F5] = 0x0, // 
    [PRESSED_KEYCODE_F6] = 0x0, // 
    [PRESSED_KEYCODE_F7] = 0x0, // 
    [PRESSED_KEYCODE_F8] = 0x0, // 
    [PRESSED_KEYCODE_F9] = 0x0, // 
    [PRESSED_KEYCODE_F10] = 0x0, // 
    [PRESSED_KEYCODE_F11] = 0x0, // 
    [PRESSED_KEYCODE_F12] = 0x0, // 
    [PRESSED_KEYCODE_PRNTSCRN] = 0x0, // 
    [PRESSED_KEYCODE_SCROLL] = 0x0, // 
    [PRESSED_KEYCODE_PAUSE] = 0x0, // 
    [PRESSED_KEYCODE_L_SQUARE_BR] = L'х',
    [PRESSED_KEYCODE_INSERT] = 0x0, // 
    [PRESSED_KEYCODE_HOME] = 0x0, // 
    [PRESSED_KEYCODE_PGUP] = 0x0, // 
    [PRESSED_KEYCODE_DELETE] = 0x7F, // 
    [PRESSED_KEYCODE_END] = 0x0, // 
    [PRESSED_KEYCODE_PGDN] = 0x0, // 
    [PRESSED_KEYCODE_U_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_L_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_D_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_R_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_NUM] = 0x0, // 
    [PRESSED_KEYCODE_KP_SLASH] = L'/',
    [PRESSED_KEYCODE_KP_STAR] = L'*',
    [PRESSED_KEYCODE_KP_MINUS] = L'-',
    [PRESSED_KEYCODE_KP_PLUS] = L'+',
    [PRESSED_KEYCODE_KPEN] = 0xA, //
    [PRESSED_KEYCODE_KP_DOT] = L'.',
    [PRESSED_KEYCODE_KP0] = 0x0, // 
    [PRESSED_KEYCODE_KP1] = 0x0, // 
    [PRESSED_KEYCODE_KP2] = 0x0, // 
    [PRESSED_KEYCODE_KP3] = 0x0, // 
    [PRESSED_KEYCODE_KP4] = 0x0, // 
    [PRESSED_KEYCODE_KP5] = 0x0, // 
    [PRESSED_KEYCODE_KP6] = 0x0, // 
    [PRESSED_KEYCODE_KP7] = 0x0, // 
    [PRESSED_KEYCODE_KP8] = 0x0, // 
    [PRESSED_KEYCODE_KP9] = 0x0, // 
    [PRESSED_KEYCODE_R_SQUARE_BR] = L'ъ',
    [PRESSED_KEYCODE_SEMICOLON] = L'ж',
    [PRESSED_KEYCODE_APOSTROPHE] = L'э',
    [PRESSED_KEYCODE_COLON] = L'б',
    [PRESSED_KEYCODE_DOT] = L'ю',
    [PRESSED_KEYCODE_SLASH] = L'.',
    [104] = 0x0, // 
    [105] = 0x0, // 
    [106] = 0x0, // 
    [107] = 0x0, // 
    [108] = 0x0, // 
    [109] = 0x0, // 
    [110] = 0x0, // 
    [111] = 0x0, // 
    [112] = 0x0, // 
    [113] = 0x0, // 
    [114] = 0x0, // 
    [115] = 0x0, // 
    [116] = 0x0, // 
    [117] = 0x0, // 
    [118] = 0x0, // 
    [119] = 0x0, // 
    [120] = 0x0, // 
    [121] = 0x0, // 
    [122] = 0x0, // 
    [123] = 0x0, // 
    [124] = 0x0, // 
    [125] = 0x0, // 
    [126] = 0x0, // 
    [127] = 0x0, // 
};


static uint16 CYR_SECOND_CHARACTER_MAP[128] = {
    [PRESSED_KEYCODE_A] = L'Ф',
    [PRESSED_KEYCODE_B] = L'И',
    [PRESSED_KEYCODE_C] = L'С',
    [PRESSED_KEYCODE_D] = L'В',
    [PRESSED_KEYCODE_E] = L'У',
    [PRESSED_KEYCODE_F] = L'А',
    [PRESSED_KEYCODE_G] = L'П',
    [PRESSED_KEYCODE_H] = L'Р',
    [PRESSED_KEYCODE_I] = L'Ш',
    [PRESSED_KEYCODE_J] = L'О',
    [PRESSED_KEYCODE_K] = L'Л',
    [PRESSED_KEYCODE_L] = L'Д',
    [PRESSED_KEYCODE_M] = L'Ь',
    [PRESSED_KEYCODE_N] = L'Т',
    [PRESSED_KEYCODE_O] = L'Щ',
    [PRESSED_KEYCODE_P] = L'З',
    [PRESSED_KEYCODE_Q] = L'Й',
    [PRESSED_KEYCODE_R] = L'К',
    [PRESSED_KEYCODE_S] = L'Ы',
    [PRESSED_KEYCODE_T] = L'Е',
    [PRESSED_KEYCODE_U] = L'Г',
    [PRESSED_KEYCODE_V] = L'М',
    [PRESSED_KEYCODE_W] = L'Ц',
    [PRESSED_KEYCODE_X] = L'Ч',
    [PRESSED_KEYCODE_Y] = L'Н',
    [PRESSED_KEYCODE_Z] = L'Я',
    [PRESSED_KEYCODE_0] = L')',
    [PRESSED_KEYCODE_1] = L'!',
    [PRESSED_KEYCODE_2] = L'"',
    [PRESSED_KEYCODE_3] = L'№',
    [PRESSED_KEYCODE_4] = L';',
    [PRESSED_KEYCODE_5] = L'%',
    [PRESSED_KEYCODE_6] = L':',
    [PRESSED_KEYCODE_7] = L'?',
    [PRESSED_KEYCODE_8] = L'*',
    [PRESSED_KEYCODE_9] = L'(',
    [PRESSED_KEYCODE_TICK] = L'~',
    [PRESSED_KEYCODE_MINUS] = L'_',
    [PRESSED_KEYCODE_EUQALS] = L'+',
    [PRESSED_KEYCODE_BACKSLASH] = L'/',
    [PRESSED_KEYCODE_BKSP] = 0x8, // 
    [PRESSED_KEYCODE_SPACE] = 0x20, //  
    [PRESSED_KEYCODE_TAB] = 0x9, // 
    [PRESSED_KEYCODE_CAPS] = 0x0, // 
    [PRESSED_KEYCODE_LSHFT] = 0x0, // 
    [PRESSED_KEYCODE_LCTRL] = 0x0, // 
    [PRESSED_KEYCODE_LGUI] = 0x0, // 
    [PRESSED_KEYCODE_LALT] = 0x0, // 
    [PRESSED_KEYCODE_RSHFT] = 0x0, // 
    [PRESSED_KEYCODE_RCTRL] = 0x0, // 
    [PRESSED_KEYCODE_RGUI] = 0x0, // 
    [PRESSED_KEYCODE_RALT] = 0x0, // 
    [PRESSED_KEYCODE_APPS] = 0x0, // 
    [PRESSED_KEYCODE_ENTER] = 0xA, // 
    [PRESSED_KEYCODE_ESC] = 0x1B, // 
    [PRESSED_KEYCODE_F1] = 0x0, // 
    [PRESSED_KEYCODE_F2] = 0x0, // 
    [PRESSED_KEYCODE_F3] = 0x0, // 
    [PRESSED_KEYCODE_F4] = 0x0, // 
    [PRESSED_KEYCODE_F5] = 0x0, // 
    [PRESSED_KEYCODE_F6] = 0x0, // 
    [PRESSED_KEYCODE_F7] = 0x0, // 
    [PRESSED_KEYCODE_F8] = 0x0, // 
    [PRESSED_KEYCODE_F9] = 0x0, // 
    [PRESSED_KEYCODE_F10] = 0x0, // 
    [PRESSED_KEYCODE_F11] = 0x0, // 
    [PRESSED_KEYCODE_F12] = 0x0, // 
    [PRESSED_KEYCODE_PRNTSCRN] = 0x0, // 
    [PRESSED_KEYCODE_SCROLL] = 0x0, // 
    [PRESSED_KEYCODE_PAUSE] = 0x0, // 
    [PRESSED_KEYCODE_L_SQUARE_BR] = L'Х',
    [PRESSED_KEYCODE_INSERT] = 0x0, // 
    [PRESSED_KEYCODE_HOME] = 0x0, // 
    [PRESSED_KEYCODE_PGUP] = 0x0, // 
    [PRESSED_KEYCODE_DELETE] = 0x7F, // 
    [PRESSED_KEYCODE_END] = 0x0, // 
    [PRESSED_KEYCODE_PGDN] = 0x0, // 
    [PRESSED_KEYCODE_U_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_L_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_D_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_R_ARROW] = 0x0, // 
    [PRESSED_KEYCODE_NUM] = 0x0, // 
    [PRESSED_KEYCODE_KP_SLASH] = L'/',
    [PRESSED_KEYCODE_KP_STAR] = L'*',
    [PRESSED_KEYCODE_KP_MINUS] = L'-',
    [PRESSED_KEYCODE_KP_PLUS] = L'+',
    [PRESSED_KEYCODE_KPEN] = 0xA, // 
    [PRESSED_KEYCODE_KP_DOT] = L'.',
    [PRESSED_KEYCODE_KP0] = 0x0, // 
    [PRESSED_KEYCODE_KP1] = 0x0, // 
    [PRESSED_KEYCODE_KP2] = 0x0, // 
    [PRESSED_KEYCODE_KP3] = 0x0, // 
    [PRESSED_KEYCODE_KP4] = 0x0, // 
    [PRESSED_KEYCODE_KP5] = 0x0, // 
    [PRESSED_KEYCODE_KP6] = 0x0, // 
    [PRESSED_KEYCODE_KP7] = 0x0, // 
    [PRESSED_KEYCODE_KP8] = 0x0, // 
    [PRESSED_KEYCODE_KP9] = 0x0, // 
    [PRESSED_KEYCODE_R_SQUARE_BR] = L'Ъ',
    [PRESSED_KEYCODE_SEMICOLON] = L'Ж',
    [PRESSED_KEYCODE_APOSTROPHE] = L'Э',
    [PRESSED_KEYCODE_COLON] = L'Б',
    [PRESSED_KEYCODE_DOT] = L'Ю',
    [PRESSED_KEYCODE_SLASH] = L',',
    [104] = 0x0, // 
    [105] = 0x0, // 
    [106] = 0x0, // 
    [107] = 0x0, // 
    [108] = 0x0, // 
    [109] = 0x0, // 
    [110] = 0x0, // 
    [111] = 0x0, // 
    [112] = 0x0, // 
    [113] = 0x0, // 
    [114] = 0x0, // 
    [115] = 0x0, // 
    [116] = 0x0, // 
    [117] = 0x0, // 
    [118] = 0x0, // 
    [119] = 0x0, // 
    [120] = 0x0, // 
    [121] = 0x0, // 
    [122] = 0x0, // 
    [123] = 0x0, // 
    [124] = 0x0, // 
    [125] = 0x0, // 
    [126] = 0x0, // 
    [127] = 0x0  // 
};
