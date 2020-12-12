export C_INCLUDE_PATH='./'


MAIN_FILES='main.c 
            drivers/display.c 
            misc/string.c 
            graphics/rectangle.c 
            misc/mem.c 
            misc/io.c 
            misc/allocator.c 
            misc/circqueue.c 
            misc/resource-manager.c 
            graphics/bmp-reader.c 
            graphics/psf1-reader.c 
            graphics/command-line.c '
	        # graphics/text-editor.c '


MAIN_COMPILE_FLAGS='-nostdlib 
                    -m32 
                    -fno-stack-protector 
                    -mno-red-zone 
                    -ffreestanding 
                    -O3 
                    -funroll-loops    
                    -march=i386 
                    -fwide-exec-charset=UCS2 
                    -fshort-wchar '

                    
GEN_REGS_ONLY_FILES='boot/interrupts.c 
                     drivers/keyboard.c '




nasm -f bin boot/bootloader.asm -o bootloader.bin
nasm -f elf32 boot/start.asm -o start.o

gcc -c $MAIN_COMPILE_FLAGS -mgeneral-regs-only -fno-toplevel-reorder drivers/disk.c -o disk.o
gcc -c $MAIN_COMPILE_FLAGS -mgeneral-regs-only -fno-toplevel-reorder boot/interrupts.c -o enable-ints.o
gcc -c $MAIN_COMPILE_FLAGS -mgeneral-regs-only -fno-toplevel-reorder drivers/keyboard.c -o keyboard.o
gcc -c $MAIN_COMPILE_FLAGS -mgeneral-regs-only -fno-toplevel-reorder drivers/timer.c -o timer.o

gcc -c $MAIN_COMPILE_FLAGS graphics/text-editor.c -o text-editor.o

# --noinhibit-exec is needed because of this: https://sourceware.org/bugzilla/show_bug.cgi?id=25585
gcc -Wl,--noinhibit-exec -T linker.ld -flto  $MAIN_COMPILE_FLAGS start.o enable-ints.o keyboard.o timer.o disk.o text-editor.o $MAIN_FILES -o image.elf

objcopy -O binary --pad-to 0x10000 image.elf image.bin

(cd ./resource; ./gen-res.py)

cat bootloader.bin image.bin ./resource/res-drive.img > drive.img
