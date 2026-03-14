#include "hardware.h"
#include "../../lib/string/string.h"

struct mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

void get_ram(void* mbi_ptr, char* buffer_res) {
    unsigned int flags = *(unsigned int*)mbi_ptr;
    unsigned long long total_bytes = 0;

    if (flags & (1 << 6)) {
        unsigned int mmap_len  = *(unsigned int*)(mbi_ptr + 44);
        unsigned int mmap_addr = *(unsigned int*)(mbi_ptr + 48);
        struct mmap_entry* entry = (struct mmap_entry*)mmap_addr;

        while ((unsigned int)entry < (mmap_addr + mmap_len)) {
            if (entry->type == 1) {
                total_bytes += entry->len;
            }
            entry = (struct mmap_entry*)((unsigned int)entry + entry->size + 4);
        }
    }

    unsigned int total_mb = (unsigned int)(total_bytes / 1048576);
    into_string(total_mb, buffer_res);
}