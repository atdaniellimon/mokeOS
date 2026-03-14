#include "screen.h"

char *video_mem = (char*) 0xB8000;
char current_colour = 0x0F;
char custom_colour = 0x0F;
int screen_byte = 0;


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

void k_print(char *message){
    unsigned char *content = (unsigned char *)message;
    for(int j = 0; content[j] != '\0'; j++){
        if(screen_byte >= 4000){
            scroll();
        }
        if(content[j] == ' '){
            video_mem[screen_byte] = ' ';
            video_mem[screen_byte + 1] = current_colour;

            screen_byte+= 2;
            continue;
        }
        if(content[j] == '\n'){
            screen_byte = screen_byte / 160;
            screen_byte = screen_byte + 1;
            screen_byte = screen_byte * 160;
            
            continue;
        }
        video_mem[screen_byte] = content[j];
        screen_byte++;
        video_mem[screen_byte] = current_colour;
        screen_byte++;
    }
}

void k_print_at(char *mensaje, int x, int y){
    int position = (y * 160) + (x * 2);
    for(int j = 0; mensaje[j] != '\0'; j++){
        if(position >= 4000){
            scroll();
        }
        if(mensaje[j] == ' '){
            video_mem[position] = ' ';
            video_mem[position +1] = current_colour;

            position += 2;
            continue;
        }
        if(mensaje[j] == '\n'){
            position = position / 160;
            position = position + 1;
            position = position * 160;
            
            continue;
        }
        video_mem[position] = mensaje[j];
        position++;
        video_mem[position] = current_colour;
        position++;
    }
}

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
