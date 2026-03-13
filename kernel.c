#include "lib/string.h"
#include "drivers/screen/screen.h"
#include "arch/i386/io.h"
#include "drivers/keyboard/keyboard.h"
#include "lib/shell.h"

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
    init_terminal();
}
//qemu-system-i386 -kernel mokeos.bin
