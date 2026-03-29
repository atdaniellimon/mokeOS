/*
mokeOS Beta - Shell con capas y cursor estable
*/
#include "shell.h"
#include "terminal.h"
#include "../../drivers/vbe/vbe.h"
#include "../../drivers/screen/screen.h"
#include "../string/string.h"
#include "../../arch/i386/io.h"
#include "../../drivers/hardware/hardware.h"
#include "../timer/timer.h"
#include "../date/date.h"
#include "../../drivers/mouse/ps2.h"
#include "../mokeUI/import.h"
#include "../../debug/mfs/moke.h"
#include "../signals/signals.h"

//From now and on, I will create comments for better readability and to maintain consistency with the codebase.

#define current_user "tester"
#define MAGIC_COLOUR 0xDEADBEEF

int app_count = 0;
int sys_count = 0;
void (*layer_apps_list[10])();
void (*layer_sys_list[10])();
static uint32_t* vram_phys = 0;

void begin_layer_draw(uint32_t* layer_ptr){
    if(vram_phys == 0) vram_phys = fb.addr;
    fb.addr = layer_ptr;
}

void end_layer_draw(){
    fb.addr = vram_phys;
}
// Cursor design: 1 = white, 2 = black, 0 = transparent
uint8_t mouse_design[16][16] = {
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,1,1,1,1,1,1,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// VBE Layers: wallpaper, apps, system
static uint32_t layer_wallpaper[1024*768];
static uint32_t layer_apps[1024*768];
static uint32_t layer_sys[1024*768];
static uint32_t final_composition_buffer[1024*768];

// Screen dimensions and UI state
int screen_w = 1024;
int screen_h = 768;
int ui_needs_update = 0;

void shell_reboot(){ power("reboot"); }
void shell_poweroff(){ power("off"); }


void clear_layer_magic(uint32_t* layer){
    for(int i= 0;i<1024*768;i++)
        layer[i]=MAGIC_COLOUR;
}

void wait_for_vsync(){
    while(inb(0x3DA) & 8);
    while(!(inb(0x3DA) & 8));
}

void ui_inject_sys(void (*func)()){
    for(int i = 0; i<sys_count; i++) if(layer_sys_list[i]==func) return;
    if(sys_count < 10) layer_sys_list[sys_count++] = func;
}

void ui_remove_sys(void (*func)()){
    for(int i = 0; i < sys_count; i++){
        if(layer_sys_list[i] == func){
            for(int j = i; j < sys_count-1; j++) layer_sys_list[j] = layer_sys_list[j+1];
            
            sys_count--;
            return;
        }
    }
}

void on_mouse_signal(int x, int y, int b){
    if((b & 1) && !mouse.last_left){ 
        UI_process_click(x, y); 
    }
    mouse.last_left = (b & 1); // Save state
    ui_needs_update = 1;
}

void draw_ui_mouse_to_buffer(uint32_t* buffer, int x, int y){
    for(int i = 0; i < 16; i++){      // For each row of the cursor
        for(int j = 0; j < 16; j++){  // For each column of the cursor
            if((x + j) >= screen_w || (y + i) >= screen_h || (x + j) < 0 || (y + i) < 0) continue;

            uint8_t colour_type = mouse_design[i][j];
            uint32_t offset = (y + i) * screen_w + (x + j);

            if(colour_type == 1) buffer[offset] = rgb(255, 255, 255);
            else if(colour_type == 2) buffer[offset] = rgb(0, 0, 0);
        }
    }
}

// Composes the final screen by layering wallpaper, apps, system, and mouse cursor, then copies to VRAM
void compose_ui(){
    memcpy(final_composition_buffer, layer_wallpaper, sizeof(layer_wallpaper));

    for(int i= 0; i < 1024*768; i++){
        if(layer_apps[i] != MAGIC_COLOUR) final_composition_buffer[i] = layer_apps[i];
        if(layer_sys[i] != MAGIC_COLOUR) final_composition_buffer[i] = layer_sys[i];
    }

    draw_ui_mouse_to_buffer(final_composition_buffer, mouse.x, mouse.y);
    memcpy(fb.addr, final_composition_buffer, sizeof(final_composition_buffer));
}

// Initializes layers with default colours and magic colour for transparency
void init_layers(){
    for(int i= 0;i<1024*768;i++){
        layer_wallpaper[i] = rgb(99,99,156);
        layer_apps[i] = MAGIC_COLOUR;
        layer_sys[i] = MAGIC_COLOUR;
    }
}

static UI_component quick_menu = { .bounds = { .hidden = 1 } };
static UI_component alert_box  = { .bounds = { .hidden = 1 } };

// Toggles the visibility of the quick menu when the "moke" button is clicked
void toggle_moke_menu(){
    if(quick_menu.bounds.hidden){
        UI_show(&quick_menu);
    } else {
        UI_hide(&quick_menu);
    }
}

void hide_alert(){
    UI_hide(&alert_box);
}

void render_alert(){
    UI_Container(alert_box, ((RectArgs){
        .x = (screen_w / 2) - 150,
        .y = (screen_h / 2) - 100,
        .w = 300,
        .h = 200,
        .hidden = alert_box.bounds.hidden
    })){
        UI_rect((RectArgs){
            .w = 300,
            .h = 200,
            .bg = rgba(221, 221, 221, 100),
            .rBotL = 25,
            .rBotR = 25,
            .rTopL = 25,
            .rTopR = 25
        });
        UI_text((TextArgs){
            .bg = rgba(221, 221, 221, 0),
            .x = 15,
            .y = 15,
            .text = "lorem",
            .colour = rgb(0, 0, 0)
        });
        UI_btn((ButtonArgs){
            .text = "ok",
            .bg = rgb(89, 171, 229),
            .fg = rgb(255, 255, 255),
            .y = 140,
            .w = 250,
            .x = 25,
            .h = 40,
            .action = hide_alert,
            .rBotL = 15,
            .rBotR = 15,
            .rTopL = 15,
            .rTopR = 15,
            .textAlign = 1
        });
    }
}

void show_alert(){
    UI_show(&alert_box);
}

// Renders the top bar with buttons and the current time, and registers their actions
void render_top_bar(){
    static UI_component top_bar_comp; 
    char buf[12];

    // Create the top bar container and its buttons, and draw a separator line
    UI_Container(top_bar_comp, ((RectArgs){
        .x = 0,
        .y = 0, 
        .w = screen_w, 
        .h = 30, 
    })){
        UI_rect((RectArgs){
            .x = 0, 
            .y = 0, 
            .w = screen_w, 
            .h = 30, 
            .bg=rgb(221,221,221)
        });
        UI_rect((RectArgs){
            .x = 0, 
            .y = 31, 
            .w = screen_w, 
            .h = 1, 
            .bg = rgba(0, 0, 0, 150)
        });
        UI_btn((ButtonArgs){
            .text = "moke",
            .x = 8,
            .y = 1,
            .h = 29,
            .padding = 11,
            .bg = rgb(221,221,221),
            .fg = rgb(0,0,0),
            .action = toggle_moke_menu
        });
        UI_btn((ButtonArgs){
            .text = "Workspace",
            .x = 52,
            .y = 1,
            .h = 29,
            .padding = 11,
            .fg = rgb(0,0,0),
            .bg = rgb(221,221,221)
        });
        UI_btn((ButtonArgs){
            .text = "File",
            .x = 134,
            .y = 1,
            .h = 29,
            .padding = 11,
            .fg = rgb(0,0,0),
            .bg = rgb(221,221,221)
        });
        UI_rect((RectArgs){
            .bg = rgb(0,0,0),
            .w = 1024,
            .h = 1,
            .x = 0,
            .y = 30
        });
    }

    get_date();
    into_string(hours,buf);
    draw_string(940,12,buf,rgb(0,0,0),rgb(221,221,221));
    draw_string(956,12,":",rgb(0,0,0),rgb(221,221,221));
    into_string(minutes,buf);
    draw_string(964,12,buf,rgb(0,0,0),rgb(221,221,221));
}

void renderMokeOptions(){
    UI_Container(quick_menu, ((RectArgs){
        .x = 10,
        .y = 35,
        .w = 120,
        .h = 100,
        .bg = 0,
        .border = 0,
        .hidden = quick_menu.bounds.hidden
    }))
    {
        UI_rect((RectArgs){
            .x= 0, 
            .y= 0, 
            .w = 120, 
            .h = 100, 
            .bg = rgb(221,221,221),
            .rTopL = 10,
            .rTopR = 10,
            .rBotL = 10,
            .rBotR = 10,
        });
        UI_btn((ButtonArgs){
            .text = "About",
            .x = 1,
            .y = 5,
            .w = 118,
            .padding = 10,
            .bg = rgb(221,221,221),
            .fg = rgb(0, 0, 0),
            .rTopL = 10,
            .rTopR = 10,
            .action = show_alert
        });
        UI_btn((ButtonArgs){
            .text = "Restart",
            .x = 1,
            .y = 35,
            .w = 118,
            .padding = 10,
            .fg = rgb(0,0,0),
            .bg = rgb(221,221,221),
            .action = shell_reboot
        });
        UI_btn((ButtonArgs){
            .text = "Power Off",
            .x = 1,
            .y = 65,
            .w = 118,
            .padding = 10,
            .fg = rgb(0,0,0),
            .bg = rgb(221,221,221),
            .action = shell_poweroff
        });
    }
}

void start_shell(){
    extern int ctx_x, ctx_y;
    ctx_x = 0; 
    ctx_y = 0;
    init_layers();
    mouse_set_handler(on_mouse_signal);
    
    while(1){
        if(ui_needs_update){
            clear_buttons(); 

            begin_layer_draw(layer_sys);
            clear_layer_magic(layer_sys);
            render_top_bar();
            renderMokeOptions();
            render_alert();
            end_layer_draw();

            begin_layer_draw(layer_apps);
                for(int i = 0; i < app_count; i++){
                    layer_apps_list[i]();
                }
            end_layer_draw();

            compose_ui();

            ui_needs_update = 0;
        }


        if(mouse.left && !mouse.last_left){
            UI_process_click(mouse.x, mouse.y);
            ui_needs_update = 1; // Forzamos refresh para ver cambios (como abrir el menú)
        }
        mouse.last_left = mouse.left;

        asm volatile("hlt");
    }
}