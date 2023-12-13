#include "disk_emulator.h"
#include "shared.h"
#include "tools.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define DISK_ERROR -1

int file_descriptor;
int total_file_size;
void *memory_mapped_file_pointer;

int create_disk(int size, const char *file_name) {
  strcat(file_name, ".ufc");
  if ((file_descriptor = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0666)) ==
      -1) {
    fail("shm_open");
  }

  total_file_size = ((size + (BLOCK_SIZE - 1)) / BLOCK_SIZE) * BLOCK_SIZE;

  if (ftruncate(file_descriptor, total_file_size) == -1) {
    fail("ftruncate");
  }

  fsync(file_descriptor);

  printf("\033[32mDisk created\033[0m\n\n");
  return total_file_size;
}

int mount_disk(const char *file_name) {
  if ((file_descriptor = open(file_name, O_RDWR, 0666)) == -1) {
    fail("shm_open");
  }

  struct stat st;

  stat(file_name, &st);
  total_file_size = st.st_size;

  printf("\033[32mDisk mounted\033[0m: n \"%s\" s - %db\n\n", file_name,
         total_file_size);

  return 0;
}

int write_n(int offset, int size, char *buffer) {
  // printf("\t\tWRITING offset: %d, size: %d\n", offset, size);

  lseek(file_descriptor, offset, SEEK_SET);
  write(file_descriptor, buffer, size);
  fsync(file_descriptor);

  return 0;
}

int read_n(int offset, int size, char *buffer) {
  // printf("\t\tREADING offset: %d, size: %d\n", offset, size);

  lseek(file_descriptor, offset, SEEK_SET);
  read(file_descriptor, buffer, size);

  return 0;
}

int close_disk() {
  printf("\n\033[33mClosing disk\033[0m\n");
  close(file_descriptor);

  return 0;
}
