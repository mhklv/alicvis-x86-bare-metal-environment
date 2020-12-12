#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"


#define IDT_BASE_ADDRESS 0x20000
#define CODE_SELECTOR 0x8

#define PIC1		0x20		 // IO base address for master PIC
#define PIC2		0xA0		 // IO base address for slave PIC
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		// End-of-interrupt command code

#define ICW1_ICW4	0x01		    // ICW4 needed
#define ICW1_SINGLE	0x02		    // Single (cascade) mode
#define ICW1_INTERVAL4	0x04		// Call address interval 4 (8)
#define ICW1_LEVEL	0x08		    // Level triggered (edge) mode
#define ICW1_INIT	0x10		    // Initialization (required!)
 
#define ICW4_8086	0x01		    // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO	0x02		    // Auto (normal) EOI
#define ICW4_BUF_SLAVE	0x08		// Buffered mode/slave
#define ICW4_BUF_MASTER	0x0C		// Buffered mode/master
#define ICW4_SFNM	0x10		    // Special fully nested (not)


struct interrupt_frame {
    uint16 ip;
    uint16 cs;
    uint16 flags;
    uint16 sp;
    uint16 ss;
};


struct IDT_descriptor {
   uint16 offset_lower; // offset bits 0..15
   uint16 selector; // a code segment selector in GDT
   uint8 zero;      // unused, set to 0
   uint8 type_attr; // type and attributes
   uint16 offset_higher; // offset bits 16..31
};



void IRQ_set_mask(uint8 irq_line);
void IRQ_clear_mask(uint8 irq_line);


#endif // INTERRUPTS_H