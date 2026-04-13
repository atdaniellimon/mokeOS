#ifndef KEYBOARD_H
#define KEYBOARD_H

extern unsigned char keyboard_map[128];
extern char* keyboard_buffer;
extern int buffer_idx;
extern int buffer_size;
extern char* raw_input_stream;

typedef void (*keyboard_callback_t)(unsigned char key);
void keyboard_set_handler(keyboard_callback_t handler);

void keyboard_handler();
void keyboard_init();
void keyboard_push(char* character);
void reset_keyboard_input();
void keyboard_splice();

#endif