#ifndef PAGING_H
#define PAGING_H
#include "../stdint/types.h"

typedef struct {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;
} __attribute__((packed)) page_entry_t;

typedef struct {
    page_entry_t entries[1024];
} __attribute__((aligned(4096))) page_table_t;

typedef struct {
    uint32_t entries[1024];
} __attribute__((aligned(4096))) page_directory_t;

void paging_init();
page_directory_t* paging_create_directory();
void paging_map(page_directory_t* dir, uint32_t virt, uint32_t phys, int user);
void paging_load(page_directory_t* dir);
void paging_enable();

extern page_directory_t* kernel_directory;

#endif