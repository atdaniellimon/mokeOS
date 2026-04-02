#ifndef ELF_H
#define ELF_H
#include "../stdint/types.h"
#include "../paging/paging.h"

#define ELF_MAGIC 0x464C457F  // "\x7FELF"

typedef struct {
    uint32_t magic;
    uint8_t  bits;        // 1 = 32bit, 2 = 64bit
    uint8_t  endian;
    uint8_t  version;
    uint8_t  os_abi;
    uint8_t  padding[8];
    uint16_t type;        // 2 = executable
    uint16_t machine;     // 3 = x86
    uint32_t version2;
    uint32_t entry;
    uint32_t ph_offset;
    uint32_t sh_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_entry_size;
    uint16_t ph_count;
    uint16_t sh_entry_size;
    uint16_t sh_count;
    uint16_t sh_str_idx;
} __attribute__((packed)) elf_header_t;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} __attribute__((packed)) elf_program_header_t;

uint32_t elf_load(void* elf_data, page_directory_t* dir);

#endif