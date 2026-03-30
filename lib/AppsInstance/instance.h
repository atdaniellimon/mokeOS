#ifndef APPSINSTANCE_H
#define APPS_INSTANCE_H

#include "../stdint/types.h"
#include "../mokeUI/import.h"

typedef struct {
    char name[32];
    int x, y, w, h;
    int is_minimized;
    int is_focused;
    uint32_t* icon_data;
    void (*update_logic)();
    UI_component window_comp;
    int app_type; // 0 = normal, 1 = full
} MokeApp;
MokeApp running_apps[10];

#endif