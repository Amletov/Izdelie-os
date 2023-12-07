#include <stdio.h>
#include <inode.h>
#include <stdlib.h>
#include <signal.h>
#include <superblock.h>
#include <file_system.h>

int correct_exit(int sig)
{
    close_disk();
}

int main(int, char **)
{
    if (signal(SIGINT, &correct_exit) == SIG_ERR)
    {
        perror("signal");
        exit(EXIT_FAILURE);

    }
    
    printf("Hello, from divad!\n");

    init_fs(500000, "RaulRamzeS");
    // create_disk(2049, "mazhan");
    // mount_disk("mazhan");

    // inode_t inode = create_inode(1);
    // superblock_t block;
    // printf("%d\n", inode.id);
    correct_exit(0);
}
