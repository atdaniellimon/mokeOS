#include "MokeUI.h"

unsigned long rgb(int r, int g, int b){   
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

//#include "import.h"
//#include "../../drivers/vbe/vbe.h"
//#include "../../drivers/screen/screen.h"
//#include "../string/string.h"
//#include "../shell/shell.h"

int ctx_x = 0;
int ctx_y = 0;


static MokeButton button_list[32];
static int button_count = 0;
static int last_buffer_len = 0;

void clear_buttons(){
    button_count = 0;
}

void UI_update_cursor_state(int mx, int my) {
    char* target_cursor = "def";

    for (int i = 0; i < button_count; i++) {
        if (mx >= button_list[i].x && mx <= (button_list[i].x + button_list[i].w) &&
            my >= button_list[i].y && my <= (button_list[i].y + button_list[i].h)) {
            
            target_cursor = button_list[i].cursor_type;
            break;
        }
    }

    change_cursor(target_cursor);
}

void add_button(int x, int y, int w, int h, void (*action)(), char* cursor_type){
    if(button_count < 32){
        button_list[button_count].action = action;
        button_list[button_count].x      = x;
        button_list[button_count].y      = y;
        button_list[button_count].w      = w;
        button_list[button_count].h      = h;
        button_list[button_count].cursor_type = (cursor_type != 0) ? cursor_type : "def";
        button_count++;
    }
}

void UI_process_click(int mx, int my){
    for(int i = 0; i < button_count; i++){
        if(mx >= button_list[i].x && mx <= (button_list[i].x + button_list[i].w) &&
            my >= button_list[i].y && my <= (button_list[i].y + button_list[i].h)){
            if(button_list[i].action) button_list[i].action();
            return;
        }
    }
}

int is_border_pixel(int px, int py, int x, int y, int w, int h, ButtonArgs a){
    if(px < x || px >= x + w || py < y || py >= y + h){
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
    if(px < x + a.rBotL && py >= (y + h) - a.rBotL){ 
        int dx = (x + a.rBotL) - px;
        int dy = py - ((y + h) - a.rBotL);
        return (dx * dx + dy * dy <= a.rBotL * a.rBotL);
    }

    // Bottom Right
    if(px >= (x + w) - a.rBotR && py >= (y + h) - a.rBotR){
        int dx = px - ((x + w) - a.rBotR);
        int dy = py - ((y + h) - a.rBotR);
        return (dx * dx + dy * dy <= a.rBotR * a.rBotR);
    }
    
    return 1;
}

int UI_is_mouse_over(int mx, int my, MokeButton btn){
    return (mx >= btn.x && mx <= (btn.x + btn.w) &&
            my >= btn.y && my <= (btn.y + btn.h));
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
    if(args.hidden == 1) return;

    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    int width = (args.w > 0) ? args.w : (strlen(args.text) * 8) + (args.padding * 2);
    int height = (args.h > 0) ? args.h : 8 + (args.padding * 2);

    if(args.bg == 0){
        args.bg = rgb(0, 0, 0);
    }

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

    if(args.textAlign == 1){
        int text_width = strlen(args.text) * 8;
        int center_x = final_x + (width / 2) - (text_width / 2);
        int center_y = final_y + (height /2) - 4;
        
        draw_string(center_x, center_y, args.text, args.fg, args.bg);
    } else {
        draw_string(final_x + args.padding, final_y + args.padding, args.text, args.fg, args.bg);
    }
    add_button(final_x, final_y, width, height, args.action, args.cursor);
}

void UI_rect(RectArgs args){
    if(args.bg == 0) return;
    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    if(args.bg == 0){
        args.bg = rgb(0, 0, 0);
        return;
    }

    for(int i = 0; i < args.h; i++){
        for(int j = 0; j < args.w; j++){
            
            int px = final_x + j;
            int py = final_y + i;

            if(is_pixel_inside_radius(px, py, final_x, final_y, args.w, args.h, (ButtonArgs){
                .rTopL = args.rTopL, .rTopR = args.rTopR, 
                .rBotL = args.rBotL, .rBotR = args.rBotR
            })){
                put_pixel(px, py, args.bg);
            }
        }
    }
    if(args.action != 0){
        add_button(final_x, final_y, args.w, args.h, args.action, "poi");
    }
}

void UI_TextEntry(TextEntryArgs args){
    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    if(args.bg == 0){
        args.bg = rgb(0, 0, 0);
    }
    int current_len = (args.buffer != 0) ? strlen(args.buffer) : 0;

    for(int i = 0; i < args.h; i++){
        for(int j = 0; j < args.w; j++){
            
            int px = final_x + j;
            int py = final_y + i;

            if(is_pixel_inside_radius(px, py, final_x, final_y, args.w, args.h, (ButtonArgs){
                .rTopL = args.rTopL, .rTopR = args.rTopR, 
                .rBotL = args.rBotL, .rBotR = args.rBotR
            })){
                put_pixel(px, py, args.bg);
            }
        }
    }
    
    if (args.buffer != 0 && strlen(args.buffer) > 0) {
        draw_string(final_x + 10, (final_y + args.h / 2) - 4, args.buffer, args.colour, rgb(0, 0, 0));
    } else if (args.placeholder != 0) {
        draw_string(final_x + 10, (final_y + args.h / 2) - 4, args.placeholder, rgb(0, 0, 0), rgb(0, 0, 0));
    }
    add_button(final_x, final_y, args.w, args.h, 0, "ty");
    if(args.keypress != 0 && current_len != last_buffer_len){
        args.keypress(args.buffer);
        last_buffer_len = current_len;
    }
}

void UI_text(TextArgs args){
    int final_x = args.x + ctx_x;
    int final_y = args.y + ctx_y;

    if(args.bg == 0){
        args.bg = rgb(0, 0, 0);
    }
    

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

void UI_hide(UI_component* c){
    if(c) c->bounds.hidden = 1; 
}

void UI_show(UI_component* c){
    if(c) c->bounds.hidden = 0; 
}
