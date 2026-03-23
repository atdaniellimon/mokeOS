#ifndef GDT_H
#define GDT_H

#include "../../lib/stdint/types.h"

/* Estructura de una entrada de la GDT */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

/* Estructura del puntero que GDTR espera */
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Funciones que expondremos al kernel */
void gdt_init();
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

/* Función definida en gdt.asm */
extern void gdt_flush(uint32_t);

#endif