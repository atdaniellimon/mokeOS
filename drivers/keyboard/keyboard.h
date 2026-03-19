#ifndef KEYBOARD_H
#define KEYBOARD_H

extern char keyboard_map[128];
extern char keyboard_buffer[1024];
extern int buffer_idx;

void keyboard_handler();

#endif