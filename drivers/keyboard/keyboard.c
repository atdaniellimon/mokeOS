#include "keyboard.h"
#include "../../lib/shell/shell.h"
#include "../vbe/vbe.h"
#include "../screen/screen.h"
#include "../../arch/i386/io.h"

/*
    Changelog:
        - Added support for shift (Currently caps lock)
*/

char keyboard_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0x3A,
    '!', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ':',
    '*' ,0, ' '
};
char shift_keyboard_map[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '"', ':', '~', 0x3A,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 
    '*', 0, ' '
};

char keyboard_buffer[1024];
int buffer_idx = 0;
int isShift = 0;

void keyboard_handler(){
    unsigned char scancode = inb(0x60);
    static unsigned char key_states[128];

    if(scancode & 0x80){
        key_states[scancode & 0x7F] = 0;
        outb(0x20, 0x20);
        return;
    }

    if(key_states[scancode] != 0){
        outb(0x20, 0x20);
        return;
    }

    char c = (isShift) ? shift_keyboard_map[scancode] : keyboard_map[scancode];

    if(c){
        if(c == 0x3A){
            isShift = !isShift;
        } else if(c == 8){
            if(buffer_idx > 0){
                buffer_idx--;
                cursor_x -= 8;
                if(cursor_x < 0){
                    cursor_x = 0;
                }
                draw_rect(cursor_x, cursor_y, 8, 8, background_colour);
            }      
        } else if(c == '\n'){
            keyboard_buffer[buffer_idx] = '\0';
            k_print("\n");
            if(shell_initialized == 1 && cat_commands == 0){
                exec_command(keyboard_buffer);
            } else if(cat_commands == 1 && shell_initialized == 0){
                exec_cat_command(keyboard_buffer);
            }
            buffer_idx = 0;
        } else if(buffer_idx < 1024 - 1){
            char str[2] = {c, '\0'};
            k_print(str);
            keyboard_buffer[buffer_idx] = c;
            buffer_idx++;
        }
    }

    key_states[scancode] = 1;
    outb(0x20, 0x20);
}