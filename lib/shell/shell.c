/*
ShimmerOS Beta - Do not copy
*/
#include "shell.h"
#include "../../drivers/screen/screen.h"
#include "../string/string.h"
#include "../../arch/i386/io.h"
#include "../../drivers/hardware/hardware.h"
#include "../timer/timer.h"
#include "../date/date.h"

#define current_user "tester"

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

char* next_arg(char* str){
    int i = 0;
    while(str[i] >= '0' && str[i] <= '9') i++;
    if(str[i] != '\0') i++;
    return &str[i];
}

void print_ram(void* mbi){
    char ram_info[32];
    get_ram(mbi, ram_info); 
    
    k_print(ram_info);
}

void tick_command(char* arg) {
    int ticks = get_timer_ticks(); 
    char buf[32]; 

    if (arg == 0 || sameas(arg, "-r")) { 
        into_string(ticks, buf); 
        k_print("Raw ticks: "); 
        k_print(buf); 
        k_print("\n"); 
    }
    else if (sameas(arg, "-s")) {
        int sec = ticks / 1000; 
        
        into_string(sec, buf); 
        k_print("Uptime (seconds): "); 
        k_print(buf);
        k_print("\n");
    }
    else if (sameas(arg, "-ms")) {
        into_string(ticks, buf); 
        k_print("Uptime (ms): "); 
        k_print(buf);
        k_print("\n");
    }
    else if (sameas(arg, "-m")) {
        int min = ticks / (1000 * 60); 
        into_string(min, buf); 
        k_print("Uptime (minutes): "); 
        k_print(buf);
        k_print("\n");
    }
    else if (sameas(arg, "-h")) {
        int hr = ticks / (1000 * 60 * 60); 
        into_string(hr, buf); 
        k_print("Uptime (hours): "); 
        k_print(buf);
        k_print("\n");
    }
    else if (sameas(arg, "-mem")) {
        char ram_info[32]; 
        get_ram(global_mbi_ptr, ram_info); 
        k_print("RAM total: "); 
        k_print(ram_info); 
        k_print("MB\n"); 
    }
    else if (sameas(arg, "-user")) {
        k_print("Current user: "); 
        k_print(current_user); 
        k_print("\n"); 
    }
    else if (sameas(arg, "-mode")) {
    char colour_buf[32]; 
    into_string(custom_colour, colour_buf); 
    k_print("Current text colour code: "); 
    k_print(colour_buf); 
    k_print("\n"); 
    }
else {
        k_print("Unknown flag. Available flags: -r -s -ms -m -h -mem -user -mode\n"); 
}    
}

void print_Shimmer_logo(){
    set_colour(0x01);
    k_print("                         %                        \n");
    k_print("                         %                        \n");
    k_print("                         %                        \n");
    k_print("                         %                        \n");
    k_print("                         %,   /%%%%(              \n");
    k_print("                   %%%%% %%         .%%%          \n");
    k_print("            %%.%%        %%              % %      \n");
    k_print("        %% (%           %%%               (  *    \n");
    k_print("    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   \n");
    k_print("      %  %              %%%           %. %*       \n");
    k_print("       (% %       %      %%       %%%%            \n");
    k_print("            .%%   %      %%%%%.                   \n");
    k_print("                  %      %.                       \n");
    k_print("                  #      %                        \n");
    k_print("                         %                        \n");
    k_print("                         %                        \n");
    set_colour(custom_colour);
}

void change_time(int h, int m, int s){
    set_time(h, m, s);
}
void change_date(int day, int month, int year){
    set_date(day, month, year);
}

void exec_command(char* command){
    if(sameas(command, "clear")){
        clean_screen();
    } else if(sameas(command, "sysfetch")){
        set_colour(0x01);
        k_print("\n                         %                        ");
        set_colour(0x0F);
        k_print(" OS: ShimmerOS\n");
        set_colour(0x01);
        k_print("                         %                        ");
        set_colour(0x0F);
        k_print(" Kernel: x86\n");
        set_colour(0x01);
        k_print("                         %                        ");
        set_colour(0x0F);
        k_print(" Memory: "); 
        print_ram(global_mbi_ptr);
        k_print("MB \n");
        set_colour(0x01);
        k_print("                         %                        ");
        set_colour(0x0F);
        k_print(" Version: Nebula Beta\n");
        set_colour(0x01);
        k_print("                         %,   /%%%%(              ");
        set_colour(0x0F);
        k_print(" Kernel: 1.0\n");
        set_colour(0x01);
        k_print("                   %%%%% %%         .%%%          \n");
        k_print("            %%.%%        %%              % %      \n");
        k_print("        %% (%           %%%               (  *    \n");
        k_print("    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   \n");
        k_print("      %  %              %%%           %. %*       \n");
        k_print("       (% %       %      %%       %%%%            \n");
        k_print("            .%%   %      %%%%%.                   \n");
        k_print("                  %      %.                       \n");
        k_print("                  #      %                        \n");
        k_print("                         %                        \n");
        k_print("                         %                        ");
        set_colour(0x00);
        k_print(" \xDB\xDB");
        set_colour(0x01);
        k_print("\xDB\xDB");
        set_colour(0x02);
        k_print("\xDB\xDB");
        set_colour(0x03);
        k_print("\xDB\xDB");
        set_colour(0x04);
        k_print("\xDB\xDB");
        set_colour(0x05);
        k_print("\xDB\xDB");
        set_colour(0x06);
        k_print("\xDB\xDB");
        set_colour(0x07);
        k_print("\xDB\xDB");
        set_colour(0x0A);
        k_print("\xDB\xDB");
        set_colour(0x0B);
        k_print("\xDB\xDB");
        set_colour(0x0C);
        k_print("\xDB\xDB");
        set_colour(0x0D);
        k_print("\xDB\xDB\n");
        set_colour(0x0F);
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
        print_Shimmer_logo();

        set_colour(0x05);
        k_print(" Creator: ");
        set_colour(0);
        k_print("Daniel Limon (nomil)\n");

        set_colour(0x05);
        k_print(" littleghost09: ");
        set_colour(0);
        k_print("As ShimmerOS' number one supporter and contributor in the development\n");
    } else if(sameas(command, "halt")){
        k_print("System returned with 0 code.");
        shell_initialized = 0;
        cat_commands = 0;
        showShellText = 0;

        sleep(500);
        asm volatile("hlt"); 
    } else if(sameas(command, "colour")){
        char* arg = get_argument(command);
        
        if (arg == 0) {
            k_print("Usage: colour <name>\n");
        } else if (sameas(arg, "red")) {
            set_colour(0x04);
            custom_colour = 0x04;
        } else if (sameas(arg, "blue")) {
            set_colour(0x01);
            custom_colour = 0x01;
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
            set_colour(0x05);
            k_print(" clear: "); set_colour(0); k_print("   Clears screen content\n");

            set_colour(0x05);
            k_print(" halt: "); set_colour(0); k_print("    Freezes CPU\n");

            set_colour(0x05);
            k_print(" reboot: "); set_colour(0); k_print("  Reboots system\n");

            set_colour(0x05);
            k_print(" poweroff: "); set_colour(0); k_print("Shuts down this Shimmerbook\n");

            set_colour(0x05);
            k_print(" sysfetch: "); set_colour(0); k_print("Displays PC info\n");

            set_colour(0x05);
            k_print(" colour: "); set_colour(0); k_print("  Change shell's text colour\n");

            set_colour(0x05);
            k_print(" echo: "); set_colour(0); k_print("    Shows shell text or hides shell default text\n");

            set_colour(0x05);
            k_print(" nano: "); set_colour(0); k_print("    Edits a file content (just visually)\n");

            set_colour(0x05);
            k_print(" uptime: "); set_colour(0); k_print("  Shows system uptime in seconds\n");

            set_colour(0x05);
            k_print(" about: "); set_colour(0); k_print("   Shows things about developer\n");

            set_colour(0x05);
            k_print(" whoami: "); set_colour(0); k_print("  Shows current user\n");

            set_colour(0x05);
            k_print(" settime: "); set_colour(0); k_print(" Changes system's RTC time\n");

            set_colour(0x05);
            k_print(" setdate: "); set_colour(0); k_print(" Changes system's RTC date\n");

            set_colour(0x05);
            k_print(" ticks: "); set_colour(0); k_print("   For debugging\n");
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

                set_colour(0x05);
                k_print("Shimmer-os> ");
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
        } else if(sameas(command, "settime")){
            char* arg = get_argument(command);
            if(arg == 0){
                k_print("Usage: settime HH MM SS\n");
            } else {
                int h = toint(arg);
                arg = next_arg(arg);
                int m = toint(arg);
                arg = next_arg(arg);
                int s = toint(arg);
                set_time(h, m, s);
                k_print("Time updated!\n");
            }
        } else if(sameas(command, "setdate")){
            char* arg = get_argument(command);
            if(arg == 0){
                k_print("Usage: settime Day Month Year\n");
            } else {
                int d = toint(arg);
                arg = next_arg(arg);
                int m = toint(arg);
                arg = next_arg(arg);
                int y = toint(arg);
                set_date(d, m, y);
                k_print("Date updated!\n");
            }
        } else if(sameas(command, "uptime")){
            char buffer[12];
            int system_uptime = get_timer_ticks() / 1000;
            into_string(system_uptime, buffer);
            
            set_colour(0x05);
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
        } else if(sameas(command, "ticks")){
            char* arg = get_argument(command);

            if(arg == 0){
                k_print("Unknown flag. Available flags: -r -s -ms -m -h -mem -user -mode\n"); 
            } else {
                tick_command(arg);
            }
        } else {
            k_print("Unknown command: ");
            k_print(command);
            k_print("\n");
        }
        if(showShellText == 1){
            set_colour(0x05);
            k_print("Shimmer-os> ");
        }
        set_colour(custom_colour);
}

void exec_cat_command(char* command){
    if(sameas(command, "!wq")){
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
        
        
        set_colour(0x05);
        k_print("Shimmer-os> ");
        set_colour(custom_colour);
    } else if(sameas(command, "!q")){
        clean_screen();
        cat_commands = 0;
        shell_initialized = 1;
        showShellText = 1;

        k_print("Changes have been deleted \n");
        set_colour(0x05);
        k_print("Shimmer-os> ");
        set_colour(custom_colour);
    } else if(sameas(command, ":w:")){
        //k_print_at("Changes have been writed", 0, 20);
    }
}

void init_shell(){
    clean_screen();

    set_colour(0x05);
    showShellText = 0;
    exec_command("sysfetch");

    set_colour(0);
    k_print("\n");
    set_colour(0x05);

    showShellText = 1;

    k_print("Shimmer-os> ");
    set_colour(0);

    shell_initialized = 1;
}



/* it should just work by typing ticks -r or ticks -user or something like that*/
/* we've got to implement it on commands list for it to work! */