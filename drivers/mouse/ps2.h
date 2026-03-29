#ifndef PS2_H
#define PS2_H
#include "../../lib/stdint/types.h"

typedef struct {
    int x, y;
    uint8_t left, right, middle;
    uint8_t last_left;
} mouse_state_t;

typedef void (*mouse_callback_t)(int x, int y, int buttons);
void mouse_set_handler(mouse_callback_t handler);
extern mouse_state_t mouse;

void mouse_init();
void mouse_handler();

#endif