#ifndef VBE_H
#define VBE_H

#include "../../lib/stdint/types.h"

typedef struct {
    uint32_t* addr;
    uint32_t  pitch;
    uint32_t  width;
    uint32_t  height;
    uint8_t   bpp;
} framebuffer_t;

typedef union {
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } channels;
    uint32_t raw;
} mcolor_t;


/*
    Defining the back_buffer to ensure
    flickering does not exist.
*/
extern uint32_t back_buffer[1024 * 768];


extern framebuffer_t fb;

void     vbe_swap();
void     vbe_init(void* mbi);
void     vbe_clear(uint32_t color);
void     put_pixel(int x, int y, uint32_t color_raw);
int      draw_char(int x, int y, char c, uint32_t fg);
uint32_t apply_alpha(uint32_t front_raw, uint32_t back_raw);
void     draw_rect(int x, int y, int w, int h, uint32_t color);
void     draw_string(int x, int y, char* str, uint32_t fg, uint32_t bg);
void     draw_rect_rounded(int x, int y, int w, int h, uint32_t colour, int tl, int tr, int bl, int br);

#endif