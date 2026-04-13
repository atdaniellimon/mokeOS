/*
    MokeUI SDK
    Version 1.0
    Made by Daniel Limon for mokeOS
    See more atdaniellimon.github.io
*/
#include "ui.h"

#define SYS_UI_RECT 10
#define SYS_UI_TEXT 11

static Point ctx_origin = {0, 0};
static bool prev_left_click =  0;
static bool ui_needs_redraw = 0;

/*
    This function manages user clicks,  and checks
    if click is inside border radius, returns true
*/
bool UI_button_clicked(Rectangle rect, MouseState ms){
    if(ms.left_click && !prev_left_click){
        if(ms.x >= rect.pos.x && ms.x <= rect.pos.x + rect.size.w &&
            ms.y >= rect.pos.y && ms.y <= rect.pos.y + rect.size.h){
            return 1;
        }
    }
    return 0;
}

/*
    Validates the last "left click" from the mouse
    component.
*/
void UI_end_frame(MouseState* ms){
    prev_left_click = ms->left_click;
}

/*
    Basic drawing primitive that sendsva rectangle
    to the Picasso engine.
*/
void UI_rect(Rectangle rect){
    if(!sdk_is_drawing || rect.is_hidden) return;
    ui_needs_redraw = 1;

    rect.pos.x += ctx_origin.x;
    rect.pos.y += ctx_origin.y;

    volatile Rectangle r = rect; 

    asm volatile (
        "movl %0, %%eax\n\t"
        "movl %1, %%ebx\n\t"
        "int $0x80"
        :
        : "g"(SYS_UI_RECT), "r"(&r)
        : "eax", "ebx", "memory"
    );
}
/*
    Basic  drawing  primitive  that  sends  a
    text  string  to  the Picasso compositor.
*/
void UI_text(Text t){
    if(!sdk_is_drawing || t.is_hidden) return;
    ui_needs_redraw = 1;

    t.pos.x += ctx_origin.x;
    t.pos.y += ctx_origin.y;

    volatile Text temp_t = t;

    asm volatile (
        "movl %0, %%eax\n\t"
        "movl %1, %%ebx\n\t"
        "int $0x80"
        :
        : "g"(SYS_UI_TEXT), "r"(&temp_t)
        : "eax", "ebx", "memory"
    );
}

/*
    This is not just simple, but mixes native
    functions such as  UI_rect  &  UI_text to
    create a fully working button.
*/
void UI_button(Button btn, MouseState ms){
    if(btn.is_hidden) return;
    ui_needs_redraw = 1;

    /*
        Button's hitbox
    */
    Rectangle click_area = { 
        .pos = { btn.pos.x + ctx_origin.x, btn.pos.y + ctx_origin.y },
        .size = btn.size
    };
    
    if(!sdk_is_drawing){
        if(UI_button_clicked(click_area, ms)){
            ui_needs_redraw = 1; 
            if(btn.onclick.execute){
                btn.onclick.execute(btn.onclick.args);
            }
        }
        return;
    }

    UI_rect((Rectangle){
        .pos = btn.pos, 
        .size = btn.size,
        .style = btn.style,
        .radius = btn.radius
    });

    UI_text((Text){
        .text = btn.content,
        .pos = btn.pos,
        .style = {.colour = rgb(0, 0, 0)} 
    });
}

/*
    Lets parents push position context in order
    to  arrange  its  children  in  its correct 
    position.
*/
void UI_PushContext(Point p){
    ctx_origin.x += p.x;
    ctx_origin.y += p.y;
}

void UI_PopContext(Point p){
    ctx_origin.x -= p.x;
    ctx_origin.y -= p.y;
}

/*
    This function makes an interruption
    in  order  to  get the  mouse's pos.
*/

void UI_get_mouse(MouseState* out_ms){
    asm volatile (
        "movl %0, %%eax\n\t"
        "movl %1, %%ebx\n\t"
        "int $0x80"
        :
        : "g"(13), "r"(out_ms)
        : "eax", "ebx", "memory"
    );
}

/*
    This function refreshes the current
    screen's content.
*/
void UI_refresh(){
    asm volatile ("int $0x80" : : "a"(12));
}

/*
    Both of this functions work in order
    to  show  or  hide  an  UI  element.
*/
void UI_hide(UI_component* c){
    if(c && c->bounds.is_hidden == 0){ 
        c->bounds.is_hidden = 1;
        ui_needs_redraw = 1;
    }
}

void UI_show(UI_component* c){
    if(c && c->bounds.is_hidden == 1){ 
        c->bounds.is_hidden = 0;
        ui_needs_redraw = 1;
    }
}

void UI_toggle(UI_component* c){
    if(c){
        if(c->bounds.is_hidden == 0){
            c->bounds.is_hidden = 1;
        } else {
            c->bounds.is_hidden = 0;
        }
    }
    ui_needs_redraw = 1;
}

void poweroff(){
    int syscall_num = 41;
    asm volatile (
        "int $0x80"
        :
        : "a" (syscall_num)
        : "memory"
    );
}

bool UI_check_dirty(){
    bool temp = ui_needs_redraw;
    ui_needs_redraw = 0;
    return temp;
}

void UI_set_drawing_mode(bool draw_mode){
    sdk_is_drawing = draw_mode;
}