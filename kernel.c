/*
    mokeOS Kernel
    Version Beta
    Made by Daniel Limon for MokeOS

    See more atdaniellimon.github.io
*/

#include "drivers/keyboard/keyboard.h"
#include "drivers/mouse/ps2.h"
#include "drivers/vbe/vbe.h"

#include "lib/kernel/syscalls.h"
#include "lib/string/string.h"
#include "lib/timer/timer.h"
#include "lib/shell/shell.h"
#include "lib/malloc/mem.h"

#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/io.h"

#include "debug/mfs/moke.h"

extern void jump_to_usermode();

/*
    This function disables the standard "VESA" graphics mode
    blinking cursor,  leaving  the screen clear for the user.
*/
void disable_bios_cursor(){
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

/*
    This function is called by boot.s, ensuring a clear boot
    is   being   executed   with   its   respective  drivers.
*/
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

    jump_to_usermode(); 

    while(1);
}