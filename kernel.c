#include "arch/i386/io.h"
#include "lib/shell/shell.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "lib/timer/timer.h"
#include "drivers/vbe/vbe.h"
#include "lib/string/string.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/mouse/ps2.h"

void disable_bios_cursor(){
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void main(void* mbi, unsigned int magic){
    (void)magic;
    global_mbi_ptr = mbi;
    
    asm volatile("cli");

    gdt_init();
    idt_init();
    disable_bios_cursor();
    vbe_init(mbi);
    timer_init(1000); 
    mouse_init();

    asm volatile("sti"); 
    start();

    for(;;){ asm volatile("hlt"); }
}