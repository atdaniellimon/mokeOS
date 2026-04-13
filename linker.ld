ENTRY(_start)
SECTIONS
{
    . = 1M;

    .text : AT(ADDR(.text))
    {
        KEEP(*(.multiboot)) 
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss) }
}