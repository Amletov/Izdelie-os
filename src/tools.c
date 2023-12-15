#include "../includes/tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void fail(const char *msg) {
  printf("\n\033[31m");
  perror(msg);
  printf("\033[0m");
  exit(EXIT_FAILURE);
}

int align(int raw_size) {
  return ((raw_size + (BLOCK_SIZE - 1)) / BLOCK_SIZE) * BLOCK_SIZE;
}

char *u64date_to_str(u64 u64_date) {
  static char str_date[20];
  time_t ctime_t = (time_t)u64_date;
  struct tm *date = localtime(&ctime_t);
  strftime(str_date, sizeof(str_date), "%d.%m.%y %H:%M", date);

  return str_date;
}

char *concat(const char *s1, const char *s2) {
  char *result =
      malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
  // in real code you would check for errors in malloc here
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

char *uinput() {
  char *string;
  // number of characters in the buffer
  size_t counter = 0;

  // size of allocated buffer
  size_t allocated = 16;

  int c;
  string = malloc(allocated); // sizeof(char) is 1
  do {
    c = getchar();
    if (c == EOF) {
      break;
    }
    // if our buffer is too small, double the allocation
    if (counter + 2 <= allocated) {
      size_t new_size = allocated * 2;
      char *new_buffer = realloc(string, new_size);
      if (!new_buffer) {
        // out of memory? try smaller increment
        new_size = allocated + 16;
        new_buffer = realloc(string, new_size);
        if (!new_buffer) {
          // really out of memory: free old block
          free(string);
          return NULL;
        }
      }
      allocated = new_size;
      string = new_buffer;
    }
    // store the character
    string[counter++] = c;
  } while (c != '\n');

  // terminate the buffer properly
  string[counter - 1] = '\0';
  return string;
}

char *read_string() {
  char *big = NULL, *old_big;
  char s[11] = {0};
  int len = 0, old_len;

  do {
    old_len = len;
    old_big = big;
    scanf("%10[^\n]", s);
    if (!(big = realloc(big, (len += strlen(s)) + 1))) {
      free(old_big);
      fprintf(stderr, "Out of memory!\n");
      return NULL;
    }
    strcpy(big + old_len, s);
  } while (len - old_len == 10);
  return big;
}
