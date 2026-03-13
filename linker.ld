ENTRY(_start)
SECTIONS
{
    . = 1M; /* El kernel empieza en 1MB */

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot) /* Forzamos el header al inicio */
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K) { *(.rodata) }
    .data BLOCK(4K) : ALIGN(4K)  { *(.data) }
    .bss BLOCK(4K) : ALIGN(4K)   { *(.COMMON) *(.bss) }
}