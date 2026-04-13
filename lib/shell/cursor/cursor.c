/*
	mokeOS' cursors
	Made by Daniel Limón for mokeOS

	See more atdaniellimon.github.io
*/

#include "cursor.h"
#include "../../../drivers/vbe/vbe.h"

/*
	Cursors buffer to save what was behind it.
	Each cursor  size  is  16  by  16  pixels.
*/
static uint32_t cursor_backup[16 * 16];
static int last_mx = -1, last_my = -1;

void draw_hardware_cursor(int x, int y){
    /*
		Restore what was behind the cursor
    */
    if(last_mx != -1 && last_my != -1){
        for(int i = 0; i < 16; i++){
            for(int j = 0; j < 16; j++){
                int px = last_mx + j;
                int py = last_my + i;
                if(px >= 0 && px < fb.width && py >= 0 && py < fb.height){
                    back_buffer[py * fb.width + px] = cursor_backup[i * 16 + j];
                }
            }
        }
    }

    /*
		Make a new snapshot of what's behind.
    */
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 16; j++){
            int px = x + j;
            int py = y + i;
            if(px >= 0 && px < fb.width && py >= 0 && py < fb.height){
                cursor_backup[i * 16 + j] = back_buffer[py * fb.width + px];
            }
        }
    }

    /*
		And finally, draw the cursor onto the
		buffer.
    */
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 16; j++){
            int px = x + j;
            int py = y + i;
            if(px >= 0 && px < fb.width && py >= 0 && py < fb.height){
                if(mouse_default_design[i][j] == 1){
                    back_buffer[py * fb.width + px] = 0xFFFFFFFF; // Black
                } else if(mouse_default_design[i][j] == 2){
                    back_buffer[py * fb.width + px] = 0xFF000000; // White
                }
            }
        }
    }

    last_mx = x;
    last_my = y;
}