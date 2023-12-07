#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <superblock.h>
#include <bitmap.h>

typedef struct fileSystem
{
    superblock_t superblock;
    bitmap_t block_bitmap;
    n
}fileSystem_t;

#endif
