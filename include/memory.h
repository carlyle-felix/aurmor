#ifndef MEMORY_H
#define MEMORY_H

#include <string.h>
#include <stdlib.h>

#define VSTR(s) #s
#define NAME_LEN 100

char *mem_malloc(char *var, int size);
char *mem_realloc(char *p, char *var, int size);

#endif
