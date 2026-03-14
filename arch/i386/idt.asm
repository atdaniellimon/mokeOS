global idt_load
global isr_keyboard
global isr_timer

extern keyboard_handler
extern timer_handler

idt_load:
    mov eax, [esp+4]
    lidt [eax]
    sti
    ret

isr_timer:
    pusha
    call timer_handler
    popa
    iret

isr_keyboard:
    pusha
    call keyboard_handler
    popa
    iret