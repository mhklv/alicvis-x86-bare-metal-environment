#include "timer.h"
#include "types.h"
#include "misc/io.h"
#include "boot/interrupts.h"


static uint8 counter_states[COUNTERS_NUMBER];
static uint32 counters[COUNTERS_NUMBER];


uint8 clocking_active() {
    uint32 i;

    for (i = 0; i < COUNTERS_NUMBER; ++i) {
        if (counter_states[i] != CNT_UNUSED) {
            return 1;
        }
    }

    return 0;
}


void init_timer() {
    uint32 i;
    uint16 reload_value = RELOAD_VALUE;

    for (i = 0; i < COUNTERS_NUMBER; ++i) {
        counter_states[i] = CNT_UNUSED;
    }

    outb(PIT_MODE_COMMAND, COM_CHANNEL0 | COM_MODE2 | COM_ACCESS_LOBYTE_HIBYTE);
    outb(PIT_CHANNEL0, reload_value);
    outb(PIT_CHANNEL0, reload_value >> 8);
}


void sleep(uint32 millisecs) {
    uint32 i, tmp;

    if (millisecs == 0) {
        return;
    }

    for (i = 0; i < COUNTERS_NUMBER; ++i) {
        if (counter_states[i] == CNT_UNUSED) {
            break;
        }
    }

    if (i == COUNTERS_NUMBER) {
        return;
    }

    if (clocking_active()) {
        counter_states[i] = CNT_COUNTING_DOWN;
        counters[i] = millisecs;
    }
    else {
        counter_states[i] = CNT_COUNTING_DOWN;
        counters[i] = millisecs;
        IRQ_clear_mask(0);
    }

    asm volatile (
        "1:"
        "movl (%[counter_states], %[i]), %[tmp];"
        "cmpl %[tmp], %[CNT_UNUSED];"
        "je 2f;"
        "hlt;"
        "jmp 1b;"
        "2:"
        : [tmp] "+r" (tmp)
        : [i] "r" (i),
          [counter_states] "r" (counter_states),
          [CNT_UNUSED] "r" (CNT_UNUSED)
        : "memory", "cc"
    );

    if (!clocking_active()) {
        IRQ_set_mask(0);
    }
}



__attribute__ ((interrupt))
void irq0_handler(struct interrupt_frame *frame) {
    uint32 i;

    for (i = 0; i < COUNTERS_NUMBER; ++i) {
        switch (counter_states[i]) {
            case CNT_COUNTING_DOWN:
                if (counters[i] == 0) {
                    counter_states[i] = CNT_UNUSED;
                }
                else {
                    counters[i] -= 1;
                }

                break;
            
            case CNT_COUNTING_UP:
                counters[i] += 1;
                
                break;
            
            default:
                break;
        }
    }

    outb(PIC1_COMMAND, PIC_EOI);
}


int8 start_timer() {
    uint32 i;

    for (i = 0; i < COUNTERS_NUMBER; ++i) {
        if (counter_states[i] == CNT_UNUSED) {
            break;
        }
    }

    if (i == COUNTERS_NUMBER) {
        return -1;
    }

    if (clocking_active()) {
        counters[i] = 0;
        counter_states[i] = CNT_COUNTING_UP;
    }
    else {
        counters[i] = 0;
        counter_states[i] = CNT_COUNTING_UP;
        IRQ_clear_mask(0);
    }

    return i;
}


uint32 stop_timer(uint8 timer_id) {
    uint32 res;
    
    if (timer_id >= COUNTERS_NUMBER || counter_states[timer_id] != CNT_COUNTING_UP) {
        return 0;
    }

    counter_states[timer_id] = CNT_UNUSED;

    if (!clocking_active()) {
        IRQ_set_mask(0);
    }

    return counters[timer_id];
}


uint32 get_timer_val(uint8 timer_id) {
    if (timer_id >= COUNTERS_NUMBER || counter_states[timer_id] != CNT_COUNTING_UP) {
        return 0;
    }

    return counters[timer_id];
}


