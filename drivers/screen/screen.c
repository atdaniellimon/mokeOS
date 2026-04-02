#include "screen.h"
<<<<<<< Updated upstream
char *video_mem = (char*) 0xB8000;
int screen_byte = 0;
char custom_colour = 0x0F;
char current_colour = 0x0F;
=======
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
>>>>>>> Stashed changes

void clean_screen(){
    for(int i = 0; i < 4000; i += 2){
        video_mem[i] = ' ';
        video_mem[i + 1] = current_colour;
    }
    screen_byte = 0;
}

void set_colour(char colour){
    if(colour == 0){
        current_colour = 0x0F;
    } else {
        current_colour = colour;
    }
}

void scroll(){
    for(int i = 160; i < 4000; i++) {
        video_mem[i - 160] = video_mem[i];
    }

    for(int i = 3840; i < 4000; i += 2) {
        video_mem[i] = ' ';
        video_mem[i + 1] = current_colour;
    }

    screen_byte = 3840;
}

void k_print(char *content){
    unsigned char *message = (unsigned char *)content;
    for(int j = 0; message[j] != '\0'; j++){
        if(screen_byte >= 4000){
            scroll();
        }
        if(message[j] == ' '){
            video_mem[screen_byte] = ' ';
            video_mem[screen_byte + 1] = current_colour;

            screen_byte+= 2;
            continue;
        }
        if(message[j] == '\n'){
            screen_byte = screen_byte / 160;
            screen_byte = screen_byte + 1;
            screen_byte = screen_byte * 160;
            
            continue;
        }
        video_mem[screen_byte] = message[j];
        screen_byte++;
        video_mem[screen_byte] = current_colour;
        screen_byte++;
    }
}

void k_print_at(char *content, int x, int y){
    int position = (y * 160) + (x * 2);
    for(int j = 0; content[j] != '\0'; j++){
        if(position >= 4000){
            scroll();
        }
        if(content[j] == ' '){
            video_mem[position] = ' ';
            video_mem[position +1] = current_colour;

            position += 2;
            continue;
        }
        if(content[j] == '\n'){
            position = position / 160;
            position = position + 1;
            position = position * 160;
            
            continue;
        }
        video_mem[position] = content[j];
        position++;
        video_mem[position] = current_colour;
        position++;
    }
}