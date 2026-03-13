#ifndef SCREEN_H
#define SCREEN_H

extern char *video_mem;
extern int screen_byte;
extern char custom_colour;
extern char current_colour;


void clean_screen();
void k_print(char *content);
void set_colour(char colour);

#endif