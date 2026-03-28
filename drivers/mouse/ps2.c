#include "ps2.h"
#include "../../lib/stdint/types.h"
#include "../vbe/vbe.h"
#include "../screen/screen.h"
#include "../../arch/i386/io.h"
#include "../../lib/string/string.h"

mouse_state_t mouse = {512, 384, 0, 0, 0, 0}; 
static int mouse_cycle = 0;
static uint8_t mouse_bytes[3];

void draw_cursor(int x, int y) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            uint8_t color_type = mouse_design[i][j];
            if (color_type == 1) {
                put_pixel(x + j, y + i, rgb(255, 255, 255));
            } else if (color_type == 2) {
                put_pixel(x + j, y + i, rgb(0, 0, 0));
            }
        }
    }
}

void ps2_wait_write() {
    while(inb(0x64) & 2);
}
void ps2_wait_read() {
    while(!(inb(0x64) & 1));
}

void mouse_write(uint8_t data) {
    ps2_wait_write();
    outb(0x64, 0xD4);
    ps2_wait_write();
    outb(0x60, data);
}

uint8_t mouse_read() {
    ps2_wait_read();
    return inb(0x60);
}

void mouse_init() {
    uint8_t status;

    while(inb(0x64) & 1) { inb(0x60); }

    ps2_wait_write();
    outb(0x64, 0xA8);

    ps2_wait_write();
    outb(0x64, 0x20);
    ps2_wait_read();
    status = (inb(0x60) | 2);
    status &= ~0x20;
    
    ps2_wait_write();
    outb(0x64, 0x60);
    ps2_wait_write();
    outb(0x60, status);

    mouse_write(0xF4);
    mouse_read();
}

void mouse_handler() {
    uint8_t status = inb(0x64);
    
    if (!(status & 0x21)) return; 

    uint8_t data = inb(0x60);
    
    if(mouse_cycle == 0 && !(data & 0x08)) return;
    
    mouse_bytes[mouse_cycle++] = data;

    if(mouse_cycle == 3) {
        mouse_cycle = 0;
        mouse.last_left = mouse.left;

        mouse.left   = mouse_bytes[0] & 0x01;
        mouse.right  = mouse_bytes[0] & 0x02;
        mouse.middle = mouse_bytes[0] & 0x04;

        int dx = (int)mouse_bytes[1] - ((mouse_bytes[0] & 0x10) ? 256 : 0);
        int dy = (int)mouse_bytes[2] - ((mouse_bytes[0] & 0x20) ? 256 : 0);

        mouse.x += dx;
        mouse.y -= dy;

        if(mouse.x < 0) mouse.x = 0;
        if(mouse.y < 0) mouse.y = 0;
        if(mouse.x > 1023) mouse.x = 1023;
        if(mouse.y > 767)  mouse.y = 767;

        if (mouse.left && !mouse.last_left) {
            on_mouse_click(mouse.x, mouse.y); 
        }
    }
}