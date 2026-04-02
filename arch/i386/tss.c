#include "tss.h"
#include "gdt.h"

tss_entry_t tss;

void tss_init(uint32_t kernel_stack){
    uint32_t base  = (uint32_t)&tss;
    uint32_t limit = base + sizeof(tss_entry_t);

    gdt_set_gate(5, base, limit, 0x89, 0x00);

    tss.ss0  = 0x10;
    tss.esp0 = kernel_stack;
    tss.cs   = 0x0B;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
}

void tss_set_stack(uint32_t stack){
    tss.esp0 = stack;
}