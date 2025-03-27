#ifndef MEMORY_H
#define MEMORY_H

#include <string.h>
#include <stdlib.h>

#define VSTR(s) #s
#define NAME_LEN 100

void mem_alloc(char **ptr, const char *var, int size);

#endif