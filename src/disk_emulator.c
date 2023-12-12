#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "disk_emulator.h"
#include "shared.h"


#define DISK_ERROR -1

int file_descriptor;
int total_file_size;
void *memory_mapped_file_pointer;

void fail(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int align(int raw_size)
{
    return ((raw_size + (BLOCK_SIZE - 1)) / BLOCK_SIZE) * BLOCK_SIZE;
}

int create_disk(int size, const char *file_name)
{
    if ((file_descriptor = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0666)) == -1)
    {
        fail("shm_open");
    }

    total_file_size = ((size + (BLOCK_SIZE - 1)) / BLOCK_SIZE) * BLOCK_SIZE;

    if (ftruncate(file_descriptor, total_file_size) == -1)
    {
        fail("ftruncate");
    }

    fsync(file_descriptor);

    printf("Disk created\n");
    return total_file_size;
}

int mount_disk(const char *file_name)
{
    if ((file_descriptor = open(file_name, O_RDWR, 0666)) == -1)
    {
        fail("shm_open");
    }

    struct stat st;

    stat(file_name, &st);
    total_file_size = st.st_size;

    printf("Disk mounted. Name - \"%s\", size - %db.\n", file_name, total_file_size);

    return 0;
}

int write_blocks(int start_block, int nblocks, void *buffer)
{
    memcpy(memory_mapped_file_pointer + (start_block * nblocks), buffer, nblocks * BLOCK_SIZE);
    return 0;
}

int read_blocks(int start_block, int nblocks, void *buffer)
{
    if ((memory_mapped_file_pointer = mmap(0, total_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0)) == MAP_FAILED)
    {
        fail("mmap");
    }

    memcpy(buffer, memory_mapped_file_pointer + (start_block * nblocks), nblocks * BLOCK_SIZE);

    return 0;
}

int write_n(int offset, int size, char *buffer)
{
    printf("\t\tWRITING offset: %d, size: %d\n", offset, size);

    lseek(file_descriptor, offset, SEEK_SET);
    write(file_descriptor, buffer, size);
    fsync(file_descriptor);

    return 0;
}

int read_n(int offset, int size, char *buffer)
{   
    printf("\t\tREADING offset: %d, size: %d\n", offset, size);

    lseek(file_descriptor, offset, SEEK_SET);
    read(file_descriptor, buffer, size);

    return 0;
}

int close_disk()
{
    printf("\nClosing disk\n");
    close(file_descriptor);
}
