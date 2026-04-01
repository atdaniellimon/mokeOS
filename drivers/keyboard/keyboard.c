#include "keyboard.h"
#include "../../arch/i386/io.h"
#include "../../lib/string/string.h"
#include "../../lib/malloc/mem.h"
#include "../../lib/stdint/types.h"

#define key_esc 27

unsigned char keyboard_map[128] = {
    0, key_esc, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0x3A,
    '!', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ':',
    '*' ,0, ' '
};
unsigned char shift_keyboard_map[128] = {
    0, key_esc, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '"', ':', '~', 0x3A,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 
    '*', 0, ' '
};

char* keyboard_buffer = 0;
char* raw_input_stream = 0;
int buffer_size = 256;
int buffer_idx = 0;
int isShift = 0;
static keyboard_callback_t _shell_handler = 0;


void keyboard_splice(){
    int len = strlen(raw_input_stream);
    if(len > 0){
        raw_input_stream[len - 1] = '\0';
    }
}

void keyboard_set_handler(keyboard_callback_t callback){
    _shell_handler = callback;
}

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

    uintptr_t c = (isShift) ? shift_keyboard_map[scancode] : keyboard_map[scancode];

    if(c){
        if(c == 0x3A){
            isShift = !isShift;
        } else if(c == 8){
            if(buffer_idx > 0){
                keyboard_buffer[buffer_idx] = '\0';
                buffer_idx--;
                keyboard_splice();
            }      
        } else if(c == '\n'){
            keyboard_buffer[buffer_idx] = '\0';
            keyboard_push("\n");
            buffer_idx = 0;
        } else if(buffer_idx < buffer_size - 1){
            // if full, grow
            if(buffer_idx >= buffer_size - 2){
                char* new_buffer = (char*)kmalloc(buffer_size * 2);
                // copy previous content
                for(int i = 0; i < buffer_idx; i++){
                    new_buffer[i] = keyboard_buffer[i];
                }
                kfree(keyboard_buffer);
                keyboard_buffer = new_buffer;
                buffer_size *= 2;
            }
            char str[2] = {c, '\0'};
            keyboard_push(str);
            keyboard_buffer[buffer_idx] = c;
            buffer_idx++;
        }
    }
    if(_shell_handler){
        _shell_handler((char)c);
    }

    key_states[scancode] = 1;
    outb(0x20, 0x20);
}

void keyboard_init(){
    keyboard_buffer = (char*)kmalloc(buffer_size);
    raw_input_stream = (char*)kmalloc(buffer_size);
    raw_input_stream[0] = '\0'; 
}

void keyboard_push(char* character){
    int len = strlen(raw_input_stream);
    if(len < buffer_size - 1){
        raw_input_stream[len] = character[0];
        raw_input_stream[len+1] = '\0';
    }
}

void reset_keyboard_input(){
    if(raw_input_stream != 0){
        raw_input_stream[0] = '\0';
    }
}