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
    
    printf("Hi, my name is Denis Perviy. I'm a student from DPI(7000). Welcome to this FS is my course work on C lang (Ш ланг)\n");
    printf("Please, choose an option:\n");
    printf("1 - Init file system\n");
    printf("2 - Mount file system\n");
    printf("Exit - Ctrl + C\n");

    int choice = 0;

    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("You have chosen an \"Init file system\" option.\n");
        printf("Sir, enter the disk name in range of 12 characters, please: ");

        char name[12];
        scanf("%s", name);

        printf("Very well.\n");
        printf("Now, select the size of the disk: ");

        int size;
        scanf("%d", &size);
        printf("Fine... I gotta work to do.\n");

        init_fs(size, name);
    }
    // else (!choice)
    // {
    //     printf("Invalid option! P");
    // }
    

    
    // create_disk(2049, "mazhan");
    // mount_disk("mazhan");

    // inode_t inode = create_inode(1);
    // superblock_t block;
    // printf("%d\n", inode.id);
    correct_exit(0);
}
