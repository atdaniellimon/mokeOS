#include "screen.h"
#include "../vbe/vbe.h"

uint32_t current_colour = 0xFFFFFF;
int background_colour   = 0x000000;
uint32_t custom_colour  = 0xFFFFFF;
int screen_byte         = 0;
int cursor_x            = 1;
int cursor_y            = 1;
uint32_t colour_map[] = {
    0x000000,
    0x0000AA,
    0x00AA00,
    0x00AAAA,
    0xAA0000,
    0xAA00AA,
    0xFFFFFF,
};

void scroll(){
    int row_height = 8;
    
    for(int y = row_height; y < (int)fb.height; y++){
        for(int x = 0; x < (int)fb.width; x++){
            fb.addr[(y - row_height) * (fb.pitch/4) + x] = fb.addr[y * (fb.pitch/4) + x];
        }
    }
    for(int x = 0; x < (int)fb.width; x++){
        fb.addr[(fb.height - row_height) * (fb.pitch/4) + x] = background_colour;
    }

    cursor_y -= row_height;
}

void k_print(char* message){
    for(int i = 0; message[i] != '\0'; i++){
        if(message[i] == '\n'){
            cursor_x = 0;
            cursor_y += 8;
        } else {
            draw_char(cursor_x, cursor_y, message[i], current_colour);
            cursor_x += 8;
            if(cursor_x >= (int)fb.width){
                cursor_x = 0;
                cursor_y += 8;
            }
        }
        if(cursor_y >= (int)fb.height - 8){
            scroll();
        }
    }
}

void k_print_at(char* message, int x, int y){
    int offset = 0;
    for(int i = 0; message[i] != '\0'; i++){
        draw_char(x + offset, y, message[i], current_colour);
        offset += 8;
    }
}

void clean_screen(){
    cursor_x = 1;
    cursor_y = 1;
    vbe_clear(0x000000);
}


void set_colour(unsigned char colour){
    if(colour == 0){
        current_colour = 0xFFFFFF;
    } else if(colour < 7){
        current_colour = colour_map[colour];
    } else {
        current_colour = 0xFFFFFF;
    }
}
