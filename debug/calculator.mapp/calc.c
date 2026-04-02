#include "dependencies/MokeApp.h"

int current_val = 0;
int stored_val  = 0;
char op         = '\0';
char display[16] = "0";
int reset_display = 0;

void num_to_str(int n, char* buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[12];
    int i = 0, j = 0;
    int is_negative = 0;

    if (n < 0) { is_negative = 1; n = -n; }

    while (n > 0) { tmp[i++] = (n % 10) + '0'; n /= 10; }
    if (is_negative) buf[j++] = '-';
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void update_display() {
    num_to_str(current_val, display);
}

void do_operation() {
    if (op == '+') current_val = stored_val + current_val;
    else if (op == '-') current_val = stored_val - current_val;
    else if (op == '*') current_val = stored_val * current_val;
    else if (op == '/') {
        if (current_val != 0) current_val = stored_val / current_val;
        else current_val = 0;
    }
    update_display();
}

void draw_ui() {
    app_draw_rect(90, 90, 220, 300, 0x1A1A1A);
    
    app_draw_rect(100, 100, 200, 50, 0xEEEEEE);
    app_draw_string(110, 118, display, 0x000000);

    app_draw_rect(100, 160, 45, 45, 0x333333); app_draw_string(118, 175, "7", 0xFFFFFF);
    app_draw_rect(155, 160, 45, 45, 0x333333); app_draw_string(173, 175, "8", 0xFFFFFF);
    app_draw_rect(210, 160, 45, 45, 0x333333); app_draw_string(228, 175, "9", 0xFFFFFF);
    app_draw_rect(265, 160, 45, 45, 0xFF9500); app_draw_string(283, 175, "/", 0xFFFFFF);

    app_draw_rect(100, 215, 45, 45, 0x333333); app_draw_string(118, 230, "4", 0xFFFFFF);
    app_draw_rect(155, 215, 45, 45, 0x333333); app_draw_string(173, 230, "5", 0xFFFFFF);
    app_draw_rect(210, 215, 45, 45, 0x333333); app_draw_string(228, 230, "6", 0xFFFFFF);
    app_draw_rect(265, 215, 45, 45, 0xFF9500); app_draw_string(283, 230, "*", 0xFFFFFF);

    app_draw_rect(100, 270, 45, 45, 0x333333); app_draw_string(118, 285, "1", 0xFFFFFF);
    app_draw_rect(155, 270, 45, 45, 0x333333); app_draw_string(173, 285, "2", 0xFFFFFF);
    app_draw_rect(210, 270, 45, 45, 0x333333); app_draw_string(228, 285, "3", 0xFFFFFF);
    app_draw_rect(265, 270, 45, 45, 0xFF9500); app_draw_string(283, 285, "-", 0xFFFFFF);

    app_draw_rect(100, 325, 45, 45, 0xA5A5A5); app_draw_string(118, 340, "C", 0x000000);
    app_draw_rect(155, 325, 45, 45, 0x333333); app_draw_string(173, 340, "0", 0xFFFFFF);
    app_draw_rect(210, 325, 45, 45, 0xFF9500); app_draw_string(228, 340, "=", 0xFFFFFF);
    app_draw_rect(265, 325, 45, 45, 0xFF9500); app_draw_string(283, 340, "+", 0xFFFFFF);
}

void _start() {
    update_display();
    
    while(1) {
        draw_ui();

        char key = app_get_key();
        
        if (key >= '0' && key <= '9') {
            if (reset_display) {
                current_val = 0;
                reset_display = 0;
            }
            current_val = (current_val * 10) + (key - '0');
            update_display();
        } 
        else if (key == '+' || key == '-' || key == '*' || key == '/') {
            stored_val = current_val;
            op = key;
            reset_display = 1;
        } 
        else if (key == '=' || key == '\n') {
            do_operation();
            op = '\0';
            reset_display = 1;
        } 
        else if (key == 'c' || key == 'C') {
            current_val = 0;
            stored_val = 0;
            op = '\0';
            update_display();
        }
        else if (key == 27) {
            app_exit();
        }
    }
}