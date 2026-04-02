#include "syscall.h"
#include "../../arch/i386/idt.h"
#include "../../drivers/vbe/vbe.h"
#include "../../drivers/screen/screen.h"
#include "../../lib/timer/timer.h"
#include "../../drivers/keyboard/keyboard.h"

#define SYS_DRAW_RECT    1
#define SYS_DRAW_STRING  2
#define SYS_GET_TICKS    3
#define SYS_GET_KEY      4
#define SYS_EXIT         5
#define SYS_DRAW_PIXEL   6

static uint32_t syscall_dispatch(uint32_t num, uint32_t a, uint32_t b, uint32_t c, uint32_t d){
    switch(num){
        case SYS_DRAW_RECT:
            // a=x, b=y, c=packed(w,h), d=colour
            draw_rect(a, b, (c >> 16), (c & 0xFFFF), d);
            return 0;

        case SYS_DRAW_STRING:
            // a=x, b=y, c=ptr string, d=colour
            draw_string(a, b, (char*)c, d, 0x000000);
            return 0;

        case SYS_GET_TICKS:
            return get_timer_ticks();

        case SYS_GET_KEY:
            if(buffer_idx > 0){
                return keyboard_buffer[buffer_idx - 1];
            }
            return 0;

        case SYS_EXIT:
            // app's done so kill it hell yeah
            return 0;

        case SYS_DRAW_PIXEL:
            put_pixel(a, b, c);
            return 0;

        default:
            return (uint32_t)-1;
    }
}

uint32_t syscall_handler(uint32_t num, uint32_t a, uint32_t b, uint32_t c, uint32_t d){
    return syscall_dispatch(num, a, b, c, d);
}

void syscall_init(){
    idt_set_gate(0x80, (uint32_t)isr_syscall, 0x08, 0xEE);
}