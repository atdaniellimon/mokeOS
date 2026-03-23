#ifndef PS2_H
#define PS2_H
#include "../../lib/stdint/types.h"

typedef struct {
    int x;
    int y;
    uint8_t left;
    uint8_t right;
    uint8_t middle;
} mouse_state_t;

extern mouse_state_t mouse;

void mouse_init();
void mouse_handler();

#endif