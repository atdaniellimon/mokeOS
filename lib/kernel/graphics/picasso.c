/*
    Picasso Compositor
    Made by Daniel Limon for mokeOS

    See more atdaniellimon.github.io
*/

#include "picasso.h"
#include "../../../drivers/vbe/vbe.h"

/*
    This function makes picasso detect  if a  pixel
    is inside the object itself and returns 0 or 1.
*/
static bool is_pixel_in_corner(int x, int y, int cx, int cy, int r){
    if (r <= 0) return 1;
    int dx = x - cx;
    int dy = y - cy;
    return (dx * dx + dy * dy) <= (r * r);
}

void picasso_draw_rect(Rectangle* r){
    if (!r) return;
    if (r->size.w > 2000 || r->size.h > 2000 || r->size.w <= 0 || r->size.h <= 0) return;

    int x1 = r->pos.x;
    int y1 = r->pos.y;
    int x2 = x1 + r->size.w;
    int y2 = y1 + r->size.h;

    uint32_t* dst = &back_buffer[r->pos.y * fb.width + r->pos.x];
    uint32_t color = r->style.fill;

    for(int i = 0; i < r->size.h; i++){
        for(int j = 0; j < r->size.w; j++){
            dst[j] = color;
        }
        dst += fb.width;
    }
}