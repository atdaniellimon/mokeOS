#ifndef IMPORT_H
#define IMPORT_H

#include "../stdint/types.h"

#define UI_Container(args) \
    UI_rect(args); \
    UI_push_context(args.x, args.y); \
    for(int _i = 1; _i > 0; _i--, UI_pop_context(args.x, args.y))

typedef struct {
    char* text;
    int x, y, w, h;
    uint32_t fg;
    uint32_t bg;
    int padding;
    int border;
    int rTopL, rTopR, rBotL, rBotR;
    void (*action)();
} ButtonArgs;

typedef struct {
    int x, y, w, h;
    uint32_t bg;
    uint32_t borderColour;
    int border;
    int rTopL, rTopR, rBotL, rBotR;
} RectArgs;

typedef struct {
    char* text;
    int x, y;
    uint32_t colour;
    uint32_t bg;
} TextArgs;

void UI_btn(ButtonArgs args);
void UI_rect(RectArgs args);
void UI_text(TextArgs args);
void UI_push_context(int x, int y);
void UI_pop_context(int x, int y);

#endif