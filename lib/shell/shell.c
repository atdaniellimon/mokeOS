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
#include "cursors/cursors.h"

//From now and on, I will create comments for better readability and to maintain consistency with the codebase.

#define current_user "tester"
#define MAGIC_COLOUR 0xDEADBEEF

uint8_t (*current_mouse_design)[16] = mouse_default_design;

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

void change_cursor(char* cursor){
    if(cursor[0] == 'd' && cursor[1] == 'e' && cursor[2] == 'f'){
        current_mouse_design = mouse_default_design;
    } else if(cursor[0] == 't' && cursor[1] == 'y'){
        current_mouse_design = mouse_ibeam_design;
    } else if(cursor[0] == 'p' && cursor[1] == 'o' && cursor[2] == 'i'){
        current_mouse_design = mouse_pointer_design;
    }
}

// VBE Layers: wallpaper, apps, system
static uint32_t layer_wallpaper[1024*768];
static uint32_t layer_apps[1024*768];
static uint32_t layer_sys[1024*768];
static uint32_t final_composition_buffer[1024*768];

// Screen dimensions and UI state
int ui_needs_update = 0;
int screen_w        = 1024;
int screen_h        = 768;


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

void open_app(void (*app_function)()) {
    if (app_count < 10) {
        layer_apps_list[app_count] = app_function;
        app_count++;
        ui_needs_update = 1;
    }
}

void on_mouse_signal(int x, int y, int b){
    if((b & 1) && !mouse.last_left){ 
        UI_process_click(x, y); 
    }
    mouse.last_left = (b & 1);
    UI_update_cursor_state(mouse.x, mouse.y);
    ui_needs_update = 1;
}

void draw_ui_mouse_to_buffer(uint32_t* buffer, int x, int y){
    for(int i = 0; i < 16; i++){      // For each row of the cursor
        for(int j = 0; j < 16; j++){  // For each column of the cursor
            if((x + j) >= screen_w || (y + i) >= screen_h || (x + j) < 0 || (y + i) < 0) continue;

            uint8_t colour_type = current_mouse_design[i][j];
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

static UI_component quick_menu       = { .bounds = { .hidden = 1 } };
static UI_component alert_box        = { .bounds = { .hidden = 1 } };
static UI_component power_alert_box  = { .bounds = { .hidden = 1 } };
static UI_component sherlock         = { .bounds = { .hidden = 1 } };
char* power_alert_box_text           = "";

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

void mouse_test(){
    change_cursor("poi");
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
            .bg = rgba(0, 0, 0, 0),
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
            .textAlign = 1,
            .cursor = "poi"
        });
    }
}

void render_power_alert(){
    UI_Container(power_alert_box, ((RectArgs){
        .x = (screen_w / 2) - 150,
        .y = (screen_h / 2) - 100,
        .w = 300,
        .h = 200,
        .hidden = power_alert_box.bounds.hidden
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
            .bg = rgba(0, 0, 0, 0),
            .x = (300/2) - ((strlen(power_alert_box_text) * 8) / 2),
            .y = 96,
            .text = power_alert_box_text,
            .colour = rgb(0, 0, 0),
        });
    }
}
void show_power_alert(char* text){
    power_alert_box_text = text;
    UI_show(&power_alert_box);
}

void show_alert(){
    UI_show(&alert_box);
}

void shell_reboot(){
    show_power_alert("Rebooting...");
    power("reboot");
}
void shell_poweroff(){
    show_power_alert("Powering off...");
    power("off");
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
            .bg = rgb(221,221,221),
            .action = mouse_test
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
    draw_string(940,12,buf,rgb(0,0,0),rgba(0, 0, 0, 0));
    draw_string(956,12,":",rgb(0,0,0),rgba(0, 0, 0, 0));
    into_string(minutes,buf);
    draw_string(964,12,buf,rgb(0,0,0),rgba(0, 0, 0, 0));
}

// Sherlock is back!!
// (Just placeholder btw)
void render_sherlock(){
    UI_Container(sherlock, ((RectArgs){
        .w      = 300,
        .h      = 60,
        .x      = (screen_w / 2) - 200,
        .y      = 200,
        .hidden = sherlock.bounds.hidden
    })){
        UI_TextEntry((TextEntryArgs){
            .placeholder = "Sherlock search",
            .x = 0,
            .y = 0,
            .w = 400,
            .h = 60,
            .bg = rgba(255, 255, 255, 150),
            .colour = rgb(0, 0, 0),
            .rBotL = 20,
            .rBotR = 20,
            .rTopL = 20,
            .rTopR = 20
        });
    }
}

void toggle_sherlock(){
    if(sherlock.bounds.hidden){
        UI_show(&sherlock);
    } else {
        UI_hide(&sherlock);
    }
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
            .h = 130, 
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
            .bg = rgba(0, 0, 0, 0),
            .fg = rgb(0, 0, 0),
            .rTopL = 10,
            .rTopR = 10,
            .action = show_alert
        });
        UI_btn((ButtonArgs){
            .text = "Sherlock",
            .x = 1,
            .y = 35,
            .w = 118,
            .padding = 10,
            .fg = rgb(0,0,0),
            .bg = rgba(0, 0, 0, 0),
            .action = toggle_sherlock
        });
        UI_btn((ButtonArgs){
            .text = "Restart",
            .x = 1,
            .y = 65,
            .w = 118,
            .padding = 10,
            .fg = rgb(0,0,0),
            .bg = rgba(0, 0, 0, 0),
            .action = shell_reboot
        });
        UI_btn((ButtonArgs){
            .text = "Power Off",
            .x = 1,
            .y = 95,
            .w = 118,
            .padding = 10,
            .fg = rgb(0,0,0),
            .bg = rgba(0, 0, 0, 0),
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
            render_power_alert();
            render_sherlock();

            end_layer_draw();

            begin_layer_draw(layer_apps);
            clear_layer_magic(layer_apps);
            for(int i = 0; i < app_count; i++){
                layer_apps_list[i]();
            }
            end_layer_draw();

            compose_ui();

            UI_update_cursor_state(mouse.x, mouse.y);
            ui_needs_update = 0;
        }


        if(mouse.left && !mouse.last_left){
            UI_process_click(mouse.x, mouse.y);
            ui_needs_update = 1;
        }
        mouse.last_left = mouse.left;

        asm volatile("hlt");
    }
}