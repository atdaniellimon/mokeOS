#include "idt.h"
#include "../../arch/i386/io.h"
#include "../../drivers/vbe/vbe.h"
#include "../../drivers/screen/screen.h"
#include "../../lib/timer/timer.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags){
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

void pic_init(){
    // inicializar
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    
    // remapear
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    
    // configurar
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    outb(0x21, 0xFC);
    outb(0xA1, 0xEF);
}

void exception_handler(){
    k_print("Triple fault.\n");
    for(;;);
}

void idt_init(){
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base  = (uint32_t)&idt;

    for (int i = 0; i < 32; i++){
        idt_set_gate(i, (uint32_t)isr_exception, 0x08, 0x8E);
    }

    for (int i = 32; i < 256; i++){
        idt_set_gate(i, (uint32_t)isr_default, 0x08, 0x8E);
    }

    pic_init();
    idt_set_gate(32, (uint32_t)isr_timer, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)isr_keyboard, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)isr_mouse, 0x08, 0x8E);

    idt_load((uint32_t)&idt_ptr);
}