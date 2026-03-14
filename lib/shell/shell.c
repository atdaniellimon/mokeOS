#include "shell.h"
#include "../../drivers/screen/screen.h"
#include "../string/string.h"
#include "../../arch/i386/io.h"
#include "../../drivers/hardware/hardware.h"
#include "../timer/timer.h"
#include "../date/date.h"

#define current_user "reddit viewers"

int showShellText = 1;
int cat_commands = 0;
int shell_initialized = 0;

void* global_mbi_ptr;

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

char* get_argument(char* full_command){
    int i = 0;
    while (full_command[i] != '\0') {
        if (full_command[i] == ' ') {
            return &full_command[i + 1];
        }
        i++;
    }
    return 0;
}

void print_ram(void* mbi){
    char ram_info[32];
    get_ram(mbi, ram_info); 
    k_print(ram_info);
}

void print_moke_logo(){
    set_colour(0x09);
    k_print("\xDC              \xDC\xDC          \n");
    k_print(" \xDB\xDB\xDB\xDC\xDB\xDB\xDB\xDC \xDC\xDB\xDB\xDB\xDC \xDB\xDB \xDC\xDB\xDF \xDC\xDB\xDF\xDB\xDC \n");
    k_print(" \xDB\xDB \xDB\xDB \xDB\xDB \xDB\xDB \xDB\xDB \xDB\xDB\xDB\xDB   \xDB\xDB\xDC\xDB\xDF \n");
    k_print("\xDC\xDB\xDB \xDB\xDB \xDF\xDB\xDC\xDF\xDB\xDB\xDB\xDF\xDC\xDB\xDB \xDF\xDB\xDC\xDC\xDF\xDB\xDC\xDC\xDC \n \n");
    set_colour(custom_colour);
}

void exec_command(char* command){
    if(sameas(command, "clear")){
        clean_screen();
    } else if(sameas(command, "neofetch")){
        print_moke_logo();
        set_colour(0x0F);
        k_print(" OS: mokeOS\n");
        k_print(" Kernel: x86\n");
        k_print(" Memory: "); 
        print_ram(global_mbi_ptr);
        k_print("MB \n");
        k_print(" Version: Nebula Beta\n"); 
    } else if(sameas(command, "reboot")){
        k_print("Preparing for reboot.");
        sleep(500);
        power("reboot");
    } else if(sameas(command, "whoami")){
        k_print(current_user);
        k_print("\n");
    } else if(sameas(command, "date")){
        char buf[12];
        get_date();
        
        into_string(hours, buf);   
        k_print(buf); 
        k_print(":");

        into_string(minutes, buf); 
        k_print(buf); 
        k_print(":");

        into_string(seconds, buf); 
        k_print(buf); 
        k_print(" ");
        
        into_string(day, buf);   
        k_print(buf); 
        k_print("/");

        into_string(month, buf); 
        k_print(buf); 
        k_print("/");

        into_string(year, buf);  
        k_print(buf);

        k_print("\n");
    } else if(sameas(command, "about")){
        print_moke_logo();

        set_colour(0x09);
        k_print(" Creator: ");
        set_colour(0);
        k_print("Daniel Limon (nomil)\n");

    } else if(sameas(command, "halt")){
        k_print("System returned with 0 code.");
        asm volatile("hlt"); 
    } else if(sameas(command, "colour")){
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
            k_print(" clear: ");
            set_colour(0);
            k_print("Clears screen content\n");

            set_colour(0x09);
            k_print(" halt: ");
            set_colour(0);
            k_print("Freezes CPU\n");

            set_colour(0x09);
            k_print(" reboot: ");
            set_colour(0);
            k_print("Reboots system\n");

            set_colour(0x09);
            k_print(" poweroff: ");
            set_colour(0);
            k_print("Shuts down this Mokebook\n");

            set_colour(0x09);
            k_print(" neofetch: ");
            set_colour(0);
            k_print("Displays PC info\n");

            set_colour(0x09);
            k_print(" colour: ");
            set_colour(0);
            k_print("Change shell's text colour\n");

            set_colour(0x09);
            k_print(" echo: ");
            set_colour(0);
            k_print("Shows shell text or hides shell default text\n");

            set_colour(0x09);
            k_print(" nano: ");
            set_colour(0);
            k_print("edits a file content (just visually)\n");

            set_colour(0x09);
            k_print(" uptime: ");
            set_colour(0);
            k_print("Shows system uptime in seconds\n");

            set_colour(0x09);
            k_print(" about: ");
            set_colour(0);
            k_print("Shows things about developer\n");

            set_colour(0x09);
            k_print(" whoami: ");
            set_colour(0);
            k_print("Shows current user\n");
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
            sleep(500);
            power("off");
        } else if(sameas(command, "uptime")){
            char buffer[12];
            int system_uptime = get_timer_ticks() / 1000;
            into_string(system_uptime, buffer);
            
            set_colour(0x09);
            k_print(" Uptime: ");
            set_colour(0);
            k_print(buffer);
            k_print(" seconds\n");
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

void init_shell(){
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
}