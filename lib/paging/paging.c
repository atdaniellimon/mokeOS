#include "paging.h"
#include "../malloc/mem.h"
#include "../../lib/string/string.h"

page_directory_t* kernel_directory = 0;
page_directory_t* current_directory = 0;

static uint32_t phys_addr = 0x400000; // starts in 4MB

uint32_t alloc_frame(){
    uint32_t addr = phys_addr;
    phys_addr += 4096;
    return addr;
}

void paging_map(page_directory_t* dir, uint32_t virt, uint32_t phys, int user){
    uint32_t pd_idx = virt >> 22;         // bits 31-22
    uint32_t pt_idx = (virt >> 12) & 0x3FF; // bits 21-12

    // si no hay page table, crearla
    if(!(dir->entries[pd_idx] & 1)){
        page_table_t* table = (page_table_t*)kmalloc(sizeof(page_table_t));
        memset(table, 0, sizeof(page_table_t));
        dir->entries[pd_idx] = ((uint32_t)table & ~0xFFF) | 0x7; // present, rw, user
    }

    page_table_t* table = (page_table_t*)(dir->entries[pd_idx] & ~0xFFF);
    table->entries[pt_idx].present = 1;
    table->entries[pt_idx].rw      = 1;
    table->entries[pt_idx].user    = user ? 1 : 0;
    table->entries[pt_idx].frame   = phys >> 12;
}

void paging_load(page_directory_t* dir){
    current_directory = dir;
    asm volatile("mov %0, %%cr3" :: "r"(dir));
}

void paging_enable(){
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // bit 31 = paging enable
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

page_directory_t* paging_create_directory(){
    page_directory_t* dir = (page_directory_t*)kmalloc(sizeof(page_directory_t));
    memset(dir, 0, sizeof(page_directory_t));
    return dir;
}

void paging_init(){
    kernel_directory = paging_create_directory();

    for(uint32_t i = 0; i < 0x1000000; i += 4096){
        paging_map(kernel_directory, i, i, 0); //kernel, not user
    }

    for(uint32_t i = 0xFD000000; i < 0xFD300000; i += 4096){
        paging_map(kernel_directory, i, i, 0);
    }

    paging_load(kernel_directory);
    paging_enable();
}