#ifndef MFS_H
#define MFS_H

#include "../../lib/stdint/types.h"

#define MFS_MAGIC 0x454B4F4D
#define MAX_MFS_FILES 128
#define MFS_SUPERBLOCK_LBA 1
#define MFS_TABLE_LBA 2

struct __attribute__((packed)) mfs_superblock {
    uint32_t magic;           // should be MFS_MAGIC (if not, good luck)
    uint32_t total_sectors;
    uint32_t free_sectors;
    uint32_t file_count;      // number of files (hopefully accurate)
};

struct __attribute__((packed)) mfs_entry {
    char name[32];
    uint32_t start_lba;       // where the file starts (no fragmentation pls)
    uint32_t size;            // size in bytes
    uint32_t flags;           // 0 = free, 1 = file, 2 = dir (dirs are a lie for now)
    int32_t parent_id;        // -1 for root
    uint8_t reserved[16];     // future me will deal with this
};

// --- core ---
int mfs_mount();
int ata_init();

int mfs_find_file(const char* filename, struct mfs_entry* out);
int mfs_find_dir_index(const char* dirname);
int mfs_list_files_by_dir(int32_t parent_id, struct mfs_entry* out_array, int max);
int mfs_read_file(struct mfs_entry* file, uint8_t* destination);
void* mfs_load_to_ram(char* filename);

int mfs_list_files(struct mfs_entry* out_array, int max);

void mfs_debug_list_files();

#endif