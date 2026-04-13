#include "dependencies/MokeUI.h"
#include "dependencies/string.h"

int current_val = 0;
int stored_val = 0;
char op = '\0';
char display[16] = "0";
int reset_display = 0;

void update_display(){
    into_string(current_val, display);
}

void on_click_num(int num){
    if(reset_display == 1){
        current_val = 0;
        reset_display = 0;
    }
    
    current_val = (current_val * 10) + num;
    update_display();
}

void on_click_op(char new_op){
    stored_val = current_val;
    op = new_op;
    reset_display = 1;
}

void on_click_calc(){
    if(op == '+'){
        current_val = stored_val + current_val;
    } else if(op == '-'){
        current_val = stored_val - current_val;
    } else if(op == '*'){
        current_val = stored_val * current_val;
    } else if(op == '/'){
        if(current_val != 0){
            current_val = stored_val / current_val;
        } else {
            current_val = 666; // Hail Satan because you divided by zero
        }
    }
    
    op = '\0';
    reset_display = 1;
    update_display();
}

void on_click_ac(){
    current_val = 0;
    stored_val = 0;
    op = '\0';
    update_display();
}

// Wrappers because C function pointers are a delight
void press_0(){ 
    on_click_num(0); 
}
void press_1(){ 
    on_click_num(1); 
}
void press_2(){ 
    on_click_num(2); 
}
void press_3(){ 
    on_click_num(3); 
}
void press_4(){ 
    on_click_num(4); 
}
void press_5(){ 
    on_click_num(5); 
}
void press_6(){ 
    on_click_num(6); 
}
void press_7(){ 
    on_click_num(7); 
}
void press_8(){ 
    on_click_num(8); 
}
void press_9(){ 
    on_click_num(9); 
}
void press_add(){ 
    on_click_op('+'); 
}
void press_sub(){ 
    on_click_op('-'); 
}
void press_mul(){ 
    on_click_op('*'); 
}
void press_div(){ 
    on_click_op('/'); 
}

int main(){
    while(1){
        UI_component calc_screen;
        UI_Container(calc_screen, ((RectArgs){
            .w = 200,
            .h = 50,
            .x = 10,
            .y = 10
        })){
            UI_rect((RectArgs){
                .w = 180,
                .h = 50,
                .x = 0,
                .y = 0,
                .bg = rgb(221, 221, 221),
                .rBotL = 20,
                .rBotR = 20,
                .rTopL = 20,
                .rTopR = 20
            });
            UI_text((TextArgs){
                .text = display,
                .colour = rgb(0, 0, 0),
                .x = 10,
                .y = 21,
            });
        }

        UI_component buttons_grid;
        UI_Container(buttons_grid, ((RectArgs){
            .w = 200,
            .h = 300,
            .x = 10,
            .y = 60
        })){
            // Row 1: The "I messed up" buttons
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .text = "AC", 
                .action = on_click_ac,
                .bg = rgb(165, 165, 165)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 150, 
                .text = "/", 
                .action = press_div,
                .bg = rgb(255, 159, 10), .fg = rgb(255, 255, 255)
            });

            // Row 2: High numbers for high expectations
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .y = 50, 
                .text = "7", 
                .action = press_7,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 50, 
                .y = 50, 
                .text = "8", 
                .action = press_8,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 100, 
                .y = 50, 
                .text = "9", 
                .action = press_9,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 150, 
                .y = 50, 
                .text = "*", 
                .action = press_mul,
                .bg = rgb(255, 159, 10), 
                .fg = rgb(255, 255, 255)
            });

            // Row 3: Middle class numbers
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .y = 100, 
                .text = "4", 
                .action = press_4,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 50, 
                .y = 100,
                .text = "5", 
                .action = press_5,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 100, 
                .y = 100, 
                .text = "6", 
                .action = press_6,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 150, 
                .y = 100, 
                .text = "-", 
                .action = press_sub,
                .bg = rgb(255, 159, 10), 
                .fg = rgb(255, 255, 255)
            });

            // Row 4: Barely numbers
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .y = 150, 
                .text = "1", 
                .action = press_1,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 50, 
                .y = 150, 
                .text = "2", 
                .action = press_2,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 100, 
                .y = 150, 
                .text = "3", 
                .action = press_3,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 50, 
                .h = 50, 
                .x = 150, 
                .y = 150, 
                .text = "+", 
                .action = press_add,
                .bg = rgb(255, 159, 10), 
                .fg = rgb(255, 255, 255)
            });

            // Row 5: The zero and the ego
            UI_btn((ButtonArgs){
                .w = 100, 
                .h = 50, 
                .y = 200, 
                .text = "0", 
                .action = press_0,
                .bg = rgb(51, 51, 51), 
                .fg = rgb(255, 255, 255)
            });
            UI_btn((ButtonArgs){
                .w = 100, 
                .h = 50, 
                .x = 100, 
                .y = 200, 
                .text = "=", 
                .action = on_click_calc,
                .bg = rgb(255, 159, 10), 
                .fg = rgb(255, 255, 255)
            });
        }
        
        // Wait for next frame or kernel will explode
        asm volatile ("hlt"); 
    }

    return 0 + 0;
}