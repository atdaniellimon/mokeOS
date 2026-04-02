#include "elf.h"
#include "../paging/paging.h"
#include "../string/string.h"

extern uint32_t alloc_frame(void);

uint32_t elf_load(void* elf_data, page_directory_t* dir){
    elf_header_t* header = (elf_header_t*)elf_data;

    if(header->magic != ELF_MAGIC) return 0;
    if(header->machine != 3) return 0; // not x86 so screw it

    for(int i = 0; i < header->ph_count; i++){
        elf_program_header_t* ph = (elf_program_header_t*)(
            (uint32_t)elf_data + header->ph_offset + 
            (i * header->ph_entry_size)
        );

        if(ph->type != 1) continue;

        uint8_t* src  = (uint8_t*)elf_data + ph->offset;
        uint8_t* dest = (uint8_t*)ph->vaddr;

        for(uint32_t addr = ph->vaddr; addr < ph->vaddr + ph->memsz; addr += 4096){
            uint32_t phys = alloc_frame();
            paging_map(dir, addr, phys, 1); // 1 = user accessible
        }

        memcpy(dest, src, ph->filesz);

        if(ph->memsz > ph->filesz){
            memset(dest + ph->filesz, 0, ph->memsz - ph->filesz);
        }
    }

    return header->entry;
}