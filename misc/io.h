#ifndef IO_H
#define IO_H

#include "../types.h"


void outb(uint16 port_number, uint8 data);
void outw(uint16 port_number, uint16 data);
uint8 inb(uint16 port_number);
uint16 inw(uint16 port_number);


#endif // IO_H