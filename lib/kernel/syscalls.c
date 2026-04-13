/*
    mokeOS' syscall manager
    Version Beta
    Made by Daniel Limon for MokeOS

    See more at daniellimon.github.io
    Target OS: mokeOS
*/

#include "syscalls.h"
#include "graphics/picasso.h"
#include "../../arch/i386/io.h"
#include "../../drivers/vbe/vbe.h"
#include "../../debug/MokeSDK/ui.h"
#include "../../drivers/mouse/ps2.h"

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax; 
    uint32_t eip, cs, eflags, useresp, ss; 
} registers_t;

/*
    This function is called by assembly in
    order to manage the interruptions made
    by the apps or processes inside ring 3.
*/
void syscall_handler(registers_t* regs){
    switch (regs->eax){
        case SYS_UI_RECT: {
            Rectangle* r = (Rectangle*)regs->ebx;

            if ((uint32_t)r < 0x1000) break;

            /*
                Draw a rounded rectangle (or a non rounded)
                depending  on  the specified  configuration.
            */
            draw_rect_rounded(
                r->pos.x, r->pos.y, 
                r->size.w, r->size.h, 
                r->style.fill, 
                r->radius.tl, r->radius.tr, 
                r->radius.bl, r->radius.br
            );
            break;
        }

        case 11: { // SYS_UI_TEXT
            Text* t = (Text*)regs->ebx;
            if ((uint32_t)t < 0x1000) break;
    
            /*
                This draws a string into  the  framebuffer.
            */
            draw_string(t->pos.x, t->pos.y, t->text, t->style.colour, 0x0); 
            break;
        }

        case 12: // SYS_UI_REFRESH
            vbe_swap();
            break;

        case 13: { // SYS_GET_MOUSE
            MouseState* user_ms = (MouseState*)regs->ebx;
            if ((uint32_t)user_ms < 0x1000) break;

            extern mouse_state_t mouse;
    
            user_ms->x = mouse.x;
            user_ms->y = mouse.y;
            user_ms->left_click = (mouse.left != 0); 
            break;
        }

        case SYS_REBOOT:
            outb(0x64, 0xFE);
            break;

        case SYS_SHUTDOWN:
            outw(0x604, 0x2000);
            outw(0x4004, 0x3400);
            outw(0xB004, 0x2000);
            break;

        default:
            break;
    }
}