#include "ps2.h"
#include "../../lib/stdint/types.h"
#include "../vbe/vbe.h"
#include "../screen/screen.h"
#include "../../arch/i386/io.h"
#include "../../lib/string/string.h"

mouse_state_t mouse = {512, 384, 0, 0, 0};
static int mouse_count = 0;

static int mouse_cycle = 0;
static uint8_t mouse_bytes[3];
int last_x = 0;
int last_y = 0;

void draw_cursor(int x, int y) {
    draw_rect(last_x, last_y, 8, 8, 0x000000);
    draw_rect(x, y, 8, 8, 0xFF0000);
    last_x = x;
    last_y = y;
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
    ps2_wait_write();
    outb(0x64, 0xA8);
    
    ps2_wait_write();
    outb(0x64, 0x60);
    ps2_wait_write();
    outb(0x60, 0x47);

    mouse_write(0xF4);
    mouse_read();

    while(inb(0x64) & 1) {
        inb(0x60);
    }
}

void mouse_handler(){
    mouse_count++;
    // dibuja el contador en esquina
    char buf[12];
    into_string(mouse_count, buf);
    draw_rect(1, 750, 100, 18, 0x000000);
    draw_string(1, 750, buf, 0xFF0000, 0x000000);

    draw_rect(1, 750, 50, 18, 0xFF0000);
    uint8_t status = inb(0x64);
    if(!(status & 0x20)){  // bit 5 is mouse data
        return;            // if is not mouse then ignore
    }
    
    uint8_t data = inb(0x60);
    
    if(mouse_cycle == 0 && !(data & 0x08)){
        return;
    }
    
    mouse_bytes[mouse_cycle] = data;
    mouse_cycle++;

    if(mouse_cycle == 3){
        mouse_cycle = 0;
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

        draw_cursor(mouse.x, mouse.y);
    }
}