#include "moke.h"
#include "../../lib/stdint/types.h"
#include "../../arch/i386/io.h"
#include "../../lib/string/string.h"
#include "../../lib/malloc/mem.h"

#define MFS_MAGIC 0x454B4F4D // "MOKE" but reversed because why not
#define MAX_MFS_FILES 128
#define MFS_SUPERBLOCK_LBA 1
#define MFS_TABLE_LBA 2
#define MFS_TABLE_SECTORS 16

static struct mfs_superblock mfs_sb;
static int mfs_ready = 0;
static int ata_ready = 0;

struct mfs_entry mfs_table[MAX_MFS_FILES];


int ata_init(){
    //select master drive (obviously the holy Moke FileSystem)
    outb(0x1F6, 0xA0);

    //Send identify cmd
    outb(0x1F7, 0xEC);

    //if no device then u fucked up
    if(inb(0x1F7) == 0){
        return 0;
    }

    while(1){
        uint8_t status = inb(0x1F7);
        
        if(status & 0x01){
            return 0;
        }
        if(!(status & 0x80) && (status & 0x08)){
            break;
        }
    }
    //read identify data & ignore content for now
    for(int i = 0; i < 256; i++){
        inw(0x1F0);
    }
    ata_ready = 1;
    return 1;
}

// ATA PIO read (because drivers are optional, right?)
void ata_read_sector(uint32_t lba, uint8_t *buffer){
    if(!ata_ready) return; //insert error code here lololol
    outb(0x1F6, (0xE0 | ((lba >> 24) & 0x0F)));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    // wait until the disk feels like responding
    while(1){
        uint8_t status = inb(0x1F7);

        if(status & 0x01) return; // error
        if(!(status & 0x80) && (status & 0x08)) break;
    }

    for(int i = 0; i < 256; i++){
        uint16_t data = inw(0x1F0);
        buffer[i*2] = (uint8_t)data;
        buffer[i*2 + 1] = (uint8_t)(data >> 8);
    }
}


// Load file table into RAM (because disk is slow and we pretend we care)
void mfs_load_table(){
    uint8_t buffer[512];

    for(int s = 0; s < MFS_TABLE_SECTORS; s++){
        ata_read_sector(MFS_TABLE_LBA + s, buffer);

        memcpy(
            ((uint8_t*)mfs_table) + (s * 512),
            buffer,
            512
        );
    }
}


// Initialize filesystem (aka "please don't be corrupted")
int mfs_mount(){
    uint8_t buffer[512];

    ata_read_sector(MFS_SUPERBLOCK_LBA, buffer);
    memcpy(&mfs_sb, buffer, sizeof(struct mfs_superblock));

    if(mfs_sb.magic != MFS_MAGIC){
        // congrats, your disk is trash
        return 0;
    }

    if(mfs_sb.total_sectors == 0){
        return 0;
    }

    if(mfs_sb.file_count > MAX_MFS_FILES){
        return 0;
    }

    // now we actually load something useful
    mfs_load_table();

    mfs_ready = 1;
    return 1;
}


// Find file by name (linear search, because performance is optional)
int mfs_find_file(const char* filename, struct mfs_entry* out){
    if(!mfs_ready) return 0;

    for(int i = 0; i < MAX_MFS_FILES; i++){
        if(mfs_table[i].flags == 1){
            if(strcmp(mfs_table[i].name, filename) == 0){
                memcpy(out, &mfs_table[i], sizeof(struct mfs_entry));
                return 1;
            }
        }
    }
    return 0;
}


// Read file into existing buffer
int mfs_read_file(struct mfs_entry* file, uint8_t* destination){
    if(!mfs_ready) return 0;

    uint32_t sectors = (file->size + 511) / 512;

    for(uint32_t i = 0; i < sectors; i++){
        ata_read_sector(file->start_lba + i, destination + (i * 512));
    }

    return 1;
}


// Load file into RAM (finally something useful)
void* mfs_load_to_ram(char* filename){
    if(!mfs_ready) return 0;

    struct mfs_entry file;

    if(!mfs_find_file(filename, &file)){
        // file not found, shocking
        return 0;
    }

    uint32_t size = file.size;
    uint32_t sectors = (size + 511) / 512;

    void* buffer = kmalloc(sectors * 512);
    if(!buffer){
        // out of memory, skill issue
        return 0;
    }

    mfs_read_file(&file, buffer);

    return buffer;
}


// List files (primitive "ls", but at least not printing like a caveman)
int mfs_list_files(struct mfs_entry* out_array, int max){
    if(!mfs_ready) return 0;

    int count = 0;

    for(int i = 0; i < MAX_MFS_FILES && count < max; i++){
        if(mfs_table[i].flags == 1){
            memcpy(&out_array[count], &mfs_table[i], sizeof(struct mfs_entry));
            count++;
        }
    }

    return count;
}

// Debug only (yes, this breaks your own architecture, but it's fine for now lol)
void mfs_debug_list_files(){
    for(int i = 0; i < MAX_MFS_FILES; i++){
        if(mfs_table[i].flags == 1){
            //k_print(mfs_table[i].name);
            //k_print("\n");
        }
    }
}

int mfs_find_dir_index(const char* dirname){
    if(!mfs_ready) return -1;

    for(int i = 0; i < MAX_MFS_FILES; i++) {
        if(mfs_table[i].flags == 2) { // We only look for inverted
            if(strcmp(mfs_table[i].name, dirname) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int mfs_list_files_by_dir(int32_t parent_id, struct mfs_entry* out_array, int max){
    if(!mfs_ready) return 0;

    int count = 0;
    for(int i = 0; i < MAX_MFS_FILES && count < max; i++) {
        // If its flag is file/dir and the parent_id is the same
        if(mfs_table[i].flags != 0 && mfs_table[i].parent_id == parent_id) {
            memcpy(&out_array[count], &mfs_table[i], sizeof(struct mfs_entry));
            count++;
        }
    }
    return count;
}