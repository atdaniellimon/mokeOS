#include "arch/i386/io.h"
#include "lib/shell/shell.h"

void disable_bios_cursor(){
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void main(void* mbi){
    global_mbi_ptr = mbi;
    disable_bios_cursor();
    init_shell();

    for(;;) { asm volatile("hlt"); }
}
//qemu-system-i386 -kernel mokeos.bin