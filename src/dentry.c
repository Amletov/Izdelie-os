#include <dentry.h>
#include <stdlib.h>
#include <string.h>

dentry_t create_dentry(int inode_id, int parent_inode_id, char * dentry_name)
{
    dentry_t dentry;
    dentry.inode_id = inode_id;
    dentry.parent_inode_id = parent_inode_id;
    strcpy(dentry.name, dentry_name);
    
    return dentry;
}
