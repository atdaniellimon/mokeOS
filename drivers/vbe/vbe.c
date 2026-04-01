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

uint8_t get_glyph_width(unsigned char c) {
    uint8_t* glyph = font8x8[(int)c];
    uint8_t max_col = 0;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (glyph[row] & (1 << col)) {
                if (col > max_col) {
                    max_col = col;
                }
            }
        }
    }
    
    if (max_col == 0 && c == ' ') return 4; 
    return max_col + 1;
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
int draw_char(int x, int y, char c, uint32_t fg) {
    uint8_t* glyph = font8x8[(int)c];
    uint8_t width = get_glyph_width(c);

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < width; col++) {
            if (glyph[row] & (1 << col)) {
                put_pixel(x + col, y + row, fg);
            }
        }
    }
    return width;
}

void draw_string(int x, int y, char* str, uint32_t fg, uint32_t bg){
    (void)bg;
    int i = 0;
    int current_x = x;

    while(str[i] != '\0'){
        int width = draw_char(current_x, y, str[i], fg);
        current_x += width + 1; 
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