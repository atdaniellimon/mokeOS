unsigned int free_mem_addr = 0x1000000;

void* kmalloc(unsigned int size) {
    void* ptr = (void*)free_mem_addr;

    free_mem_addr += size;

    if (free_mem_addr & 0x3) {
        free_mem_addr &= ~0x3;
        free_mem_addr += 0x4;
    }

    return ptr;
}