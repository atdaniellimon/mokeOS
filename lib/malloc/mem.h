#ifndef MEM_H
#define MEM_H

void heap_init();
void* kmalloc(unsigned int size);
void kfree(void* ptr);

#endif