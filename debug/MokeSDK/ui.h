#ifndef SYSTEM_UI_FRAMEWORK
#define SYSTEM_UI_FRAMEWORK

#include "../../lib/stdint/types.h"
#define rgba(r, g, b, a) ((uint32_t)((((a)&0xFF)<<24)|(((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF)))
#define rgb(r, g, b)     rgba(r, g, b, 255)
typedef struct {
    int x, y;
    bool left_click;
} MouseState;

typedef struct {
    void (*execute)(void*);
    void* args;
} UI_Event;

#define Call(function, arg) (UI_Event){ .execute = (void (*)(void*))(function), .args = (void*)(arg) }
#define RadiusAll(r) (Corners){.tl = r, .tr = r, .bl = r, .br = r}
static bool sdk_is_drawing = 0;

/*
	This  are  styles that can be choosen
	by the developer / user / environment.
*/

typedef struct {
    int x, y;
} __attribute__((packed)) Point;

typedef struct {
    uint32_t colour;
} __attribute__((packed)) TextStyle;

typedef struct {
    int w, h;
} __attribute__((packed)) Size;

typedef struct {
    uint32_t fill;
    uint32_t stroke;
    int thickness;
} __attribute__((packed)) Style;

typedef struct {
    int tl, tr, bl, br;
} __attribute__((packed)) Corners;

/*
	This are the UI elements themselves.
*/

/*
	Rectangle: Just a rectangle
*/
typedef struct {
    Point pos;
    Size size;
    Style style;
    Corners radius;
    bool is_hidden;
} __attribute__((packed)) Rectangle;

/*
	Text: Allows developers / environment
	to write  text  into the screen  with
	interruptions.
*/
typedef struct {
    char* text;
    Point pos;
    TextStyle style;
    bool is_hidden;
} __attribute__((packed)) Text;

/*
	Button:  Allows users to  trigger  an
	action determined by the developer or
	environment.
*/
typedef struct {
	char* content;
	Point pos;
	Size size;
	Style style;
	Corners radius;
	UI_Event onclick;
	bool is_hidden;
} __attribute__((packed)) Button;

typedef struct {
    Rectangle bounds;
} UI_component;

#define UI_container(rect) \
    for(int _i = 1; _i > 0 && !(rect).is_hidden; _i = 0) \
        for(int _j = (UI_rect(rect), UI_PushContext((rect).pos), 1); \
             _j > 0; \
             _j = (UI_PopContext((rect).pos), 0))

void UI_refresh();
void UI_text(Text t);
bool UI_check_dirty();
void UI_PopContext(Point p);
void UI_PushContext(Point p);
void UI_rect(Rectangle rect);
void UI_hide(UI_component* c);
void UI_show(UI_component* c);
void UI_toggle(UI_component* c);
void UI_end_frame(MouseState* ms);
void UI_get_mouse(MouseState* out_ms);
void UI_set_drawing_mode(bool draw_mode);
void UI_button(Button btn, MouseState ms);
void UI_ProcessButton(Button btn, MouseState ms);
bool UI_button_clicked(Rectangle rect, MouseState ms);

void poweroff();

#endif