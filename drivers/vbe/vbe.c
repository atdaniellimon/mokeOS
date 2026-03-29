#include "vbe.h"
#include "font.h"
#include "../../lib/multiboot/m.h"
#include "../../lib/stdint/types.h"

#define PW(p) (fb.width * p / 100)
#define PH(p) (fb.height * p / 100)

framebuffer_t fb;
static uint32_t back_buffer[1024 * 768];

void vbe_init(void* mbi_ptr){
    multiboot_info_t* mbi = (multiboot_info_t*)mbi_ptr;

    fb.addr   = (uint32_t*)(uint32_t)mbi->framebuffer_addr;
    fb.pitch  = mbi->framebuffer_pitch;
    fb.width  = mbi->framebuffer_width;
    fb.height = mbi->framebuffer_height;
    fb.bpp    = mbi->framebuffer_bpp;
}

void put_pixel(int x, int y, uint32_t color_raw){
    if(x < 0 || x >= 1024 || y < 0 || y >= 768) return;

    mcolor_t colour = {.raw = color_raw};

    if(colour.channels.a == 255){
        fb.addr[y * 1024 + x] = color_raw;
    } 
    else if(colour.channels.a > 0){
        uint32_t bg = fb.addr[y * 1024 + x];
        fb.addr[y * 1024 + x] = blend_colors(colour, bg);
    }
}

void vbe_swap(){
    for(int i = 0; i < (1024 * 768); i++){
        fb.addr[i] = back_buffer[i];
    }
}

void draw_rect(int x, int y, int w, int h, uint32_t colour){
    for(int row = y; row < y + h; row++){
        for(int col = x; col < x + w; col++){
            put_pixel(col, row, colour);
        }
    }
}

void vbe_clear(uint32_t colour){
    draw_rect(0, 0, fb.width, fb.height, colour);
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

uint32_t blend_colors(mcolor_t front, uint32_t back_raw){
    if (front.channels.a == 255) return front.raw;
    if (front.channels.a == 0)   return back_raw;

    mcolor_t back = {.raw = back_raw};
    uint32_t alpha = front.channels.a;
    uint32_t inv_alpha = 255 - alpha;

    // Formula: (Front * Alpha + Background * (255 - Alpha)) / 255
    uint8_t r = (uint8_t)((front.channels.r * alpha + back.channels.r * inv_alpha) >> 8);
    uint8_t g = (uint8_t)((front.channels.g * alpha + back.channels.g * inv_alpha) >> 8);
    uint8_t b = (uint8_t)((front.channels.b * alpha + back.channels.b * inv_alpha) >> 8);

    return (r << 16) | (g << 8) | b;
}