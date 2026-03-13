#include "shell.h"
#include "../drivers/screen/screen.h"
#include "../arch/i386/io.h"
#include "string.h"

typedef unsigned int   uint32_t;
typedef unsigned long long uint64_t;

struct mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

int showShellText = 1;
int cat_commands = 0;
int shell_initialized = 0;
void* global_mbi_ptr;

void intO_string(unsigned int n, char* buffer) {
    char temp[12];
    int i = 0, j = 0;

    if (n == 0) {
        buffer[j++] = '0';
        buffer[j] = '\0';
        return;
    }

    while (n > 0) {
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }

    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}

void get_ram(void* mbi_ptr, char* buffer_res) {
    unsigned int flags = *(unsigned int*)mbi_ptr;
    unsigned long long total_bytes = 0;

    if (flags & (1 << 6)) {
        unsigned int mmap_len  = *(unsigned int*)(mbi_ptr + 44);
        unsigned int mmap_addr = *(unsigned int*)(mbi_ptr + 48);
        struct mmap_entry* entry = (struct mmap_entry*)mmap_addr;

        while ((unsigned int)entry < (mmap_addr + mmap_len)) {
            if (entry->type == 1) {
                total_bytes += entry->len;
            }
            entry = (struct mmap_entry*)((unsigned int)entry + entry->size + 4);
        }
    }

    unsigned int total_mb = (unsigned int)(total_bytes / 1048576);
    intO_string(total_mb, buffer_res);
}

void power(char* options){
    if(sameas(options, "reboot")){
        unsigned char temp;
    
        do {
            temp = inb(0x64);
            if (temp & 1) inb(0x60);
        } while (temp & 2);

        outb(0x64, 0xFE);
    } else if(sameas(options, "off")){
        outw(0x604, 0x2000);
        outw(0x4004, 0x3400);
        outw(0xB004, 0x2000);
    }
}

char* get_argument(char* full_command) {
    int i = 0;
    while (full_command[i] != '\0') {
        if (full_command[i] == ' ') {
            return &full_command[i + 1];
        }
        i++;
    }
    return 0;
}

void exec_command(char* command){
    if(sameas(command, "clear")){
        clean_screen();
    } else if(sameas(command, "neofetch")){
        char ram_info[32];
        get_ram(global_mbi_ptr, ram_info);

        set_colour(0x09);
        k_print("\xDC              \xDC\xDC          \n");
        k_print(" \xDB\xDB\xDB\xDC\xDB\xDB\xDB\xDC \xDC\xDB\xDB\xDB\xDC \xDB\xDB \xDC\xDB\xDF \xDC\xDB\xDF\xDB\xDC \n");
        k_print(" \xDB\xDB \xDB\xDB \xDB\xDB \xDB\xDB \xDB\xDB \xDB\xDB\xDB\xDB   \xDB\xDB\xDC\xDB\xDF \n");
        k_print("\xDC\xDB\xDB \xDB\xDB \xDF\xDB\xDC\xDF\xDB\xDB\xDB\xDF\xDC\xDB\xDB \xDF\xDB\xDC\xDC\xDF\xDB\xDC\xDC\xDC \n \n");
        set_colour(0x0F);
        k_print(" OS: mokeOS\n");
        k_print(" Kernel: x86\n");
        k_print(" Memory: "); 
        k_print(ram_info);
        k_print("MB \n");
        k_print(" Version: Nebula Beta\n");
    } else if(sameas(command, "reboot")){
        k_print("Preparing for reboot.");
        power("reboot");
    } else if(sameas(command, "halt")){
        k_print("System returned with 0 code.");
        asm volatile("hlt"); 
    } else if (sameas(command, "colour")) {
        char* arg = get_argument(command);
        
        if (arg == 0) {
            k_print("Usage: colour <name>\n");
        } else if (sameas(arg, "red")) {
            set_colour(0x04);
            custom_colour = 0x04;
        } else if (sameas(arg, "blue")) {
            set_colour(0x09);
            custom_colour = 0x09;
        } else if (sameas(arg, "green")) {
            set_colour(0x0A);
            custom_colour = 0x0A;
        } else if(sameas(arg, "white")){
            set_colour(0);
            custom_colour = 0x0F;
        } else {
            k_print("Unknown colour.\n");
        }
    } else if(sameas(command, "help")){
            set_colour(0x09);
            k_print("clear: ");
            set_colour(0);
            k_print("Clears screen content\n");

            set_colour(0x09);
            k_print("halt: ");
            set_colour(0);
            k_print("Freezes CPU\n");

            set_colour(0x09);
            k_print("reboot: ");
            set_colour(0);
            k_print("Reboots system\n");

            set_colour(0x09);
            k_print("poweroff: ");
            set_colour(0);
            k_print("Shuts down this Mokebook\n");

            set_colour(0x09);
            k_print("neofetch: ");
            set_colour(0);
            k_print("Displays PC info\n");

            set_colour(0x09);
            k_print("colour: ");
            set_colour(0);
            k_print("Change shell's text colour\n");

            set_colour(0x09);
            k_print("echo <args> / <on/off>: ");
            set_colour(0);
            k_print("Shows shell text or hides shell default text\n");
        } else if(sameas(command, "echo")){
            char* arg = get_argument(command);
            if(sameas(arg, "off")){
                k_print("\n");
                showShellText = 0;
                set_colour(custom_colour);
                return;
            } else if(sameas(arg, "on")){
                k_print("\n");
                showShellText = 1;

                set_colour(0x09);
                k_print("moke-os> ");
                set_colour(custom_colour);
                return;
            }
            if(arg != 0){
                k_print(arg);
            }
            k_print("\n");
        } else if(sameas(command, "poweroff")){
            k_print("Preparing for shut down. \n");
            power("off");
        } else if(sameas(command, "exit")){
            shell_initialized = 0;
            k_print("Exited with code 0; \n");
            k_print("Commands line killed. \n");
        } else if(sameas(command, "nano")){
            clean_screen();
            k_print("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB NANO \xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB");
            showShellText = 0;
            shell_initialized = 0;
            cat_commands = 1;
        } else {
        k_print("Command not found: ");
        k_print(command);
        k_print("\n");
    }
    if(showShellText == 1){
        set_colour(0x09);
        k_print("moke-os> ");
    }
    set_colour(custom_colour);
}

void exec_cat_command(char* command){
    if(sameas(command, ":wq")){
        char* arg = get_argument(command);

        clean_screen();
        cat_commands = 0;
        shell_initialized = 1;
        showShellText = 1;
        set_colour(0);
        if(!arg){
            k_print("A file name needs to be specified. Changes have been deleted\n");
        } else {
            k_print("File succesfully saved as: ");
            k_print(arg);
            k_print("\n");
        }
        
        
        set_colour(0x09);
        k_print("moke-os> ");
        set_colour(custom_colour);
    } else if(sameas(command, ":q")){
        clean_screen();
        cat_commands = 0;
        shell_initialized = 1;
        showShellText = 1;

        set_colour(0x09);
        k_print("moke-os> ");
        set_colour(0);
    }
}