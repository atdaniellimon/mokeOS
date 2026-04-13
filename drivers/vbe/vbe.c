/*
    mokeOS's VBE driver
    Version 1.2
    Made by Daniel Limon for MokeOS

    See more atdaniellimon.github.io
*/

#include "vbe.h"
#include "font.h"
#include "../../lib/multiboot/m.h"
#include "../../lib/stdint/types.h"
#include "../../lib/malloc/mem.h"
#include "../../lib/string/string.h"
#include "../mouse/ps2.h"

uint32_t back_buffer[1024 * 768];
framebuffer_t fb;

void vbe_init(void* mbi_ptr){
    multiboot_info_t* mbi = (multiboot_info_t*)mbi_ptr;

    fb.addr   = (uint32_t*)(uint32_t)mbi->framebuffer_addr;
    fb.pitch  = mbi->framebuffer_pitch;
    fb.width  = mbi->framebuffer_width;
    fb.height = mbi->framebuffer_height;
    fb.bpp    = mbi->framebuffer_bpp;
    
    // Clean buffer when started
    vbe_clear(0x000000); 
}

/*
    This   function  lets  Picasso   draw
    pixels inside the screen's resolution.
*/
void put_pixel(int x, int y, uint32_t color_raw){
    if(x < 0 || x >= (int)fb.width || y < 0 || y >= (int)fb.height) return;

    mcolor_t c = {.raw = color_raw};

    if(c.channels.a == 255){
        back_buffer[y * fb.width + x] = color_raw;
    } else if(c.channels.a > 0){
        uint32_t bg = back_buffer[y * fb.width + x];
        back_buffer[y * fb.width + x] = apply_alpha(color_raw, bg);
    }
}

/*
    This function ensures the content of the display
    is  never  overwritten  by  the  mouse's  design.
*/
void vbe_swap(){
    extern mouse_state_t mouse;

    memcpy(fb.addr, back_buffer, fb.width * fb.height * 4);
}

/*
    This function makes sure to overwrite all of the
    content  with  a  specific  solid  colour.
*/
void vbe_clear(uint32_t colour){
    for(uint32_t i = 0; i < (fb.width * fb.height); i++){
        back_buffer[i] = colour;
    }
}

/*
    This function draws a perfect rectangle with the
    specified coordinates.
*/
void draw_rect(int x, int y, int w, int h, uint32_t colour){
    for(int row = y; row < y + h; row++){
        for(int col = x; col < x + w; col++){
            put_pixel(col, row, colour);
        }
    }
}

/*
    This allows the OS to write a specific character
    on the VBE framebuffer.
*/
int draw_char(int x, int y, char c, uint32_t fg){
    uint8_t* glyph = font8x8[(int)c];
    uint8_t width = 8;

    for(int row = 0; row < 8; row++){
        for(int col = 0; col < width; col++){
            if(glyph[row] & (1 << col)){
                put_pixel(x + col, y + row, fg);
            }
        }
    }
    return width;
}

/*
    This function uses the previously defined func-
    tion "draw_char" multiple times in order to wr-
    ite a complete  string  inside the framebuffer.
*/
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

/*
    This are defined in order to enable a RGBA col-
    our pallete.
*/
uint32_t apply_alpha(uint32_t front_raw, uint32_t back_raw){
    mcolor_t front = {.raw = front_raw};
    mcolor_t back  = {.raw = back_raw};

    uint8_t a = front.channels.a;
    if(a == 255) return front_raw;
    if(a == 0)   return back_raw;

    uint16_t inv_a = 255 - a;

    uint8_t r = (uint8_t)((front.channels.r * a + back.channels.r * inv_a) >> 8);
    uint8_t g = (uint8_t)((front.channels.g * a + back.channels.g * inv_a) >> 8);
    uint8_t b = (uint8_t)((front.channels.b * a + back.channels.b * inv_a) >> 8);

    return (0xFF000000 | (r << 16) | (g << 8) | b);
}

/*
    This function uses some basic maths in order to
    detect if a pixel is inside the assigned border
    radius,  which returns  true or false depending
    on the case and draws  the rect pixel by pixel.
*/
void draw_rect_rounded(int x, int y, int w, int h, uint32_t colour, int tl, int tr, int bl, int br){
    mcolor_t c = {.raw = colour};

    for(int py = y; py < y + h; py++){
        for(int col = x; col < x + w; col++){
            bool should_draw = 1;

            if(col < x + tl && py < y + tl){                  // Top left
                int dx = (x + tl) - col;
                int dy = (y + tl) - py;
                if (dx * dx + dy * dy > tl * tl) should_draw = 0;
            } else if(col >= x + w - tr && py < y + tr){      // Top right
                int dx = col - (x + w - tr - 1);
                int dy = (y + tr) - py;
                if (dx * dx + dy * dy > tr * tr) should_draw = 0;
            } else if(col < x + bl && py >= y + h - bl){      // Bottom left
                int dx = (x + bl) - col; 
                int dy = py - (y + h - bl - 1);
                if(dx * dx + dy * dy > bl * bl) should_draw = 0;
            } else if(col >= x + w - br && py >= y + h - br){ // Bottom right
                int dx = col - (x + w - br - 1);
                int dy = py - (y + h - br - 1);
                if(dx * dx + dy * dy > br * br) should_draw = 0;
            }

            if(should_draw) put_pixel(col, py, colour);
        }
    }
}