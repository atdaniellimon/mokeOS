/*
    mokeOS Sherlock UI
    Version Beta 10
    Made by Daniel Limon for MokeOS

    See more atdaniellimon.github.io

    Version codename: Nebula Beta
    Target OS:        mokeOS base
*/
#include "shell.h"
#include "terminal.h"
#include "../date/date.h"
#include "cursor/cursor.h"
#include "../string/string.h"
#include "../../debug/MokeSDK/ui.h"


#define current_user "tester"
#define MAGIC_COLOUR 0xDEADBEEF

int screen_w        = 1024;
int screen_h        = 768;
int bg_colour       = rgb(99,99,156);
MouseState ms;

typedef void (*DrawCallback)();

typedef enum {
    LAYER_BG = 0,
    LAYER_APPS,
    LAYER_SYSTEM,
    LAYER_MAX
} LayerID;

typedef struct {
    uint32_t* buffer;
    DrawCallback render;
    bool is_dirty;
} Layer;

Layer system_layers[LAYER_MAX];

static UI_component Moke_menu = { .bounds = { .is_hidden = 1 } };
static UI_component Sherlock  = { .bounds = { .is_hidden = 1 } }; 
static UI_component Test_app  = { .bounds = { .is_hidden = 0 } };

void draw_sherlock(){
    UI_container(((Rectangle){
        .pos  = {((screen_w / 2) - 150), 100},
        .size = {300, 50},
        .is_hidden = Sherlock.bounds.is_hidden
    })){
        UI_rect((Rectangle){
            .pos = {0, 0},
            .size = {300, 50},
            .style = {.fill = rgba(221, 221, 221, 200)},
            .radius = RadiusAll(20)
        });
        UI_text((Text){
            .pos = {10, 20},
            .text = "Sherlock Search",
            .style = {
                .colour = rgb(0, 0, 0)
            }
        });
    }
}

void change_background(){
    bg_colour = rgb(100, 100, 100);
}

void test_app(){
    UI_container(((Rectangle){
        .pos  = {((screen_w / 2) - 300), 100},
        .size = {600, 400},
        .is_hidden = Test_app.bounds.is_hidden

    })){
        UI_rect((Rectangle){
            .pos = {0, 0},
            .size = {600, 400},
            .style = {.fill = rgba(221, 221, 221, 200)},
            .radius = RadiusAll(20)
        });
        UI_rect((Rectangle){
            .pos = {5, 5},
            .size = {590, 30},
            .style = {.fill = rgb(221, 221, 221)},
            .radius = RadiusAll(12)
        });
        UI_rect((Rectangle){
            .pos = {10, 10},
            .size = {20, 20},
            .style = {.fill = rgb(255, 109, 98)},
            .radius = RadiusAll(10)
        });
        UI_rect((Rectangle){
            .pos = {35, 10},
            .size = {20, 20},
            .style = {.fill = rgb(245, 231, 0)},
            .radius = RadiusAll(10)
        });
        UI_rect((Rectangle){
            .pos = {60, 10},
            .size = {20, 20},
            .style = {.fill = rgb(66, 211, 11)},
            .radius = RadiusAll(10)
        });
        UI_text((Text){
            .pos = {((600 / 2) - (4 * 8)), 15},
            .text = "Test",
            .style = {
                .colour = rgb(0, 0, 0)
            }
        });
    }
}

void draw_apps_layer(){
    test_app();
}
void draw_background_layer(){
    UI_rect((Rectangle){
        .pos = {0, 0},
        .size = {screen_w, screen_h},
        .style = {.fill = bg_colour},
    });
}

void draw_system_layer(){
    UI_rect((Rectangle){
        .pos = {0, 0},
        .size = {screen_w, 30},
        .style = {.fill = rgb(221, 221, 221)},
        .radius = {0, 0, 0, 0},
        .is_hidden = 0
    });
    /*UI_rect((Rectangle){
        .pos = {5, 5},
        .size = {200, 30},
        .style = {.fill = rgb(221, 221, 221)},
        .radius = {0, 0, 0, 0},
        .is_hidden = 0,
        .radius = RadiusAll(12)
    });*/

    UI_button((Button){
        .content = "moke",
        .pos = {18, 10},
        .size = {32, 8},
        .onclick = Call(UI_toggle, &Moke_menu),
    }, ms);

    UI_text((Text){
        .text = "Workspace",
        .pos = {62, 10},
        .style = {
            .colour = rgb(0, 0, 0)
        }
    });

    UI_text((Text){
        .text = "File",
        .pos = {154, 10},
        .style = {
            .colour = rgb(0, 0, 0)
        }
    });

    UI_container(((Rectangle){
            .pos = {10, 35},
            .size = {120, 130},
            .style = {.fill = rgb(221, 221, 221)},
            .radius = RadiusAll(15),
            .is_hidden = Moke_menu.bounds.is_hidden
        })){
            UI_button((Button){
                .content = "About",
                .pos = {10, 15},
                .size = {40, 8},
                .radius = RadiusAll(8),
                .onclick = Call(poweroff, 0)
            }, ms);
            UI_button((Button){
                .content = "Sherlock",
                .pos = {10, 45},
                .size = {40, 8},
                .radius = RadiusAll(8),
                .onclick = Call(UI_toggle, &Sherlock)
            }, ms);
            UI_button((Button){
                .content = "Lock screen",
                .pos = {10, 75},
                .size = {40, 8},
                .radius = RadiusAll(8),
                .onclick = Call(change_background, 0)
            }, ms);
            UI_button((Button){
                .content = "Power off",
                .pos = {10, 105},
                .size = {40, 8},
                .radius = RadiusAll(8),
                .onclick = Call(poweroff, 0)
            }, ms);
        }

    draw_sherlock();
}

void shell_main(){
    MouseState new_ms;
    system_layers[LAYER_BG].render     = draw_background_layer;
    system_layers[LAYER_APPS].render   = draw_apps_layer;
    system_layers[LAYER_SYSTEM].render = draw_system_layer;

    for(int i = 0; i < LAYER_MAX; i++) system_layers[i].is_dirty = 1;

    while(1){
        UI_get_mouse(&new_ms);
        ms = new_ms; 
    
        UI_set_drawing_mode(0);
        for(int i = 0; i < LAYER_MAX; i++){
            if(system_layers[i].is_dirty){ 
                if(system_layers[i].render) system_layers[i].render();
                system_layers[i].is_dirty = 0;
            }
        }

        if(UI_check_dirty()){
            system_layers[LAYER_SYSTEM].is_dirty = 1;
            system_layers[LAYER_APPS].is_dirty = 1;
        }

        UI_set_drawing_mode(1);
        for(int i = 0; i < LAYER_MAX; i++){
            if(system_layers[i].render) system_layers[i].render();
        }

        draw_hardware_cursor(new_ms.x, new_ms.y);

        UI_refresh();
        UI_end_frame(&new_ms);
    }
}