#include "screen.h"
char *video_mem = (char*) 0xB8000;
int screen_byte = 0;
char custom_colour = 0x0F;
char current_colour = 0x0F;

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