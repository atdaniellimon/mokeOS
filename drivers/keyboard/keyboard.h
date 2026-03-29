#ifndef KEYBOARD_H
#define KEYBOARD_H

extern char keyboard_map[128];
extern char* keyboard_buffer;
extern int buffer_idx;
extern int buffer_size;

void keyboard_handler();
void keyboard_init();

#endif