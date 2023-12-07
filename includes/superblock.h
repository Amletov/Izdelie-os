#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <shared.h>
// u16 block_size;         // Размер одного блока
//     u32 first_block;        // Номер первого блока
//     u64 total_blocks;       // Размер ФС в блоках
//     char name[10];          // Название ФС
//     u64 free_blocks;        // Свободный объём хранилища в байтах
//     u32 block_bitmap_size;  // Размер битовой карты св./з. блоков
//     u32 inode_bitmap_size;  // Размер битовой карты св./з. inode
typedef struct superblock
{
    u16 block_size;         // Размер одного блока
    u64 total_blocks;       // Размер ФС в блоках
    u32 total_inodes;
    char name[12];          // Название ФС
    u32 first_block;        // Номер первого блока
    u64 free_blocks;        // Свободный объём хранилища в байтах
    u32 first_bitmap_block;  // Размер битовой карты св./з. блоков
    u32 first_bitmap_inode;  // Размер битовой карты св./з. inode
    u32 first_inode_block;
} superblock_t;

#endif 