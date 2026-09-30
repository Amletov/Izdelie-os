#include "../includes/terminal.h"
#include "../includes/kernel.h"
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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
    printf("\n");
    get_path();
    printf(" > ");
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
    } else if (strcmp(argv[0], "cd") == 0) {
      cd(argc, argv);
    } else if (strcmp(argv[0], "r") == 0) {
      r(argc, argv);
    } else if (strcmp(argv[0], "w") == 0) {
      w(argc, argv);
    } else if (strcmp(argv[0], "app") == 0) {
      app(argc, argv);
    } else if (strcmp(argv[0], "exit") == 0) {
      is_running = 0;
    } else if (strcmp(argv[0], "clear") == 0) {
      system("clear");
    } else {
      printf("\033[31mERROR: unknown command \"%s\"\033[0m", argv[0]);
    }

    free_parsed_args(argv);
  }

  return 0;
}
