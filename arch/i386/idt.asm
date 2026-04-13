global idt_load
global isr_keyboard
global isr_timer

extern keyboard_handler
extern timer_handler

idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ;sti
    ret

isr_timer:
    pusha
    call timer_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

isr_keyboard:
    pusha
    call keyboard_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

global isr_default
isr_default:
    push eax
    mov al, 0x20
    out 0x20, al
    out 0xA0, al
    pop eax
    iret

global isr_exception
extern exception_handler

isr_exception:
    pusha
    call exception_handler
    popa
    add esp, 4
    iret

global isr_mouse
extern mouse_handler
isr_mouse:
    pusha
    call mouse_handler
    mov al, 0x20
    out 0xA0, al
    out 0x20, al
    popa
    iret

global isr_syscall
extern syscall_handler

isr_syscall:
    pusha            ; Guarda registros generales
    push ds          ; Guarda segmentos
    push es
    push fs
    push gs

    mov ax, 0x10     ; Cargar selector de datos del Kernel
    mov ds, ax
    mov es, ax

    push esp         ; Pasa el puntero a los registros al C
    call syscall_handler
    add esp, 4       ; Limpia el argumento

    pop gs           ; Restaura en orden inverso exacto
    pop fs
    pop es
    pop ds
    popa
    iret             ; Vuelve a la Shell