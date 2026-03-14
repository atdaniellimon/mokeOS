#include "keyboard.h"
#include "../../lib/shell/shell.h"
#include "../screen/screen.h"
#include "../../arch/i386/io.h"

char keyboard_map[128] = {
0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,
'\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
'a','s','d','f','g','h','j','k','l',';','\'','`',0,
'\\','z','x','c','v','b','n','m',',','.','/',':',
'*',0,' '
};

char keyboard_buffer[256];
int buffer_idx = 0;

void check_key(){
    unsigned char scancode = inb(0x60);
    static unsigned char key_states[128];

    if(scancode & 0x80){
        key_states[scancode & 0x7F] = 0;
        return;
    }

    if(key_states[scancode] == 0){
        char c = keyboard_map[scancode];
        
        if(c){
            if(c == 8 && screen_byte > 0){
                  if(buffer_idx > 0){
                    screen_byte -= 2;
                    video_mem[screen_byte] = ' ';
                    video_mem[screen_byte + 1] = current_colour;
                    buffer_idx--;
				  }				
            } else if(c == '\n'){
                keyboard_buffer[buffer_idx] = '\0';
                k_print("\n");
                
                if(shell_initialized == 1 && cat_commands == 0){
                    exec_command(keyboard_buffer);
                } else if(cat_commands == 1 && shell_initialized == 0){
                    exec_cat_command(keyboard_buffer);
                } else {
                    k_print("Kernel panic");
                }
                
                buffer_idx = 0;
            } else if(buffer_idx < 256 - 1){
                char str[2] = {c, '\0'};
                k_print(str);
                
                keyboard_buffer[buffer_idx] = c;
                buffer_idx++;
            }
        }
        key_states[scancode] = 1;
    }
}