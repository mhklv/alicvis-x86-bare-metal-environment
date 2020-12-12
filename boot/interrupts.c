#include "misc/io.h"
#include "drivers/display.h"
#include "interrupts.h"



static uint32 idt_ptr[2];


void pic_remap() {
    uint8 pic1_offset = 32, pic2_offset = 40;

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // ICW1
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, pic1_offset);               // ICW2 (IDT offset)
    outb(PIC2_DATA, pic2_offset);
    outb(PIC1_DATA, 0x04);          // Slave PIC at IRQ2 (0100b)
    outb(PIC2_DATA, 0x02);          // Cascade ID for the slave PIC
    outb(PIC1_DATA, ICW4_8086);     // Operation mode
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC1_DATA, 0xFB);          //  Disable all IRQs except IRQ2
    outb(PIC2_DATA, 0xFF);
}


void IRQ_set_mask(uint8 irq_line) {
    uint8 port, mask;
    
    if (irq_line < 8) {
        port = PIC1_DATA;
    }
    else if (irq_line < 16) {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    else {
        return;
    }

    mask = ((uint8) 1 << irq_line);

    outb(port, inb(port) | mask);
}


void IRQ_clear_mask(uint8 irq_line) {
    uint8 port, mask;

    if (irq_line < 8) {
        port = PIC1_DATA;
    }
    else if (irq_line < 16) {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    else {
        return;
    }
    
    mask = ~((uint8) 1 << irq_line);

    outb(port, inb(port) & mask);
}


void load_idt(uint32* idt_address) {
    asm volatile (
        "lidt (%[idt_address]);"
        "sti;"
        :
        : [idt_address] "r" (idt_address)
        : "memory"
    );
}


__attribute__ ((interrupt))
void generic_exception_handler(struct interrupt_frame *frame, uint32 error_code) {

}

uint32 cnt = 0;

__attribute__ ((interrupt))
void generic_irq_handler(struct interrupt_frame *frame) {
     plot_pixel(cnt, cnt);
    ++cnt;
    outb(PIC1_COMMAND, PIC_EOI);
}


__attribute__ ((interrupt))
extern void irq0_handler(struct interrupt_frame *frame);


__attribute__ ((interrupt))
extern void irq1_handler(struct interrupt_frame *frame);


void init_idt() {
    pic_remap();
    struct IDT_descriptor IDT[256];
    uint32 i, handler_address, idt_address;
    struct IDT_descriptor* desc_ptr;

    // Set empty exceptions handler
    handler_address = (uint32) generic_exception_handler;

    for (i = 0; i < 32; ++i) {
        IDT[i].offset_lower = handler_address & 0xFFFF;
        IDT[i].selector = CODE_SELECTOR;
        IDT[i].zero = 0;
        IDT[i].type_attr = 0x8E;
        IDT[i].offset_higher = (handler_address & 0xFFFF0000) >> 16;
    }

    // Set empty IRQ handlers
    handler_address = (uint32) generic_irq_handler;

    for (i = 32; i < 48; ++i) {
        IDT[i].offset_lower = handler_address & 0xFFFF;
        IDT[i].selector = CODE_SELECTOR;
        IDT[i].zero = 0;
        IDT[i].type_attr = 0x8E;
        IDT[i].offset_higher = (handler_address & 0xFFFF0000) >> 16;
    }

    // Set IRQ0 (timer) handler
    handler_address = (uint32) irq0_handler;
    IDT[32].offset_lower = handler_address & 0xFFFF;
    IDT[32].offset_higher = (handler_address & 0xFFFF0000) >> 16;

    // Set IRQ1 (keyboard) handler
    handler_address = (uint32) irq1_handler;
    IDT[33].offset_lower = handler_address & 0xFFFF;
    IDT[33].offset_higher = (handler_address & 0xFFFF0000) >> 16;

    // Copy the IDT to it's place
    for (desc_ptr = (struct IDT_descriptor*) IDT_BASE_ADDRESS, i = 0; i < 256; ++desc_ptr, ++i) {
        *desc_ptr = IDT[i];
    }

    // Preparing for LIDT command
    idt_address = (uint32) IDT;
    idt_ptr[0] = (sizeof(struct IDT_descriptor) * 256) | ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    load_idt(idt_ptr);
}



