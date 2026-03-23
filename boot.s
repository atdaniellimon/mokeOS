MBOOT_MAGIC    equ 0x1BADB002
MBOOT_FLAGS    equ 0x00000007
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM
    dd 0, 0, 0, 0, 0
    dd 0
    dd 1024
    dd 768
    dd 32

section .text
extern main
global _start

_start:
    cli
    mov esp, stack_top
    push eax
    push ebx
    call main

halt:
    hlt
    jmp halt


section .bss
align 16
stack_bottom:
    resb 16384
stack_top: