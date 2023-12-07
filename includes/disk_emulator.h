#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <superblock.h>
#include <inode.h>

// ОШИБКИ В ЛР9

// typedef struct file_system
// {
//     superblock_t superblock;    // Суперблок
//     bitmap_t block_bitmap;      // Битовая карта блоков
//     bitmap_t inode_bitmap;      // Битовая карта inode
//     inode_t *inode;
//     // Инф. пользователей
//     // Корневой каталог
// }file_system_t;
extern int align(int raw_size);
extern int create_disk(int size, const char *file_name);
extern int mount_disk(const char *file_name);
extern int read_blocks(int start_block, int nblocks, void *buffer);
extern int write_blocks(int start_block, int nblocks, void *buffer);
extern int write_n(int offset, int size, char * buffer);
extern int close_disk();

#endif
