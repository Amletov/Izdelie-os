#include "../includes/kernel.h"
#include "../includes/dentry.h"
#include "../includes/disk_emulator.h"
#include "../includes/inode.h"
#include "../includes/list.h"
#include "../includes/shared.h"
#include "../includes/superblock.h"
#include "../includes/tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KRNL_ERROR -1
#define KRNL_ERROR_OUT_OF_BLOCKS -2
#define KRNL_ERROR_OUT_OF_INODE -3
#define ARG_ERROR -4

dentry_t current_dentry;
dentry_t *current_dir_items;
superblock_t superblock;
int current_dir_items_count;
node_t *head = NULL;

void get_path() { print_list(head, '/'); }

int relative_block(inode_t *inode, int offset) {
  return inode->adresses[offset / BLOCK_SIZE];
}

int save_inode(inode_t *inode) {
  // printf("save_inode\n");
  int offset =
      superblock.first_inode_block * BLOCK_SIZE + inode->id * sizeof(inode_t);
  inode->mtime = (u64)time(NULL);
  write_n(offset, sizeof(inode_t), (void *)inode);
  display_inode(inode);

  return 0;
}

int find_free_block() {
  for (int i = 0; i < superblock.total_blocks; i += 8) {
    u8 bitmap;
    read_n(superblock.first_bitmap_block * BLOCK_SIZE + (i / 8), sizeof(u8),
           (char *)&bitmap);
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
  // printf("allocate_block\n");
  int block_id = find_free_block();
  if (block_id == KRNL_ERROR_OUT_OF_BLOCKS) {
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
  // printf("allocate_inode\n");
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
  inode->adresses[0] = block_id;
  inode->mode = 0;
  inode->ctime = (u64)time(NULL);
  inode->mtime = (u64)time(NULL);

  save_inode(inode);
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

  if (offset == 0) {
    // printf("offset = 0 (write_bytes)\n");
    inode->size = 0;
  }

  // for (int i = 0; i < size; ++i) {
  //   printf("%d ", (int)*(char *)(buffer + i));
  // }

  char block[BLOCK_SIZE];
  int relative_block_id = relative_block(inode, offset);

  if (relative_block_id == -1) {
    int new_block_id = allocate_block();
    if (new_block_id == KRNL_ERROR_OUT_OF_BLOCKS) {
      return KRNL_ERROR_OUT_OF_BLOCKS;
    }
    inode->adresses[offset / BLOCK_SIZE] = new_block_id;
  }

  read_n(relative_block_id * BLOCK_SIZE, BLOCK_SIZE, block);

  int relative_offset = offset % BLOCK_SIZE;
  int available_space;
  if (size > BLOCK_SIZE - relative_offset) {
    // printf("size(%d) > 512 - rel_of(%d)\n", size, relative_offset);
    available_space = BLOCK_SIZE - relative_offset;
  } else {
    available_space = size - relative_offset;
  }
  write_n(relative_block_id * BLOCK_SIZE + relative_offset, available_space,
          buffer);

  printf("size(%d) += avail_sp(%d)\n", inode->size, available_space);
  inode->size += available_space;
  inode->mtime = (u64)time(NULL);
  save_inode(inode);

  printf("'%s' written to %d inode\n", buffer, inode->id);
  if (available_space + size > BLOCK_SIZE - relative_offset) {
    write_bytes(inode, offset + available_space, size - available_space,
                buffer + available_space);
  }

  return 0;
}

inode_t *get_inode(int inode_id) {
  // printf("get_inode\n");
  int offset =
      superblock.first_inode_block * BLOCK_SIZE + inode_id * sizeof(inode_t);
  inode_t *inode = malloc(sizeof(inode_t));
  read_n(offset, sizeof(inode_t), (char *)inode);
  display_inode(inode);

  return inode;
}

int read_directory(dentry_t *dentry, dentry_t *items) {
  inode_t *inode = get_inode(dentry->inode_id);
  int nitems = 0;
  char buffer[inode->size];
  printf("\tReading directory content\n");
  int read_b = read_bytes_start(inode, buffer);
  char *p = buffer;
  printf("\tRead bytes: %db\n", read_b);
  memcpy(&dentry->inode_id, p, sizeof(int));
  p += sizeof(int);
  printf("\tInode id copied %d\n", dentry->inode_id);
  memcpy(&dentry->parent_inode_id, p, sizeof(int));
  p += sizeof(int);
  printf("\tParent inode id copied %d\n", dentry->parent_inode_id);
  memcpy(dentry->name, p, sizeof(dentry->name));
  p += sizeof(dentry->name);
  printf("\tName copied %s\n", dentry->name);
  memcpy(&nitems, p, sizeof(int));
  p += sizeof(int);
  printf("\tNitems copied - %d\n", nitems);

  if (nitems == 0) {
    free(inode);
    return 0;
  }

  items = realloc(items, sizeof(dentry_t) * (nitems));

  for (int i = 0; i < nitems; i++) {
    dentry_t item;
    item.parent_inode_id = inode->id;
    memcpy(&item.inode_id, p, sizeof(int));
    memcpy(item.name, p + 4, sizeof(dentry->name));
    p += 4 + 12;
    items[i] = item;
  }
  free(inode);

  return nitems;
}

int save_directory(dentry_t *dentry, int nitems, dentry_t *items) {
  int buffer_size = 4 + 4 + 12 + 4 + nitems * (4 + 12);
  printf("buffer_size = %d\n", buffer_size);
  char buffer[buffer_size];
  char *p = buffer;
  memcpy(p, &dentry->inode_id, sizeof(int));
  p += sizeof(int);
  memcpy(p, &dentry->parent_inode_id, sizeof(int));
  p += sizeof(int);
  memcpy(p, dentry->name, 12);
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
  printf("\tSetting inode sys + dir flag\n");
  set_inode_dir_flag(root);
  set_inode_system_flag(root);
  printf("\tSaving inode\n");
  save_inode(root);
  printf("\tCreating directory\n");
  current_dentry = create_dentry(root->id, -1, "ROOT");
  printf("\tSaving directory\n");
  save_directory(&current_dentry, 0, current_dir_items);
  printf("\033[32m\nComplete kernel initialization!\033[0m\n");

  head = create_node(current_dentry.name);

  free(root);
  return 0;
}

int mkdir(int argc, char **argv) {
  // printf("mkdir\n");
  int inode_id = mkfile(argc, argv);
  if (inode_id == ARG_ERROR) {
    return ARG_ERROR;
  }

  inode_t *inode = get_inode(inode_id);
  set_inode_dir_flag(inode);
  save_inode(inode);
  display_inode(inode);

  dentry_t d = create_dentry(inode->id, current_dentry.inode_id, argv[1]);

  save_directory(&d, 0, NULL);

  free(inode);

  return 0;
}

int mkfile(int argc, char **argv) {
  // printf("mkfile\n");
  if (argc < 2) {
    printf("\033[31mUSAGE: mkfile/mkdir <path_to_file>\033[0m\n");
    return ARG_ERROR;
  }

  inode_t *inode = allocate_inode();
  dentry_t *items = malloc(sizeof(dentry_t) * current_dir_items_count);
  int inode_id = inode->id;

  memcpy(items, current_dir_items, sizeof(dentry_t) * current_dir_items_count);
  current_dir_items = realloc(current_dir_items,
                              sizeof(dentry_t) * (current_dir_items_count + 1));
  memcpy(current_dir_items, items, sizeof(dentry_t) * current_dir_items_count);
  free(items);
  current_dir_items[current_dir_items_count++] =
      create_dentry(inode->id, current_dentry.inode_id, argv[1]);
  save_directory(&current_dentry, current_dir_items_count, current_dir_items);

  display_inode(inode);
  free(inode);

  return inode_id;
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
  current_dir_items_count = read_directory(&current_dentry, current_dir_items);
  printf("inode_id\tname\tsize\tmode\tcreation_date\tmodification_date\n");

  inode_t *inode = get_inode(current_dentry.inode_id);
  printf("%d\t\t.\t%db\t%d\t%s\t%s\n", inode->id, inode->size, inode->mode,
         u64date_to_str(inode->ctime), u64date_to_str(inode->mtime));
  if (current_dentry.inode_id != 0) {
    inode_t *i = get_inode(current_dentry.parent_inode_id);
    printf("%d\t\t..\t%db\t%d\t%s\t%s\n", i->id, i->size, i->mode,
           u64date_to_str(i->ctime), u64date_to_str(i->mtime));
  }
  free(inode);

  for (int i = 0; i < current_dir_items_count; ++i) {
    inode_t *inode = get_inode(current_dir_items[i].inode_id);
    char *creation_date = u64date_to_str(inode->ctime);
    char *modification_date = u64date_to_str(inode->mtime);

    printf("%d\t", inode->id);
    printf("\t%s", current_dir_items[i].name);
    printf("\t%db", inode->size);
    printf("\t%d", inode->mode);
    printf("\t%s", creation_date);
    printf("\t%s", modification_date);
    printf("\n");
    free(inode);
  }

  return 0;
}

int find_by_name(char *name) {
  for (int i = 0; i < current_dir_items_count; ++i) {
    if (strcmp(current_dir_items[i].name, name) == 0) {
      // printf("Found %s in %s\n", name, current_dentry.name);
      return current_dir_items[i].inode_id;
    }
  }

  return -1;
}

int set_current_dir(char *name) {
  const char *op = "set_current_dir";

  if (strcmp(name, ".") == 0) {
    return 0;
  } else if (strcmp(name, "..") == 0) {
    if (current_dentry.inode_id == 0) {
      return 0;
    }
    dentry_t temp_dentry =
        create_dentry(current_dentry.parent_inode_id, -1, name);
    // printf("created %d %d %s at (%s)\n", temp_dentry.inode_id,
    //        temp_dentry.parent_inode_id, temp_dentry.name, op);

    current_dir_items_count = read_directory(&temp_dentry, current_dir_items);
    // printf("got a %d dentry in it (%s)\n", current_dir_items_count, op);
    memcpy(&current_dentry, &temp_dentry, sizeof(dentry_t));
    printf("dentry copied %d %d %s (%s)\n", current_dentry.inode_id,
           current_dentry.parent_inode_id, current_dentry.name, op);

    remove_last(head);
    return 0;
  }

  // printf("Input name: %s (%s)", name, op);

  int inode_id = find_by_name(name);
  if (inode_id == -1) {
    printf("\033[31mERROR: dentry '%s' does not exists\033[0m\n", name);
    return -1;
  }

  inode_t *inode = get_inode(inode_id);
  if ((inode->mode & I_DIR) == 0) {
    printf("\033[31m '%s' is not a directory\n", name);
    return -1;
  }

  dentry_t temp_dentry = create_dentry(inode_id, current_dentry.inode_id, name);
  // printf("created %d %d %s at (%s)\n", temp_dentry.inode_id,
  //        temp_dentry.parent_inode_id, temp_dentry.name, op);

  current_dir_items_count = read_directory(&temp_dentry, current_dir_items);
  // printf("got a %d dentries in it (%s)\n", current_dir_items_count, op);
  memcpy(&current_dentry, &temp_dentry, sizeof(dentry_t));
  // printf("dentry copied %d %d %s (%s)\n", current_dentry.inode_id,
  //        current_dentry.parent_inode_id, current_dentry.name, op);

  push(head, name);

  return 0;
}

int cd(int argc, char **argv) {
  // printf("cd\n");
  if (argc < 2) {
    printf("\033[31mUSAGE: cd <path>\033[0m\n");
    return ARG_ERROR;
  }

  char *token;
  char *copy = strdup(argv[1]);

  token = strtok(copy, "/");

  while (token != NULL) {
    if (set_current_dir(token) == -1) {
      printf("\033[31mERROR: directory \"%s\" does not exist\033[0m\n", token);
      return -1;
    }
    token = strtok(NULL, "/");
  }

  free(copy);

  return 0;
}

int r(int argc, char **argv) {
  if (argc < 2) {
    printf("\033[31mUSAGE: r <file>\033[0m\n");
    return ARG_ERROR;
  }

  int inode_id = find_by_name(argv[1]);
  if (inode_id == -1) {
    printf("\033[31mERROR: current dir does not contains %s\033[0m\n", argv[1]);
    return -1;
  }

  inode_t *inode = get_inode(inode_id);
  if ((inode->mode & I_DIR) != 0) {
    printf("\033[0mERROR: cannot read a dir\033[0m\n");
    return -1;
  }

  char *buffer = malloc(inode->size);
  read_bytes(inode, 0, buffer);

  printf("size: %d\n", inode->size);
  printf("%s", buffer);
  free(buffer);
  return 0;
}

int w(int argc, char **argv) {
  if (argc < 2) {
    printf("\033[31mUSAGE: p <file>\033[0m\n");
    return ARG_ERROR;
  }

  int inode_id = find_by_name(argv[1]);
  if (inode_id == -1) {
    inode_id = mkfile(argc, argv);
  }

  inode_t *inode = get_inode(inode_id);
  if ((inode->mode & I_DIR) != 0) {
    printf("\033[31mERROR: cannot write to a dir\033[0m\n");
    return -1;
  }

  char *input = NULL, *result = "";

  do {
    input = uinput();
    if (strcmp(input, ":q") == 0) {
      break;
    }
    result = concat(result, input);
    result = concat(result, "\n");

  } while (strcmp(input, ":q") != 0);

  inode->mtime = (u64)time(NULL);
  write_bytes(inode, 0, strlen(result) + 1, result);

  return 0;
}

int app(int argc, char **argv) {
  if (argc < 2) {
    printf("\033[31mUSAGE: app <file>\n\033[0m");
    return ARG_ERROR;
  }

  int inode_id = find_by_name(argv[1]);
  if (inode_id == -1) {
    inode_id = mkfile(argc, argv);
  }

  inode_t *inode = get_inode(inode_id);
  if ((inode->mode & I_DIR) != 0) {
    printf("\033[31mERROR: cannot write to a dir\n\033[0m");
    return -1;
  }

  char *input = NULL, *result = "";

  do {
    input = uinput();
    if (strcmp(input, ":q") == 0) {
      break;
    }
    result = concat(result, input);
    result = concat(result, "\n");

  } while (strcmp(input, ":q") != 0);

  inode->mtime = (u64)time(NULL);
  write_bytes(inode, inode->size - 2, inode->size + strlen(input) + 1, result);

  return 0;
}
