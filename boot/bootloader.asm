org 7C00h
[BITS 16]

        
        ; Initialize code and data segment.
        ; We want cs = ds = es = 0000h.
        ; No interrupts must be enabled because,
        ; handlers may use uninitialized stack.
        cli
        jmp 0000h:boot_start
        

boot_start:
        xor ax, ax
        mov ds, ax
        mov es, ax

        ; Initialize stack in a 64K segment at 90000h (end of conventional memory)
        mov ax, 8000h
        mov ss, ax
        xor bp, bp
        mov sp, bp
        ; sti

        

        ; Get info about 118h video mode
        mov ax, 4F01h
        mov cx, 0118h
        mov di, video_mode_info_buffer
        int 10h

        ; Set 118h video mode
        mov ax, 4F02h
        mov bx, 0118h
        int 10h


        ; Load 128 sectors starting from the 2-nd
        ; into the 64K segment 1000h.
        mov ah, 42h
        ; mov dl, 80h     ; 80h -- first drive, 81h -- second drive
        mov si, DAP
        int 13h
        jc err_halt

        ; mov ax, 1000h
        ; mov es, ax
        ; mov ah, 02h
        ; mov al, 17
        ; mov ch, 0
        ; mov cl, 2
        ; mov dh, 0
        ; ; mov dl, 80h
        ; mov bx, 0
        ; int 13h
        ; jc err_halt

        ; Switch to the protected operation mode
        cli
        lgdt [gdtr_contents]
        mov eax, cr0
        or al, 1
        mov cr0, eax

        ; Clear the pipeline
        jmp code_descriptor_offset:pipeline_cleared

        

        
[BITS 32]
pipeline_cleared:
        ; Set data segment registers
        mov ax, data_descriptor_offset
        mov ss, ax
        mov es, ax
        mov ds, ax
        mov fs, ax
        mov gs, ax
        ; mov esp, 80000h
        ; mov ebp, 80000h
        mov esp, 90000000h
        mov ebp, 90000000h

        ; Pass parameters to the C code
        mov edi, kernel_load_address
        mov eax, [video_mode_info_buffer+28h]
        mov [edi], eax
        add edi, 4
        mov ax, [video_mode_info_buffer+12h]
        movzx eax, ax
        mov [edi], eax
        add edi, 4
        mov ax, [video_mode_info_buffer+14h]
        movzx eax, ax
        mov [edi], eax
        add edi, 4
        mov eax, 24         ; To be generalized
        mov [edi], eax
        add edi, 4
        mov ax, [video_mode_info_buffer+10h]
        movzx eax, ax
        mov [edi], eax


        ; Start the execution of the C code
        jmp (kernel_load_address + promotion_params_end - promotion_params_start)


        ; Halt the execution if an error occured
err_halt:
        cli
.h:     hlt
        jmp .h


align 8
DAP:
        db 10h                                ; Size of the DAP
        db 00h                                ; Reserved
        dw sectors_to_load                    ; Number of blocks to transfer
        dd (kernel_load_address << 12)        ; Load the program at address 10000h
        dq 1                                  ; Starting block number

align 8
descriptor_table_start:
null_descriptor:
        dq 0
code_descriptor:
        dw 0FFFFh
        dw 0000h
        db 00h
        db 10011010b
        db 11001111b
        db 00h
data_descriptor:
        dw 0FFFFh
        dw 0000h
        db 00h
        db 10010010b
        db 11001111b
        db 00h
mode16__sutdown_code_descriptor:
        dw 0FFFFh
        dw 0000h
        db 01h
        db 10011010b
        db 00000000b
        db 00h
mode16_shutdown_data_descriptor:
        dw 0FFFFh
        dw 0000h
        db 01h
        db 10010010b
        db 00000000b
        db 00h
descriptor_table_end:


        ;   Promotion parameters layout
        ; Offset    Type            name
        ; 0         uint32          framebuffer address
        ; 4         uint32          width
        ; 8         uint32          height
        ; 12        uint32          depth
promotion_params_start:
framebuffer_address:    dd 0
width:                  dd 0
height:                 dd 0
depth:                  dd 0
bytes_per_line:         dd 0
promotion_params_end:


align 8
gdtr_contents:
        dw descriptor_table_end - descriptor_table_start - 1
        dd descriptor_table_start

        code_descriptor_offset equ code_descriptor - descriptor_table_start
        data_descriptor_offset equ data_descriptor - descriptor_table_start

        sectors_to_load equ 128

        kernel_load_address equ 10000h

align 8
video_mode_info_buffer:




        times 0x1b4 - ($ - $$) db 0
        ; db 55h, 0AAh

        ; 0x1b4
        db "12345678", 0x0, 0x0     ; 10 byte unique id

        ; 0x1be         ; Partition 1 -- create one big partition that spans the whole disk (2880 sectors, 1.44mb)
        db 0x80         ; boot indicator flag = on

        ; start sector
        db 0            ; starting head = 0
        db 0b00000001   ; cyilinder = 0, sector = 1 (2 cylinder high bits, and sector. 00 000001 = high bits db 0x00)
        db 0            ; 7-0 bits of cylinder (insgesamt 9 bits) 

        ; filesystem type
        db 1            ; filesystem type = fat12

        ; end sector = 2880th sector (because a floppy disk is 1.44mb)
        db 1            ; ending head = 1
        db 18           ; cyilinder = 79, sector = 18 (2 cylinder high bits, and sector. 00 000001 = high bits db 0x00)
        db 79           ; 7-0 bits of cylinder (insgesamt 9 bits) 

        dd 0            ; 32 bit value of number of sectors between MBR and partition
        dd 2880         ; 32 bit value of total number of sectors

        ; 0x1ce         ; Partition 2
        times 16 db 0

        ; 0x1de         ; Partition 3
        times 16 db 0

        ; 0x1ee         ; Parititon 4
        times 16 db 0

        ; 0x1fe         ; Signature
        dw  0xAA55



        ; times 510 - ($ - $$) db 0
        ; db 55h, 0AAh

        ; [7C00 -- 7DFF] -- Bootloader and GDT
        ; [10000 -- 1FFFF] -- Kernel
        ; 20000 -- Base of the IDT
        ; 80000 -- Base of the stack (stack should be larger)
        ; [> 100000] -- Heap
