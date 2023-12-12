#include "kernel.h"
#include "dentry.h"
#include "inode.h"
#include "shared.h"
#include "superblock.h"
#include <disk_emulator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KRNL_ERROR -1
#define KRNL_ERROR_OUT_OF_BLOCKS -2
#define KRNL_ERROR_OUT_OF_INODE -3
#define ARG_ERROR -4

dentry_t current_dentry;
dentry_t *current_dir_items;
superblock_t superblock;
int current_dir_items_count;

char *get_path() { return current_dentry.name; }

int relative_block(inode_t *inode, int offset) {
  return inode->addreses[offset / BLOCK_SIZE];
}

int save_inode(inode_t *inode) {
  int offset = superblock.first_inode_block + inode->id * sizeof(inode_t);
  write_n(offset, sizeof(inode_t), inode);

  return 0;
}

int find_free_block() {
  for (int i = 0; i < superblock.total_blocks; i += 8) {
    u8 bitmap;
    read_n(superblock.first_bitmap_block * BLOCK_SIZE + (i / 8), sizeof(u8),
           &bitmap);
    for (int j = 0; j < 8; j++) {
      if ((bitmap & 1 << j) == 0) {
        return i + j;
      }
    }
  }
  return KRNL_ERROR_OUT_OF_BLOCKS;
}

int find_free_inode() {
  for (int i = 0; i < superblock.total_inodes; i += 8) {
    u8 bitmap;
    read_n(superblock.first_bitmap_inode * BLOCK_SIZE + i / 8, sizeof(u8),
           &bitmap);
    for (int j = 0; j < 8; j++) {
      if ((bitmap & 1 << j) == 0) {
        return i + j;
      }
    }
  }
  return KRNL_ERROR_OUT_OF_INODE;
}

int allocate_block() {
  int block_id = find_free_block();
  if (block_id == -1) {
    return KRNL_ERROR_OUT_OF_BLOCKS;
  }

  u8 bitmap;
  int offset = superblock.first_bitmap_block * BLOCK_SIZE + (block_id / 8);
  read_n(offset, sizeof(u8), &bitmap);
  bitmap |= 1 << (block_id % 8);
  write_n(offset, sizeof(u8), &bitmap);

  return block_id;
}

inode_t *allocate_inode() {
  int inode_id, block_id, offset;

  inode_id = find_free_inode();
  if (inode_id == KRNL_ERROR_OUT_OF_INODE) {
    return NULL;
  }

  block_id = allocate_block();
  if (block_id == KRNL_ERROR_OUT_OF_BLOCKS) {
    return NULL;
  }

  u8 bitmap;
  offset = superblock.first_bitmap_inode * BLOCK_SIZE + inode_id / 8;
  read_n(offset, sizeof(u8), &bitmap);
  bitmap |= 1 << (inode_id % 8);
  write_n(offset, sizeof(u8), &bitmap);

  inode_t *inode = malloc(sizeof(inode_t));
  inode->id = inode_id;
  inode->addreses[0] = block_id;

  return inode;
}

int read_bytes(inode_t *inode, int offset, void *buffer) {
  int bytes_to_read, block_id;
  block_id = relative_block(inode, offset);
  printf("\tREADING BYTES from %d: ", block_id);
  if ((inode->size - offset) > BLOCK_SIZE) {
    bytes_to_read = BLOCK_SIZE;
  } else {
    bytes_to_read = inode->size - offset;
  }
  printf("bytes to read - %db\n", bytes_to_read);
  read_n(block_id * BLOCK_SIZE, bytes_to_read, buffer);

  if (bytes_to_read + bytes_to_read == BLOCK_SIZE) {
    return read_bytes(inode, offset + bytes_to_read, buffer + bytes_to_read);
  }
  return 0;
}

int read_bytes_start(inode_t *inode, void *buffer) {
  int block_id, bytes_to_read;
  block_id = relative_block(inode, 0);
  printf("\tREADING BYTES from %d: ", block_id);
  if (inode->size > BLOCK_SIZE) {
    bytes_to_read = BLOCK_SIZE;
  } else {
    bytes_to_read = inode->size;
  }

  printf("bytes to read - %db\n", bytes_to_read);
  read_n(block_id * BLOCK_SIZE, bytes_to_read, buffer);

  return bytes_to_read +
         (bytes_to_read > BLOCK_SIZE
              ? read_bytes(inode, bytes_to_read, buffer + bytes_to_read)
              : 0);
}

int write_bytes(inode_t *inode, int offset, int size, void *buffer) {
  if (offset > 9 * BLOCK_SIZE) {
    return KRNL_ERROR;
  }

  char block[BLOCK_SIZE];
  int relative_block_id = relative_block(inode, offset);

  if (relative_block_id == -1) {
    int new_block_id = allocate_block();
    if (new_block_id == KRNL_ERROR_OUT_OF_BLOCKS) {
      return KRNL_ERROR_OUT_OF_BLOCKS;
    }
    inode->addreses[offset/BLOCK_SIZE] = new_block_id;
  }

  read_n(relative_block_id * BLOCK_SIZE, BLOCK_SIZE, block);

  int relative_offset = offset % BLOCK_SIZE;
  int available_space;
  if (size > BLOCK_SIZE - relative_offset) {
    available_space = BLOCK_SIZE - relative_offset;
  } else {
    available_space = size - relative_offset;
  }
  write_n(relative_block_id * BLOCK_SIZE + relative_offset, available_space,
          buffer);

  inode->size += available_space;
  save_inode(inode);

  if (available_space + size > BLOCK_SIZE - relative_offset) {
    write_bytes(inode, offset + available_space, size - available_space,
                buffer + available_space);
  }

  return 0;
}

inode_t *get_inode(int inode_id) {
  int offset = superblock.first_inode_block + inode_id * sizeof(inode_t);
  inode_t *inode = malloc(sizeof(inode_t));
  read_n(offset, sizeof(inode_t), inode);

  return inode;
}

int read_directory(dentry_t *dentry, int *nitems, dentry_t *items) {
  printf("\tGetting inode\n");
  inode_t *inode = get_inode(dentry->inode_id);
  printf("\tINODE:\n");
  printf("\t\tid: %d\n", inode->id);
  printf("\t\tsize: %d\n", inode->size);
  printf("\t\tfirst block: %d\n", inode->addreses[0]);
  char buffer[inode->size];
  printf("\tReading directory content\n");
  int read_b = read_bytes_start(inode, buffer);
  char *p = buffer;
  printf("\tRead bytes: %db\n", read_b);
  memcpy(&dentry->inode_id, p, sizeof(int));
  p += sizeof(int);
  printf("\tInode id copied\n");
  memcpy(&dentry->parent_inode_id, p, sizeof(int));
  p += sizeof(int);
  printf("\tParent inode id copied\n");
  memcpy(dentry->name, p, sizeof(dentry->name));
  p += sizeof(dentry->name);
  printf("\tName copied\n");
  memcpy(nitems, p, sizeof(int));
  p += sizeof(int);
  printf("\tNitems copied - %d\n", *nitems);

  items = realloc(items, sizeof(dentry_t) * (*nitems));

  for (int i = 0; i < *nitems; i++) {
    dentry_t item;
    item.parent_inode_id = inode->id;
    memcpy(&item.inode_id, p, sizeof(int));
    memcpy(item.name, p + 4, sizeof(dentry->name));
    p += 4 + 12;
    items[i] = item;
  }
  free(inode);

  return 0;
}

int save_directory(dentry_t *dentry, int nitems, dentry_t *items) {
  int buffer_size = 4 + 4 + 12 + nitems * (4 + 12);
  char buffer[buffer_size];
  char *p = buffer;
  memcpy(p, &dentry->inode_id, sizeof(int));
  p += sizeof(int);
  memcpy(p, &dentry->parent_inode_id, sizeof(int));
  p += sizeof(int);
  memcpy(p, dentry->name, sizeof(dentry->name));
  p += sizeof(dentry->name);
  memcpy(p, &nitems, sizeof(int));
  p += sizeof(int);

  for (int i = 0; i < nitems; i++) {
    dentry_t item = items[i];
    memcpy(p, &item.inode_id, sizeof(int));
    memcpy(p + 4, item.name, sizeof(dentry->name));
    p += 4 + 12;
  }
  inode_t *inode = get_inode(dentry->inode_id);
  write_bytes(inode, 0, buffer_size, buffer);
  free(inode);

  return 0;
}

int init_kernel() {
  printf("INIT KERNEL:\n");
  printf("\tReading superblock\n");
  read_n(0, sizeof(superblock_t), &superblock);
  printf("\tChecking superblock.magic_number\n");
  if (superblock.magic_number != MAGIC_NUMBER) {
    return KRNL_ERROR;
  }
  printf("\tAllocating system blocks\n");
  for (int i = 0; i < superblock.first_block; i++) {
    allocate_block();
  }

  printf("\tAllocating inode\n");
  inode_t *root = allocate_inode();
  printf("\tAllocated inode:\n\t\tid: %d\n\t\taflags: %d\n", root->id,
         root->mode);
  printf("\tSetting inode directory flag\n");
  set_inode_dir_flag(root);
  printf("\tSetting inode system flag\n");
  set_inode_system_flag(root);
  printf("\tSaving inode\n");
  save_inode(root);
  printf("\tCreating directory\n");
  current_dentry = create_dentry(root->id, -1, "ROOT");
  printf("\tSaving directory\n");
  save_directory(&current_dentry, 0, current_dir_items);
  printf("COMPLETE KERNEL INIT!\n");

  return 0;
}

int mkdir(int argc, char **argv) { return 0; }

int mkfile(int argc, char **argv) {
  if (argc < 2) {
    printf("USAGE: mkfile <path_to_file>\n");
    return ARG_ERROR;
  }

  inode_t *inode = allocate_inode();
  dentry_t *items = malloc(sizeof(dentry_t) * current_dir_items_count);
  memcpy(items, current_dir_items, sizeof(dentry_t) * current_dir_items_count);
  current_dir_items = realloc(current_dir_items,
                              sizeof(dentry_t) * (current_dir_items_count + 1));
  memcpy(current_dir_items, items, sizeof(dentry_t) * current_dir_items_count);
  free(items);
  current_dir_items[current_dir_items_count++] =
      create_dentry(inode->id, current_dentry.inode_id, argv[1]);
  save_directory(&current_dentry, current_dir_items_count, current_dir_items);

  return 0;
}

int sb(int argc, char **argv) {
  printf("SUPERBLOCK INFO: \n");
  printf("\tname: %s\n", superblock.name);
  printf("\tblock size: %d\n", superblock.block_size);
  printf("\tfirst bitmap block of blocks: %d\n", superblock.first_bitmap_block);
  printf("\tfirst bitmap inode of inodes: %d\n", superblock.first_bitmap_inode);
  printf("\tfirst block of data: %d\n", superblock.first_block);
  printf("\tfirst block of inodes: %d\n", superblock.first_inode_block);
  printf("\tfree blocks: %ld\n", superblock.free_blocks);
  printf("\ttotal blocks: %ld\n", superblock.total_blocks);
  printf("\ttotal inodes: %d\n", superblock.total_inodes);

  return 0;
}

int ls(int argc, char **argv) {
  read_directory(&current_dentry, &current_dir_items_count, current_dir_items);
  printf("name\tmode\tsize\n");
  for (int i = 0; i < current_dir_items_count; ++i) {
    printf("%s\n", current_dir_items[i].name);
  }

  return 0;
}