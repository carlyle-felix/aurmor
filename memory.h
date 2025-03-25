#ifndef MEMORY_H
#define MEMORY_H

#define VSTR(s) #s

char *mem_malloc(char *var, int size);
char *mem_realloc(char *p, char *var, int size);

#endif
