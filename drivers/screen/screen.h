#ifndef SCREEN_H
#define SCREEN_H

extern char *video_mem;
extern char current_colour;
extern char custom_colour;
extern int screen_byte;

void k_print(char* content);
void k_print_at(char *mensaje, int x, int y);
void scroll();
void clean_screen();
void set_colour(char colour);

#endif