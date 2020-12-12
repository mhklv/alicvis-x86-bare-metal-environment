#ifndef KEYBOARD_H
#define KEYBOARD_H



struct KeyPressPacket {
    uint8 keycode;
    uint8 ctrl_pressed : 1, shift_pressed : 1, alt_pressed : 1, caps_locked : 1;
    uint16 character;
};


enum KeyCodes {
    PRESSED_KEYCODE_A = 0x0,
    PRESSED_KEYCODE_B = 0x1,
    PRESSED_KEYCODE_C = 0x2,
    PRESSED_KEYCODE_D = 0x3,
    PRESSED_KEYCODE_E = 0x4,
    PRESSED_KEYCODE_F = 0x5,
    PRESSED_KEYCODE_G = 0x6,
    PRESSED_KEYCODE_H = 0x7,
    PRESSED_KEYCODE_I = 0x8,
    PRESSED_KEYCODE_J = 0x9,
    PRESSED_KEYCODE_K = 0xA,
    PRESSED_KEYCODE_L = 0xB,
    PRESSED_KEYCODE_M = 0xC,
    PRESSED_KEYCODE_N = 0xD,
    PRESSED_KEYCODE_O = 0xE,
    PRESSED_KEYCODE_P = 0xF,
    PRESSED_KEYCODE_Q = 0x10,
    PRESSED_KEYCODE_R = 0x11,
    PRESSED_KEYCODE_S = 0x12,
    PRESSED_KEYCODE_T = 0x13,
    PRESSED_KEYCODE_U = 0x14,
    PRESSED_KEYCODE_V = 0x15,
    PRESSED_KEYCODE_W = 0x16,
    PRESSED_KEYCODE_X = 0x17,
    PRESSED_KEYCODE_Y = 0x18,
    PRESSED_KEYCODE_Z = 0x19,
    PRESSED_KEYCODE_0 = 0x1A,
    PRESSED_KEYCODE_1 = 0x1B,
    PRESSED_KEYCODE_2 = 0x1C,
    PRESSED_KEYCODE_3 = 0x1D,
    PRESSED_KEYCODE_4 = 0x1E,
    PRESSED_KEYCODE_5 = 0x1F,
    PRESSED_KEYCODE_6 = 0x20,
    PRESSED_KEYCODE_7 = 0x21,
    PRESSED_KEYCODE_8 = 0x22,
    PRESSED_KEYCODE_9 = 0x23,
    PRESSED_KEYCODE_TICK = 0x24,
    PRESSED_KEYCODE_MINUS = 0x25,
    PRESSED_KEYCODE_EUQALS = 0x26,
    PRESSED_KEYCODE_BACKSLASH = 0x27,
    PRESSED_KEYCODE_BKSP = 0x28,
    PRESSED_KEYCODE_SPACE = 0x29,
    PRESSED_KEYCODE_TAB = 0x2A,
    PRESSED_KEYCODE_CAPS = 0x2B,
    PRESSED_KEYCODE_LSHFT = 0x2C,
    PRESSED_KEYCODE_LCTRL = 0x2D,
    PRESSED_KEYCODE_LGUI = 0x2E,
    PRESSED_KEYCODE_LALT = 0x2F,
    PRESSED_KEYCODE_RSHFT = 0x30,
    PRESSED_KEYCODE_RCTRL = 0x31,
    PRESSED_KEYCODE_RGUI = 0x32,
    PRESSED_KEYCODE_RALT = 0x33,
    PRESSED_KEYCODE_APPS = 0x34,
    PRESSED_KEYCODE_ENTER = 0x35,
    PRESSED_KEYCODE_ESC = 0x36,
    PRESSED_KEYCODE_F1 = 0x37,
    PRESSED_KEYCODE_F2 = 0x38,
    PRESSED_KEYCODE_F3 = 0x39,
    PRESSED_KEYCODE_F4 = 0x3A,
    PRESSED_KEYCODE_F5 = 0x3B,
    PRESSED_KEYCODE_F6 = 0x3C,
    PRESSED_KEYCODE_F7 = 0x3D,
    PRESSED_KEYCODE_F8 = 0x3E,
    PRESSED_KEYCODE_F9 = 0x3F,
    PRESSED_KEYCODE_F10 = 0x40,
    PRESSED_KEYCODE_F11 = 0x41,
    PRESSED_KEYCODE_F12 = 0x42,
    PRESSED_KEYCODE_PRNTSCRN = 0x43,
    PRESSED_KEYCODE_SCROLL = 0x44,
    PRESSED_KEYCODE_PAUSE = 0x45,
    PRESSED_KEYCODE_L_SQUARE_BR = 0x46,
    PRESSED_KEYCODE_INSERT = 0x47,
    PRESSED_KEYCODE_HOME = 0x48,
    PRESSED_KEYCODE_PGUP = 0x49,
    PRESSED_KEYCODE_DELETE = 0x4A,
    PRESSED_KEYCODE_END = 0x4B,
    PRESSED_KEYCODE_PGDN = 0x4C,
    PRESSED_KEYCODE_U_ARROW = 0x4D,
    PRESSED_KEYCODE_L_ARROW = 0x4E,
    PRESSED_KEYCODE_D_ARROW = 0x4F,
    PRESSED_KEYCODE_R_ARROW = 0x50,
    PRESSED_KEYCODE_NUM = 0x51,
    PRESSED_KEYCODE_KP_SLASH = 0x52,
    PRESSED_KEYCODE_KP_STAR = 0x53,
    PRESSED_KEYCODE_KP_MINUS = 0x54,
    PRESSED_KEYCODE_KP_PLUS = 0x55,
    PRESSED_KEYCODE_KPEN = 0x56,
    PRESSED_KEYCODE_KP_DOT = 0x57,
    PRESSED_KEYCODE_KP0 = 0x58,
    PRESSED_KEYCODE_KP1 = 0x59,
    PRESSED_KEYCODE_KP2 = 0x5A,
    PRESSED_KEYCODE_KP3 = 0x5B,
    PRESSED_KEYCODE_KP4 = 0x5C,
    PRESSED_KEYCODE_KP5 = 0x5D,
    PRESSED_KEYCODE_KP6 = 0x5E,
    PRESSED_KEYCODE_KP7 = 0x5F,
    PRESSED_KEYCODE_KP8 = 0x60,
    PRESSED_KEYCODE_KP9 = 0x61,
    PRESSED_KEYCODE_R_SQUARE_BR = 0x62,
    PRESSED_KEYCODE_SEMICOLON = 0x63,
    PRESSED_KEYCODE_APOSTROPHE = 0x64,
    PRESSED_KEYCODE_COLON = 0x65,
    PRESSED_KEYCODE_DOT = 0x66,
    PRESSED_KEYCODE_SLASH = 0x67,

    RELEASED_KEYCODE_A = PRESSED_KEYCODE_A + 0x80,
    RELEASED_KEYCODE_B = PRESSED_KEYCODE_B + 0x80,
    RELEASED_KEYCODE_C = PRESSED_KEYCODE_C + 0x80,
    RELEASED_KEYCODE_D = PRESSED_KEYCODE_D + 0x80,
    RELEASED_KEYCODE_E = PRESSED_KEYCODE_E + 0x80,
    RELEASED_KEYCODE_F = PRESSED_KEYCODE_F + 0x80,
    RELEASED_KEYCODE_G = PRESSED_KEYCODE_G + 0x80,
    RELEASED_KEYCODE_H = PRESSED_KEYCODE_H + 0x80,
    RELEASED_KEYCODE_I = PRESSED_KEYCODE_I + 0x80,
    RELEASED_KEYCODE_J = PRESSED_KEYCODE_J + 0x80,
    RELEASED_KEYCODE_K = PRESSED_KEYCODE_K + 0x80,
    RELEASED_KEYCODE_L = PRESSED_KEYCODE_L + 0x80,
    RELEASED_KEYCODE_M = PRESSED_KEYCODE_M + 0x80,
    RELEASED_KEYCODE_N = PRESSED_KEYCODE_N + 0x80,
    RELEASED_KEYCODE_O = PRESSED_KEYCODE_O + 0x80,
    RELEASED_KEYCODE_P = PRESSED_KEYCODE_P + 0x80,
    RELEASED_KEYCODE_Q = PRESSED_KEYCODE_Q + 0x80,
    RELEASED_KEYCODE_R = PRESSED_KEYCODE_R + 0x80,
    RELEASED_KEYCODE_S = PRESSED_KEYCODE_S + 0x80,
    RELEASED_KEYCODE_T = PRESSED_KEYCODE_T + 0x80,
    RELEASED_KEYCODE_U = PRESSED_KEYCODE_U + 0x80,
    RELEASED_KEYCODE_V = PRESSED_KEYCODE_V + 0x80,
    RELEASED_KEYCODE_W = PRESSED_KEYCODE_W + 0x80,
    RELEASED_KEYCODE_X = PRESSED_KEYCODE_X + 0x80,
    RELEASED_KEYCODE_Y = PRESSED_KEYCODE_Y + 0x80,
    RELEASED_KEYCODE_Z = PRESSED_KEYCODE_Z + 0x80,
    RELEASED_KEYCODE_0 = PRESSED_KEYCODE_0 + 0x80,
    RELEASED_KEYCODE_1 = PRESSED_KEYCODE_1 + 0x80,
    RELEASED_KEYCODE_2 = PRESSED_KEYCODE_2 + 0x80,
    RELEASED_KEYCODE_3 = PRESSED_KEYCODE_3 + 0x80,
    RELEASED_KEYCODE_4 = PRESSED_KEYCODE_4 + 0x80,
    RELEASED_KEYCODE_5 = PRESSED_KEYCODE_5 + 0x80,
    RELEASED_KEYCODE_6 = PRESSED_KEYCODE_6 + 0x80,
    RELEASED_KEYCODE_7 = PRESSED_KEYCODE_7 + 0x80,
    RELEASED_KEYCODE_8 = PRESSED_KEYCODE_8 + 0x80,
    RELEASED_KEYCODE_9 = PRESSED_KEYCODE_9 + 0x80,
    RELEASED_KEYCODE_TICK = PRESSED_KEYCODE_TICK + 0x80,
    RELEASED_KEYCODE_MINUS = PRESSED_KEYCODE_MINUS + 0x80,
    RELEASED_KEYCODE_EUQALS = PRESSED_KEYCODE_EUQALS + 0x80,
    RELEASED_KEYCODE_BACKSLASH = PRESSED_KEYCODE_BACKSLASH + 0x80,
    RELEASED_KEYCODE_BKSP = PRESSED_KEYCODE_BKSP + 0x80,
    RELEASED_KEYCODE_SPACE = PRESSED_KEYCODE_SPACE + 0x80,
    RELEASED_KEYCODE_TAB = PRESSED_KEYCODE_TAB + 0x80,
    RELEASED_KEYCODE_CAPS = PRESSED_KEYCODE_CAPS + 0x80,
    RELEASED_KEYCODE_LSHFT = PRESSED_KEYCODE_LSHFT + 0x80,
    RELEASED_KEYCODE_LCTRL = PRESSED_KEYCODE_LCTRL + 0x80,
    RELEASED_KEYCODE_LGUI = PRESSED_KEYCODE_LGUI + 0x80,
    RELEASED_KEYCODE_LALT = PRESSED_KEYCODE_LALT + 0x80,
    RELEASED_KEYCODE_RSHFT = PRESSED_KEYCODE_RSHFT + 0x80,
    RELEASED_KEYCODE_RCTRL = PRESSED_KEYCODE_RCTRL + 0x80,
    RELEASED_KEYCODE_RGUI = PRESSED_KEYCODE_RGUI + 0x80,
    RELEASED_KEYCODE_RALT = PRESSED_KEYCODE_RALT + 0x80,
    RELEASED_KEYCODE_APPS = PRESSED_KEYCODE_APPS + 0x80,
    RELEASED_KEYCODE_ENTER = PRESSED_KEYCODE_ENTER + 0x80,
    RELEASED_KEYCODE_ESC = PRESSED_KEYCODE_ESC + 0x80,
    RELEASED_KEYCODE_F1 = PRESSED_KEYCODE_F1 + 0x80,
    RELEASED_KEYCODE_F2 = PRESSED_KEYCODE_F2 + 0x80,
    RELEASED_KEYCODE_F3 = PRESSED_KEYCODE_F3 + 0x80,
    RELEASED_KEYCODE_F4 = PRESSED_KEYCODE_F4 + 0x80,
    RELEASED_KEYCODE_F5 = PRESSED_KEYCODE_F5 + 0x80,
    RELEASED_KEYCODE_F6 = PRESSED_KEYCODE_F6 + 0x80,
    RELEASED_KEYCODE_F7 = PRESSED_KEYCODE_F7 + 0x80,
    RELEASED_KEYCODE_F8 = PRESSED_KEYCODE_F8 + 0x80,
    RELEASED_KEYCODE_F9 = PRESSED_KEYCODE_F9 + 0x80,
    RELEASED_KEYCODE_F10 = PRESSED_KEYCODE_F10 + 0x80,
    RELEASED_KEYCODE_F11 = PRESSED_KEYCODE_F11 + 0x80,
    RELEASED_KEYCODE_F12 = PRESSED_KEYCODE_F12 + 0x80,
    RELEASED_KEYCODE_PRNTSCRN = PRESSED_KEYCODE_PRNTSCRN + 0x80,
    RELEASED_KEYCODE_SCROLL = PRESSED_KEYCODE_SCROLL + 0x80,
    RELEASED_KEYCODE_L_SQUARE_BR = PRESSED_KEYCODE_L_SQUARE_BR + 0x80,
    RELEASED_KEYCODE_INSERT = PRESSED_KEYCODE_INSERT + 0x80,
    RELEASED_KEYCODE_HOME = PRESSED_KEYCODE_HOME + 0x80,
    RELEASED_KEYCODE_PGUP = PRESSED_KEYCODE_PGUP + 0x80,
    RELEASED_KEYCODE_DELETE = PRESSED_KEYCODE_DELETE + 0x80,
    RELEASED_KEYCODE_END = PRESSED_KEYCODE_END + 0x80,
    RELEASED_KEYCODE_PGDN = PRESSED_KEYCODE_PGDN + 0x80,
    RELEASED_KEYCODE_U_ARROW = PRESSED_KEYCODE_U_ARROW + 0x80,
    RELEASED_KEYCODE_L_ARROW = PRESSED_KEYCODE_L_ARROW + 0x80,
    RELEASED_KEYCODE_D_ARROW = PRESSED_KEYCODE_D_ARROW + 0x80,
    RELEASED_KEYCODE_R_ARROW = PRESSED_KEYCODE_R_ARROW + 0x80,
    RELEASED_KEYCODE_NUM = PRESSED_KEYCODE_NUM + 0x80,
    RELEASED_KEYCODE_KP_SLASH = PRESSED_KEYCODE_KP_SLASH + 0x80,
    RELEASED_KEYCODE_KP_STAR = PRESSED_KEYCODE_KP_STAR + 0x80,
    RELEASED_KEYCODE_KP_MINUS = PRESSED_KEYCODE_KP_MINUS + 0x80,
    RELEASED_KEYCODE_KP_PLUS = PRESSED_KEYCODE_KP_PLUS + 0x80,
    RELEASED_KEYCODE_KPEN = PRESSED_KEYCODE_KPEN + 0x80,
    RELEASED_KEYCODE_KP_DOT = PRESSED_KEYCODE_KP_DOT + 0x80,
    RELEASED_KEYCODE_KP0 = PRESSED_KEYCODE_KP0 + 0x80,
    RELEASED_KEYCODE_KP1 = PRESSED_KEYCODE_KP1 + 0x80,
    RELEASED_KEYCODE_KP2 = PRESSED_KEYCODE_KP2 + 0x80,
    RELEASED_KEYCODE_KP3 = PRESSED_KEYCODE_KP3 + 0x80,
    RELEASED_KEYCODE_KP4 = PRESSED_KEYCODE_KP4 + 0x80,
    RELEASED_KEYCODE_KP5 = PRESSED_KEYCODE_KP5 + 0x80,
    RELEASED_KEYCODE_KP6 = PRESSED_KEYCODE_KP6 + 0x80,
    RELEASED_KEYCODE_KP7 = PRESSED_KEYCODE_KP7 + 0x80,
    RELEASED_KEYCODE_KP8 = PRESSED_KEYCODE_KP8 + 0x80,
    RELEASED_KEYCODE_KP9 = PRESSED_KEYCODE_KP9 + 0x80,
    RELEASED_KEYCODE_R_SQUARE_BR = PRESSED_KEYCODE_R_SQUARE_BR + 0x80,
    RELEASED_KEYCODE_SEMICOLON = PRESSED_KEYCODE_SEMICOLON + 0x80,
    RELEASED_KEYCODE_APOSTROPHE = PRESSED_KEYCODE_APOSTROPHE + 0x80,
    RELEASED_KEYCODE_COLON = PRESSED_KEYCODE_COLON + 0x80,
    RELEASED_KEYCODE_DOT = PRESSED_KEYCODE_DOT + 0x80,
    RELEASED_KEYCODE_SLASH = PRESSED_KEYCODE_SLASH + 0x80,

    KEYCODE_INVALID = 0xFF
};





void init_keyboard();
uint8 key_queue_empty();
void flush_key_queue();
struct KeyPressPacket dequeue_press_packet();
uint8 dequeue_key();



#endif // KEYBOARD_H