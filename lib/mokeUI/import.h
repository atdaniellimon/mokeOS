#ifndef IMPORT_H
#define IMPORT_H

#include "../stdint/types.h"


typedef enum {
    LAYER_WALLPAPER = 0,
    LAYER_APPS      = 1,
    LAYER_SYSTEM    = 2,
    LAYER_CURSOR    = 3,
    LAYER_COUNT     = 4
} LayerID;

typedef struct {
    int x, y, w, h;
    uint32_t bg;
    uint32_t borderColour;
    int border;
    int rTopL, rTopR, rBotL, rBotR;
    int layer;
    int hidden;
} RectArgs;

typedef struct {
    int x, y, w, h;
    void (*action)();
} MokeButton;

typedef struct {
    RectArgs bounds;
} UI_component;

#define UI_Container(comp, args) \
    comp.bounds = args; \
    for (int _i = (!comp.bounds.hidden ? (UI_push_context(comp.bounds.x, comp.bounds.y), 1) : 0); \
         _i > 0; \
         _i = (UI_pop_context(comp.bounds.x, comp.bounds.y), 0))

typedef struct {
    char* text;
    int x, y, w, h;
    uint32_t fg;
    uint32_t bg;
    int padding;
    int border;
    int rTopL, rTopR, rBotL, rBotR;
    void (*action)();
    LayerID layer;
    int hidden;
    int textAlign;
} ButtonArgs;

typedef struct {
    char* text;
    int x, y;
    uint32_t colour;
    uint32_t bg;
    LayerID layer;
    int hidden;
} TextArgs;

void UI_btn(ButtonArgs args);
void UI_rect(RectArgs args);
void UI_text(TextArgs args);
void UI_push_context(int x, int y);
void UI_pop_context(int x, int y);
void UI_hide(UI_component* c);
void UI_show(UI_component* c);

void clear_buttons();
void add_button(int x, int y, int w, int h, void (*action)());
void UI_process_click(int mx, int my);

#endif