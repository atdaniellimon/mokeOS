#include "vbe.h"
#include "font.h"
#include "../../lib/multiboot/m.h"
#include "../../lib/stdint/types.h"

#define PW(p) (fb.width * p / 100)
#define PH(p) (fb.height * p / 100)

framebuffer_t fb;
static uint32_t back_buffer[1024 * 768];
uint8_t mouse_design[16][16] = {
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,1,1,1,1,1,1,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

void vbe_init(void* mbi_ptr){
    multiboot_info_t* mbi = (multiboot_info_t*)mbi_ptr;

    fb.addr   = (uint32_t*)(uint32_t)mbi->framebuffer_addr;
    fb.pitch  = mbi->framebuffer_pitch;
    fb.width  = mbi->framebuffer_width;
    fb.height = mbi->framebuffer_height;
    fb.bpp    = mbi->framebuffer_bpp;
}

void put_pixel(int x, int y, uint32_t color) {
    if(x < 0 || x >= 1024 || y < 0 || y >= 768) return;
    back_buffer[y * 1024 + x] = color;
}

void vbe_swap() {
    for (int i = 0; i < (1024 * 768); i++) {
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

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
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
void vbe_draw_cursor(int x, int y){
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            uint8_t color_type = mouse_design[i][j];
            if (color_type == 1) {
                put_pixel(x + j, y + i, 0xFFFFFF); // Blanco
            } else if (color_type == 2) {
                put_pixel(x + j, y + i, 0x000000); // Negro
            }
        }
    }
}