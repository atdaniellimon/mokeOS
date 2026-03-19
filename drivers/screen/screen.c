#include "screen.h"
#define MAX_BYTES 4000
#define LINE_SIZE 160

char *video_mem = (char*) 0xB8000;
char current_colour = 0x0F;
char custom_colour = 0x0F;
int screen_byte = 0;


void scroll(){
    for(int i = LINE_SIZE; i < MAX_BYTES; i++){
        video_mem[i - LINE_SIZE] = video_mem[i];
    }

    for(int i = MAX_BYTES - LINE_SIZE; i < MAX_BYTES; i += 2){
        video_mem[i] = ' ';
        video_mem[i + 1] = current_colour;
    }

    screen_byte = MAX_BYTES - LINE_SIZE;
}

void k_print(char *message){
    unsigned char *content = (unsigned char *)message;
    for(int j = 0; content[j] != '\0'; j++){
        if(screen_byte >= MAX_BYTES){
            scroll();
        }
        if(content[j] == '\n'){
            screen_byte = ((screen_byte / LINE_SIZE) + 1) * LINE_SIZE;
            continue;
        }
        
        video_mem[screen_byte++] = content[j];
        video_mem[screen_byte++] = current_colour;
    }
}

void k_print_at(char *message, int x, int y){
    int position = (y * LINE_SIZE) + (x * 2);

    for(int j = 0; message[j] != '\0'; j++){
        if(position >= MAX_BYTES){
            scroll();
            position -= LINE_SIZE;
        }

        if(message[j] == '\n'){
            position = ((position / LINE_SIZE) + 1) * LINE_SIZE;
            continue;
        }

        video_mem[position++] = message[j];
        video_mem[position++] = current_colour;
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
    if(colour == 0 || !colour){
        current_colour = 0x0F;
    } else {
        current_colour = colour;
    }
}
