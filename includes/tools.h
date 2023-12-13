#ifndef TOOLS_H
#define TOOLS_H

#include "shared.h"

void fail(const char *msg);
int align(int raw_size);
char *u64date_to_str(u64 u64_date);

#endif