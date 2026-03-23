#ifndef SCREEN_H
#define SCREEN_H
#include "../../lib/stdint/types.h"

extern uint32_t current_colour;
extern uint32_t custom_colour;
extern int cursor_x;
extern int cursor_y;
extern int background_colour;

void k_print(char* content);
void k_print_at(char *mensaje, int x, int y);
void scroll();
void clean_screen();
void set_colour(unsigned char colour);

#endif