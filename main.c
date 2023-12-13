#include <disk_emulator.h>
#include <file_system.h>
#include <inode.h>
#include <kernel.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <superblock.h>
#include <terminal.h>

void correct_exit(int sig) {
  close_disk();
  exit(sig);
}

int main() {
  if (signal(SIGINT, correct_exit) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  printf("\nPlease, choose an option:\n");
  printf("1 - Init file system\n");
  printf("2 - Mount file system\n");
  printf("Exit - Ctrl + C\n");

  int choice = 0;

  scanf("%d", &choice);

  if (choice == 1) {
    // printf("You have chosen an \"Init file system\" option.\n");
    // printf("Sir, enter the disk name in range of 12 characters, please: ");

    // char name[12];
    // scanf("%s", name);

    // printf("Very well.\n");
    // printf("Now, select the size of the disk: ");

    // int size;
    // scanf("%d", &size);
    // printf("Fine... I gotta work to do.\n");

    const char name[] = "lol";
    int size = 8192;
    init_fs(size, name);

  } else {
    printf("Invalid option! P");
    correct_exit(0);
  }
  init_kernel();
  run();

  // create_disk(2049, "mazhan");
  // mount_disk("mazhan");
  // inode_t inode = create_inode(1);
  // superblock_t block;
  // printf("%d\n", inode.id);
  correct_exit(0);
}
