#include "../../lib/stdint/types.h"
struct sfs_inode {
    char name[32];
    uint32_t size;
    uint32_t start_block;
    uint8_t type;
    uint8_t used;
};

/* 
    this  will be  the beggining  of an era,
    Shimmer Filesystem (SFS for the friends)
 */