#include "../../lib/stdint/types.h"
#include "../../arch/i386/io.h"
//Simple mokeOS filesystem for testing & debugging

#define MFS_MAGIC 0x454B4F4D // Magic keyword just like boot.s ig
#define MAX_MFS_FILES 128
#define MFS_TABLE_LBA 2 //Files are in sector 2

struct __attribute__((packed)) mfs_superblock {
    uint32_t magic; // should be MFS_MAGIC
    uint32_t total_sectors;
    uint32_t free_sectors;
    uint32_t file_count;
};

struct __attribute__((packed)) mfs_entry {
    char name[32];
    uint32_t start_lba; //Start sector
    uint32_t size; //Size in bytes
    uint32_t flags; //If 0 then it's free, if 1 file and if 2 dir
    uint8_t reserved[20]; // Padding
};

//Read a sector as it name says
void ata_read_sector(uint32_t lba, uint8_t *buffer){
    outb(0x1F6, (0xE0 | ((lba >> 24) & 0x0F)));
    outb(0x1F2, 1); //read a sector
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    while((inb(0x1F7) & 0x88) != 0x08);

    for(int i = 0; i < 256; i++){
        uint16_t data = inw(0x1F0);
        buffer[i*2] = (uint8_t)data;
        buffer[i*2 + 1] = (uint8_t)(data >> 8);
    }
}

int mfs_find_file(const char* filename, struct mfs_entry* out_entry){
    static uint8_t sector_buffer[512];
    
    // We check sectors from 2 to 5 (until MAX_MFS_FILES)
    for (int s = 0; s < 16; s++) {
        ata_read_sector(MFS_TABLE_LBA + s, sector_buffer);
        struct mfs_entry *entries = (struct mfs_entry*)sector_buffer;

        for (int i = 0; i < 8; i++) {
            if (entries[i].flags == 1 && strcmp(entries[i].name, filename) == 0) {
                memcpy(out_entry, &entries[i], sizeof(struct mfs_entry));
                return 1;
            }
        }
    }
    return 0; // Not found
}

// This function is to read a file content
void mfs_read_file(struct mfs_entry* file, uint8_t* destination) {
    uint32_t sectors_to_read = (file->size + 511) / 512;
    
    for (uint32_t i = 0; i < sectors_to_read; i++) {
        ata_read_sector(file->start_lba + i, destination + (i * 512));
    }
}