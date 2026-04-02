#include "lib/string/string.h"
#include "drivers/screen/screen.h"
#include "arch/i386/io.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "drivers/keyboard/keyboard.h"
#include "lib/shell/shell.h"
#include "drivers/mouse/ps2.h"
#include "lib/malloc/mem.h"
#include "lib/timer/timer.h"
#include "debug/mfs/moke.h"
#include "lib/syscall/syscall.h"
#include "drivers/vbe/vbe.h"

void disable_bios_cursor(){
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void main(void* mbi, unsigned int magic){
    (void)magic;
    global_mbi_ptr = mbi;
    
    asm volatile("cli");
    heap_init();
    gdt_init();
    ata_init();
    mfs_mount();
    idt_init();
    disable_bios_cursor();
    vbe_init(mbi);
    timer_init(1000); 
    mouse_init(); 
    keyboard_init();
    asm volatile("sti"); 

    start_shell();

    for(;;){ asm volatile("hlt"); }
}