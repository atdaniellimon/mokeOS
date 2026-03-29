#include "mem.h"
#include "../stdint/types.h"
#define HEAP_START 0x1000000
#define HEAP_SIZE  0x1000000

typedef struct block_header {
    uint32_t size;
    uint32_t free; // 1 = free, 0 = not free
    struct block_header* next;
} block_header_t;

static block_header_t* heap_start = 0;

void heap_init(){
    heap_start = (block_header_t*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(block_header_t);
    heap_start->free = 1;
    heap_start->next = 0;
}

void* kmalloc(unsigned int size){
    size = (size + 3) & ~0x3;

    block_header_t* current = heap_start;

    while(current){
        if(current->free && current->size >= size){
            if(current->size >= size + sizeof(block_header_t) + 4){
                block_header_t* new_block =
                    (block_header_t*)((uintptr_t)current + sizeof(block_header_t) + size);

                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->free = 1;
                new_block->next = current->next;

                current->next = new_block;
                current->size = size;
            }

            current->free = 0;

            return (void*)((uintptr_t)current + sizeof(block_header_t));
        }

        current = current->next;
    }

    return 0;
}

void kfree(void* ptr){
    if(!ptr) return;

    if((uintptr_t)ptr < HEAP_START || (uintptr_t)ptr >= HEAP_START + HEAP_SIZE){
        return;
    }

    block_header_t* block = (block_header_t*)((uintptr_t)ptr - sizeof(block_header_t));

    if(block->free) return;

    block->free = 1;

    block_header_t* current = heap_start;

    while(current && current->next){
        if(current->free && current->next->free){
            current->size += sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}