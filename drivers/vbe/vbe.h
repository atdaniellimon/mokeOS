#ifndef VBE_H
#define VBE_H

#include "../../lib/stdint/types.h"
#define rgba(r, g, b, a) (((mcolor_t){.channels = {(uint8_t)(b), (uint8_t)(g), (uint8_t)(r), (uint8_t)(a)}}).raw)
#define rgb(r, g, b) rgba(r, g, b, 255)

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

void vbe_init(void* mbi);
void put_pixel(int x, int y, uint32_t color_raw);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void vbe_clear(uint32_t color);
void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void draw_string(int x, int y, char* str, uint32_t fg, uint32_t bg);
extern framebuffer_t fb;
uint32_t blend_colors(mcolor_t front, uint32_t back_raw);
void vbe_swap();

#endif