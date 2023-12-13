#include "tools.h"
#include <inode.h>
#include <stdio.h>


int set_inode_mode(inode_t *inode, int owner_permissions,
                   int others_permissions) {
  u8 flags = inode->mode >> 6;
  inode->mode &= ~0b11111111;

  inode->mode |= (owner_permissions & 0b111) |
                 ((others_permissions & 0b111) << 3) | (flags << 6);

  return 0;
}

void set_inode_dir_flag(inode_t *inode) { inode->mode |= I_DIR; }

void set_inode_system_flag(inode_t *inode) { inode->mode |= I_SYS; }

// FIXME: Если менять эти строчки местами, то в первом случае, 
// дата создания будет меняться на дату изменения, а во втором — 
// ни дата изменения, ни дата создания не будут меняться вообще
void display_inode(inode_t *inode) {
  char *creation_date = u64date_to_str(inode->ctime);
  char *modification_date = u64date_to_str(inode->mtime);

  printf("\t\033[2mINODE#%d: s %d fb %d m %d ", inode->id, inode->size,
         inode->adresses[0], inode->mode);
  printf("cd %s md %s\n\033[0m", creation_date, modification_date);
}