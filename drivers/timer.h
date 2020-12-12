#ifndef TIMER_H
#define TIMER_H

#include "types.h"



#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_MODE_COMMAND 0x43

#define COUNTERS_NUMBER 8
#define TIMER_BASE_FREQUENCY 1193182    // Hz
#define TIMER_OUTPUT_FREQUENCY 1000     // Hz
#define RELOAD_VALUE (TIMER_BASE_FREQUENCY / TIMER_OUTPUT_FREQUENCY)


enum ModeCommandFlags {
    COM_BCD_MODE = 0x1,
    // ----------
    COM_MODE0 = 0x0,
    COM_MODE1 = 0x2,
    COM_MODE2 = 0x4,
    COM_MODE3 = 0x6,
    COM_MODE4 = 0x8,
    COM_MODE5 = 0xA,
    // ----------
    COM_LATCH_COUNT_VAL = 0x0,
    COM_ACCESS_LOBYTE_ONLY = 0x10,
    COM_ACCESS_HIBYTE_ONLY = 0x20,
    COM_ACCESS_LOBYTE_HIBYTE = 0x30,
    // ----------
    COM_CHANNEL0 = 0x0,
    COM_CHANNEL1 = 0x40,
    COM_CHANNEL2 = 0x80,
    COM_READBACK = 0xC0     // (8254 only)
};


enum CounterStates {
    CNT_UNUSED,
    CNT_COUNTING_DOWN,
    CNT_COUNTING_UP
};



void init_timer();
void sleep(uint32 millisecs);
int8 start_timer();
uint32 stop_timer(uint8 timer_id);
uint32 get_timer_val(uint8 timer_id);


#endif // TIMER_H