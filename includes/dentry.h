#ifndef DENTRY_H
#define DENTRY_H

typedef struct dentry
{
    int inode_id;
    int parent_inode_id;
    char name[12];
}dentry_t;

dentry_t create_dentry(int inode_id, int parent_inode_id, char * dentry_name);

#endif