#include "terminal.h"
#include "kernel.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 1024

static int set_args(char *args, char **argv) {
  int count = 0;

  while (isspace(*args))
    ++args;
  while (*args) {
    if (argv)
      argv[count] = args;
    while (*args && !isspace(*args))
      ++args;
    if (argv && *args)
      *args++ = '\0';
    while (isspace(*args))
      ++args;
    count++;
  }
  return count;
}

char **parsed_args(char *args, int *argc) {
  char **argv = NULL;
  int argn = 0;

  if (args && *args && (args = strdup(args)) && (argn = set_args(args, NULL)) &&
      (argv = malloc((argn + 1) * sizeof(char *)))) {
    *argv++ = args;
    argn = set_args(args, argv);
  }

  if (args && !argv)
    free(args);

  *argc = argn;
  return argv;
}

void free_parsed_args(char **argv) {
  if (argv) {
    free(argv[-1]);
    free(argv - 1);
  }
}

int run() {
  int is_running = 1;
  char prompt[MEM_SIZE];
  char **argv;
  int argc;

  while (is_running) {
    printf("\n%s: ", get_path());
    fgets(prompt, MEM_SIZE, stdin);
    argv = parsed_args(prompt, &argc);
    if (argc < 1) {
      continue;
    }
    if (strcmp(argv[0], "mkfile") == 0) {
      mkfile(argc, argv);
    } else if (strcmp(argv[0], "sb") == 0) {
      sb(argc, argv);
    } else if (strcmp(argv[0], "ls") == 0) {
      ls(argc, argv);
    } else if (strcmp(argv[0], "mkdir") == 0) {
      mkdir(argc, argv);
    } else {
      printf("\033[31mERROR: unknown command \"%s\"\033[0m", argv[0]);
    }

    free_parsed_args(argv);
  }

  return 0;
}
