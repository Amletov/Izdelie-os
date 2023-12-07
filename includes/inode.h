#ifndef INODE_H
#define INODE_H

#include <shared.h>

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

// inode_t create_inode(int id);
#endif 

