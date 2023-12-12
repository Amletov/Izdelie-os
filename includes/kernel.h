#ifndef KERNEL_H
#define KERNEL_H

int init_kernel();
int mkdir(int argc, char **argv);
int mkfile(int argc, char **argv);
int ls(int argc, char **argv);
int cd(int argc, char **argv);
int mv(int argc, char **argv);
int rm(int argc, char **argv);
int cp(int argc, char **argv);
int sb(int argc, char **argv);
char *get_path();

#endif