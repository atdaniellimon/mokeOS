#include "instance.h"
#include "../string/string.h"

void render_app_window(MokeApp* app) {
    if(app->app_type == 0){
        UI_Container(app->window_comp, ((RectArgs){
            .w = app->w,
            .h = app->h,
            .x = app->x,
            .y = app->y,
            .hidden = app->is_minimized
        })){
            static UI_component window_top_bar;

            UI_Container(window_top_bar, ((RectArgs){
                .w = ((app->w) - 14),
                .h = 40,
                .x = 7,
                .y = 7
            })){
                UI_rect((RectArgs){
                    .w = ((app->w) - 14),
                    .h = 40,
                    .x = 0,
                    .y = 0,
                    .bg = rgba(255, 255, 255, 120),
                    .rBotL = 25,
                    .rTopL = 25,
                    .rBotR = 25,
                    .rTopR = 25
                });
                UI_btn((ButtonArgs){
                    .w = 20,
                    .h = 20,
                    .x = 10,
                    .y = 10,
                    .rBotL = 11,
                    .rTopL = 11,
                    .rBotR = 11,
                    .rTopR = 11,
                    .bg = rgb(255, 81, 81),
                    .cursor = "poi"
                });
                UI_btn((ButtonArgs){
                    .w = 20,
                    .h = 20,
                    .x = 40,
                    .y = 10,
                    .rBotL = 11,
                    .rTopL = 11,
                    .rBotR = 11,
                    .rTopR = 11,
                    .bg = rgb(255, 214, 81),
                    .cursor = "poi"
                });
                UI_btn((ButtonArgs){
                    .w = 20,
                    .h = 20,
                    .x = 70,
                    .y = 10,
                    .rBotL = 11,
                    .rTopL = 11,
                    .rBotR = 11,
                    .rTopR = 11,
                    .bg = rgb(92, 217, 75),
                    .cursor = "poi"
                });
                UI_text((TextArgs){
                    .y = 16,
                    .x = (((app->w) / 2) - ((strlen(app->name) * 8) / 2)),
                    .text = app->name,
                    .colour = rgb(0, 0, 0)
                });
            }

            app->update_logic(); 
        }
    } else {
        UI_Container(app->window_comp, ((RectArgs){
            .w = app->w,
            .h = app->h,
            .x = app->x,
            .y = app->y,
            .hidden = app->is_minimized
        })){
            static UI_component window_top_bar;

            UI_Container(window_top_bar, ((RectArgs){
                .w = 100,
                .h = 40,
                .x = 7,
                .y = 7
            })){
                UI_rect((RectArgs){
                    .w = 100,
                    .h = 40,
                    .x = 0,
                    .y = 0,
                    .bg = rgba(255, 255, 255, 120),
                    .rBotL = 25,
                    .rTopL = 25,
                    .rBotR = 25,
                    .rTopR = 25
                });
                UI_btn((ButtonArgs){
                    .w = 20,
                    .h = 20,
                    .x = 10,
                    .y = 10,
                    .rBotL = 11,
                    .rTopL = 11,
                    .rBotR = 11,
                    .rTopR = 11,
                    .bg = rgb(255, 81, 81),
                    .cursor = "poi"
                });
                UI_btn((ButtonArgs){
                    .w = 20,
                    .h = 20,
                    .x = 40,
                    .y = 10,
                    .rBotL = 11,
                    .rTopL = 11,
                    .rBotR = 11,
                    .rTopR = 11,
                    .bg = rgb(255, 214, 81),
                    .cursor = "poi"
                });
                UI_btn((ButtonArgs){
                    .w = 20,
                    .h = 20,
                    .x = 70,
                    .y = 10,
                    .rBotL = 11,
                    .rTopL = 11,
                    .rBotR = 11,
                    .rTopR = 11,
                    .bg = rgb(92, 217, 75),
                    .cursor = "poi"
                });
            }

            app->update_logic(); 
        }
    }
}