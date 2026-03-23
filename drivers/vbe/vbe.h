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

void vbe_init(void* mbi);
void put_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void vbe_clear(uint32_t color);
void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void draw_string(int x, int y, char* str, uint32_t fg, uint32_t bg);
extern framebuffer_t fb;
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);
uint32_t alpha_blend(uint32_t fg, uint32_t bg, uint8_t alpha);
void draw_rect_alpha(int x, int y, int w, int h, uint32_t color, uint8_t alpha);

#endif