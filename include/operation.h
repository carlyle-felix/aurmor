#ifndef OPERATION_H
#define OPERATION_H

#include <alpm.h>

typedef struct package List;

void target_clone(char *url);
int install(List *pkglist,  alpm_pkgreason_t reason);
void clean(void);
void print_search(char *pkgname);
void print_installed(void);
void update(void);
void force_update(List *pkglist);

#endif