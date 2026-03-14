#ifndef TIMER_H
#define TIMER_H

void timer_init(int velocity);
int get_timer_ticks();
void timer_handler();
void sleep(int ms);

#endif