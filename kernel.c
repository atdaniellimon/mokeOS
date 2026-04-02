#include "lib/string.h"
#include "drivers/screen/screen.h"
#include "arch/i386/io.h"
#include "drivers/keyboard/keyboard.h"
<<<<<<< Updated upstream
#include "lib/shell.h"
=======
#include "drivers/mouse/ps2.h"
#include "lib/malloc/mem.h"
#include "debug/mfs/moke.h"
#include "lib/syscall/syscall.h"

void jump_to_usermode(uint32_t entry, uint32_t stack);
>>>>>>> Stashed changes

void disable_bios_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void init_terminal(){
    clean_screen();
    set_colour(0x09);
    showShellText = 0;
    exec_command("neofetch");
    set_colour(0);
    k_print("\n Type 'help' to get a list of available commands. \n");
    set_colour(0x09);
    showShellText = 1;
    k_print("moke-os> ");
    set_colour(0);
    shell_initialized = 1;
    while(1){
        check_key();
    }
}

void main(void* mbi){
    global_mbi_ptr = mbi;

    clean_screen();
    disable_bios_cursor();
<<<<<<< Updated upstream
    init_terminal();
}
//qemu-system-i386 -kernel mokeos.bin
=======
    syscall_init();
    vbe_init(mbi);
    timer_init(1000); 
    mouse_init(); 
    keyboard_init();
    asm volatile("sti"); 

    start_shell();

    for(;;){ asm volatile("hlt"); }
}
>>>>>>> Stashed changes
