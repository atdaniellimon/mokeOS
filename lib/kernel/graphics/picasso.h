#ifndef PICASSO_H
#define PICASSO_H
#include "../../../debug/MokeSDK/ui.h"

// I'll later add gradients, I promise
typedef struct {
    uint32_t colors[4];
    int count;
    char direction;
} UI_Gradient;

void picasso_draw_rect(Rectangle* r);

#endif