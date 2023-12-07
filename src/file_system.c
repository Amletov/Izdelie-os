#include <string.h>
#include <stdio.h>
#include <math.h>
#include <file_system.h>
#include <superblock.h>
#include <disk_emulator.h>

int init_fs(int size, const char *file_name)
{
    int total_size = create_disk(size, file_name);

    int allocated_blocks = 0;
    superblock_t superblock;
    allocated_blocks += 1;

    superblock.total_blocks = total_size / BLOCK_SIZE;
    superblock.total_inodes = total_size / 100;

    superblock.first_bitmap_block = allocated_blocks;
    allocated_blocks += align(sizeof(u8) * ceil((double)superblock.total_blocks / 8)) / BLOCK_SIZE;

    superblock.first_bitmap_inode = allocated_blocks;
    allocated_blocks += align(sizeof(u8) * ceil((double)superblock.total_inodes / 8)) / BLOCK_SIZE;

    superblock.first_inode_block = allocated_blocks;
    allocated_blocks += align(sizeof(inode_t) * ceil((double)superblock.total_inodes / sizeof(inode_t))) / BLOCK_SIZE;

    superblock.first_block = allocated_blocks;
    superblock.block_size = BLOCK_SIZE;

    printf("allocated_blocks: %d\n", allocated_blocks);
    strcpy(superblock.name, "PoleChudes");

    write_n(0, sizeof(superblock_t), (void *)&superblock);

    u8 O = 0;

    printf("Writing BitMap...\n");
    for (int i = 0; i < superblock.total_blocks; i += 8)
    {
        write_n((superblock.first_bitmap_block * BLOCK_SIZE) + i / 8, 1, (void *)&O);
    }

    printf("Writing Inode BitMap...\n");
    for (int i = 0; i < superblock.total_inodes; i += 8)
    {
        write_n((superblock.first_bitmap_inode * BLOCK_SIZE) + i / 8, 1, (void *)&O);
    }

    printf("Writing Inodes...\n");
    for (int i = 0; i < superblock.total_inodes; i++)
    {
        inode_t inode;
        inode.id = i;
        write_n((superblock.first_inode_block * BLOCK_SIZE) + i * sizeof(inode_t), sizeof(inode_t), (void *)&inode);
    }
}

int load_fs()
{
}