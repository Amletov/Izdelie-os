#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <shared.h>

typedef struct superblock
{
    u16 block_size;         // Размер одного блока
    u32 first_block;        // Номер первого блока
    u64 total_blocks;       // Размер ФС в блоках
    char name[10];          // Название ФС
    u64 free_blocks;        // Свободный объём хранилища в байтах
    u32 block_bitmap_size;  // Размер битовой карты св./з. блоков
    u32 inode_bitmap_size;  // Размер битовой карты св./з. inode
} superblock_t;

#endif 