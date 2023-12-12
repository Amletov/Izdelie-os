#ifndef INODE_H
#define INODE_H

#include "shared.h"

#define I_DIR 128
#define I_SYS 64

typedef struct inode
{
    u32 id;
    u16 mode;           // Права доступа
    u16 uid;            // ID владельца
    u32 size;           // Размер файла
    u64 mtime;          // Дата и время создания файла
    u64 ctime;          // Дата и время модификации файла
    u32 file_blocks;    // Число блоков, занимаемых, занимаемых файлом
    u32 addreses[9];    // Массив адресов 
} inode_t;

int set_inode_mode(inode_t *inode, int owner_permissions, int others_permissions);
void set_inode_dir_flag(inode_t *inode);
void set_inode_system_flag(inode_t *inode);

#endif