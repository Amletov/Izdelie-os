#include <inode.h>

int set_inode_mode(inode_t *inode, int owner_permissions, int others_permissions)
{
    u8 flags = 0b11000000 & inode->mode;
    inode->mode &= ~0b11111111;

    inode->mode |= (owner_permissions & 0b111) | ((others_permissions & 0b111) << 3) | flags;
}

void set_inode_dir_flag(inode_t *inode)
{
    inode->mode |= I_DIR;
}

void set_inode_system_flag(inode_t *inode)
{
    inode->mode |= I_SYS;
}