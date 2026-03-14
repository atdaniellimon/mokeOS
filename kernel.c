#include "arch/i386/io.h"
#include "lib/shell/shell.h"
#include "arch/i386/idt.h"
#include "lib/timer/timer.h"

void disable_bios_cursor(){
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void main(void* mbi){
    timer_init(1000);
    global_mbi_ptr = mbi;

    disable_bios_cursor();
    idt_init();

    init_shell();
    

    for(;;) { asm volatile("hlt"); }
}