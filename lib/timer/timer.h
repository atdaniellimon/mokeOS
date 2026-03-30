#ifndef TIMER_H
#define TIMER_H

typedef struct {
    int target_tick;
    void (*callback)();
    int active;
} DelayedTask;

void timer_init(int velocity);
int get_timer_ticks();
void timer_handler();

#endif