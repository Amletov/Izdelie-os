#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fail(const char *msg) {
  perror(msg);
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
