#include "vbe.h"
#include "font.h"
#include "../../lib/multiboot/m.h"
#include "../../lib/stdint/types.h"

#define PW(p) (fb.width * p / 100)
#define PH(p) (fb.height * p / 100)

framebuffer_t fb;

void vbe_init(void* mbi_ptr){
    multiboot_info_t* mbi = (multiboot_info_t*)mbi_ptr;

    fb.addr = (uint32_t*)(uint32_t)mbi->framebuffer_addr;
    fb.pitch  = mbi->framebuffer_pitch;
    fb.width  = mbi->framebuffer_width;
    fb.height = mbi->framebuffer_height;
    fb.bpp    = mbi->framebuffer_bpp;
}

void put_pixel(int x, int y, uint32_t color){
    if(x < 0 || x >= (int)fb.width)  return;
    if(y < 0 || y >= (int)fb.height) return;

    fb.addr[y * (fb.pitch / 4) + x] = color;
}

void draw_rect(int x, int y, int w, int h, uint32_t color){
    for(int row = y; row < y + h; row++){
        for(int col = x; col < x + w; col++){
            put_pixel(col, row, color);
        }
    }
}

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t alpha_blend(uint32_t fg, uint32_t bg, uint8_t alpha){
    uint8_t fg_r = (fg >> 16) & 0xFF;
    uint8_t fg_g = (fg >> 8)  & 0xFF;
    uint8_t fg_b =  fg        & 0xFF;

    uint8_t bg_r = (bg >> 16) & 0xFF;
    uint8_t bg_g = (bg >> 8)  & 0xFF;
    uint8_t bg_b =  bg        & 0xFF;

    uint8_t r = (fg_r * alpha + bg_r * (255 - alpha)) / 255;
    uint8_t g = (fg_g * alpha + bg_g * (255 - alpha)) / 255;
    uint8_t b = (fg_b * alpha + bg_b * (255 - alpha)) / 255;

    return rgb(r, g, b);
}

void vbe_clear(uint32_t color){
    draw_rect(0, 0, fb.width, fb.height, color);
}
void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg){
    uint8_t* glyph = font8x8[(int)c];

    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){
            if(glyph[row] & (1 << col)){
                put_pixel(x + col, y + row, fg);
            } else {
                put_pixel(x + col, y + row, bg);
            }
        }
    }
}

void draw_string(int x, int y, char* str, uint32_t fg, uint32_t bg){
    int i = 0;
    while(str[i] != '\0'){
        draw_char(x + (i * 8), y, str[i], fg, bg);
        i++;
    }
}

void draw_rect_alpha(int x, int y, int w, int h, uint32_t color, uint8_t alpha){
    for(int row = y; row < y + h; row++) {
        for(int col = x; col < x + w; col++) {
            // leer pixel actual del framebuffer
            uint32_t bg = fb.addr[row * (fb.pitch/4) + col];
            // mezclar con el color nuevo
            uint32_t result = blend(color, bg, alpha);
            // escribir resultado
            fb.addr[row * (fb.pitch/4) + col] = result;
        }
    }
}