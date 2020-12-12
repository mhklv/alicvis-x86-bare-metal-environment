#include "io.h"



void outb(uint16 port_number, uint8 data) {
    asm volatile (
        "out %%al, %%dx;"
        : 
        : [port_number] "d" (port_number),
          [data] "a" (data)
        : "memory"
    );    
}


void outw(uint16 port_number, uint16 data) {
    asm volatile (
        "out %%ax, %%dx;"
        : 
        : [port_number] "d" (port_number),
          [data] "a" (data)
        : "memory"
    );
}


uint8 inb(uint16 port_number) {
    uint8 data;

    asm volatile (
        "in %%dx, %%al;"
        "movb %%al, %[data];"
        : [data] "=r" (data)
        : [port_number] "d" (port_number)
        : "memory", "ax"
    );

    return data;
}


uint16 inw(uint16 port_number) {
    uint16 data;

    asm volatile (
        "in %%dx, %%ax;"
        "movw %%ax, %[data];"
        : [data] "=r" (data)
        : [port_number] "d" (port_number)
        : "memory", "ax"
    );

    return data;
}