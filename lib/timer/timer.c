#include "timer.h"
#include "../../arch/i386/io.h"
#define PIT_HZ 1193180

static volatile int ticks = 0;

void timer_init(int velocity){
    if(velocity <= 0) return;
    int real_vel = PIT_HZ / velocity;
    outb(0x43, 0x36);
    
    outb(0x40, real_vel & 0xFF);
    outb(0x40, real_vel >> 8);
}

void timer_handler(){
    ticks++;
    outb(0x20, 0x20);
}

int get_timer_ticks(){
    return ticks;
}

void sleep(int ms){
    int goal = get_timer_ticks() + ms;
    while(get_timer_ticks() < goal){
        asm volatile("sti");
        asm volatile("hlt");
    }
}