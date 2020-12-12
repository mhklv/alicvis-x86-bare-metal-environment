SECTIONS
{
  .text : {
	_FRAMEBUFFER_ADDRESS = .;
	. += 4;
	_FRAMEBUFFER_WIDTH = .;
	. += 4;
	_FRAMEBUFFER_HEIGHT = .;
	. += 4;
	_FRAMEBUFFER_DEPTH = .;
	. += 4;
	_FRAMEBUFFER_BYTES_PER_LINE = .;
	. += 4;
  	*(.text);
  }
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  .got : { *(.got) }
  .bss : { *(.bss) }
}
