#ifndef FILESYSTEM_H
#define FILESYSTEM_H

int create_disk(int size, const char *file_name);
int mount_disk(const char *file_name);
int write_n(int offset, int size, char * buffer);
int read_n(int offset, int size, char * buffer);
int close_disk();

#endif
