#include <stdio.h>
#include <inode.h>

int main(int, char**){
    printf("Hello, from divad!\n");

    inode_t inode = create_inode(1);
    printf("%d\n", inode.id);
}
