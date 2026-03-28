#include "import.h"
#include "../../drivers/vbe/vbe.h"
#include "../../drivers/screen/screen.h"
#include "../string/string.h"

int ctx_x = 0;
int ctx_y = 0;

int is_border_pixel(int px, int py, int x, int y, int w, int h, ButtonArgs a){
    if (px < x || px >= x + w || py < y || py >= y + h) {
        return 0;
    }

    // Top Left
    if(px < x + a.rTopL && py < y + a.rTopL){
        int dx = (x + a.rTopL) - px;
        int dy = (y + a.rTopL) - py;
        return (dx * dx + dy * dy <= a.rTopL * a.rTopL && dx * dx + dy * dy > (a.rTopL - 2) * (a.rTopL - 2));
    }
    
    // Top Right
    if(px >= x + w - a.rTopR && py < y + a.rTopR){
        int dx = px - (x + w - a.rTopR - 1);
        int dy = (y + a.rTopR) - py;
        return (dx * dx + dy * dy <= a.rTopR * a.rTopR && dx * dx + dy * dy > (a.rTopR - 2) * (a.rTopR - 2));
    }

    // Bottom Left
    if (px < x + a.rBotL && py >= (y + h) - a.rBotL) { 
        int dx = (x + a.rBotL) - px;
        int dy = py - ((y + h) - a.rBotL);
        return (dx * dx + dy * dy <= a.rBotL * a.rBotL);
    }

    // Bottom Right
    if (px >= (x + w) - a.rBotR && py >= (y + h) - a.rBotR) {
        int dx = px - ((x + w) - a.rBotR);
        int dy = py - ((y + h) - a.rBotR);
        return (dx * dx + dy * dy <= a.rBotR * a.rBotR);
    }
    
    return 1;
}

int is_pixel_inside_radius(int px, int py, int x, int y, int w, int h, ButtonArgs a){
    if(px < x + a.rTopL && py < y + a.rTopL){
        int dx = (x + a.rTopL) - px;
        int dy = (y + a.rTopL) - py;
        return (dx * dx + dy * dy <= a.rTopL * a.rTopL);
    }

    if(px >= x + w - a.rTopR && py < y + a.rTopR){
        int dx = px - (x + w - a.rTopR - 1);
        int dy = (y + a.rTopR) - py;
        return (dx * dx + dy * dy <= a.rTopR * a.rTopR);
    }

    if(px < x + a.rBotL && py >= y + h - a.rBotL){
        int dx = (x + a.rBotL) - px;
        int dy = py - (y + h - a.rBotL - 1);
        return (dx * dx + dy * dy <= a.rBotL * a.rBotL);
    }

    if(px >= x + w - a.rBotR && py >= y + h - a.rBotR){
        int dx = px - (x + w - a.rBotR - 1);
        int dy = py - (y + h - a.rBotR - 1);
        return (dx * dx + dy * dy <= a.rBotR * a.rBotR);
    }

    return 1;
}

void UI_btn(ButtonArgs args){
    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    int width = (args.w > 0) ? args.w : (strlen(args.text) * 8) + (args.padding * 2);
    int height = (args.h > 0) ? args.h : 8 + (args.padding * 2);

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int curr_x = final_x + j;
            int curr_y = final_y + i;

            if(is_pixel_inside_radius(curr_x, curr_y, final_x, final_y, width, height, args)){
                if(args.border && is_border_pixel(curr_x, curr_y, final_x, final_y, width, height, args)){
                    put_pixel(curr_x, curr_y, rgb(0, 0, 0));
                }else{
                    put_pixel(curr_x, curr_y, args.bg);
                }
            }
        }
    }

    draw_string(final_x + args.padding, final_y + args.padding, args.text, args.fg, args.bg);
    add_button(final_x, final_y, width, height, args.action);
}

void UI_rect(RectArgs args){
    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    for (int i = 0; i < args.h; i++){
        for (int j = 0; j < args.w; j++){
            
            int px = final_x + j;
            int py = final_y + i;

            if (is_pixel_inside_radius(px, py, final_x, final_y, args.w, args.h, (ButtonArgs){
                .rTopL = args.rTopL, .rTopR = args.rTopR, 
                .rBotL = args.rBotL, .rBotR = args.rBotR
            })){
                put_pixel(px, py, args.bg);
            }
        }
    }
}

void UI_text(TextArgs args){
    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    draw_string(final_x, final_y, args.text, args.colour, args.bg);
}

void UI_push_context(int x, int y){
    // For programming like swift
    ctx_x += x;
    ctx_y += y;
}

void UI_pop_context(int x, int y){
    // Reset to previous cords
    ctx_x -= x;
    ctx_y -= y;
}