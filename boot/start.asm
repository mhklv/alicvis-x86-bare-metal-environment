[BITS 32]

section .text
global _start:function
global switch_to_real_mode_and_shutdown:function
extern main
extern init_idt


_start: call init_idt

	    call main
	    
	    cli
.L1:    hlt
	    jmp .L1


; Switching from protected mode to real mode as described in the Intel's manual Volume 3A, 9.9.2.
; This code must be loaded at address 10000h.
switch_to_real_mode_and_shutdown:
        cli

        jmp 24 : shutdown16
        
[BITS 16]
shutdown16:
        mov ax, 32
	    mov ds, ax
	    mov es, ax
	    mov fs, ax
	    mov gs, ax
	    mov ss, ax

        lidt [idt_real]

        mov eax, cr0
        and eax, 0xFFFFFFFE
        mov cr0, eax

        jmp 1000h : pipeline_cleared_real_mode

pipeline_cleared_real_mode:
        mov ax, 1000h
	    mov ds, ax
	    mov es, ax
	    mov fs, ax
	    mov gs, ax
	    mov ss, ax

        mov sp, 0

        mov ax, 0x5307
        mov bx, 0x0001
        mov cx, 0x0003
        int 0x15
	    
	    cli
.L1:    hlt
	    jmp .L1

idt_real:
	    dw 0x3ff		; 256 entries, start at address 0
	    dd 0
