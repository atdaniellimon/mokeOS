global jump_to_usermode

jump_to_usermode:
    mov ebx, [esp + 4]
    mov ecx, [esp + 8]

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23
    push ecx
    
    pushf
    pop eax
    or eax, 0x200
    push eax
    
    push 0x1B
    push ebx

    iret