#ifndef PS2_H
#define PS2_H
#include "../../lib/stdint/types.h"

typedef struct {
    int x, y;
    uint8_t left, right, middle;
    uint8_t last_left;
} mouse_state_t;


extern uint8_t mouse_design[16][16];
extern mouse_state_t mouse;

void mouse_init();
void mouse_handler();

#endif