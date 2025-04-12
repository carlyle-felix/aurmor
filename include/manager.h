#ifndef MANAGER_H
#define MANAGER_H

#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

typedef struct package List;

typedef struct data {
    char *data;
    struct data *next;
} Data;

List *foreign_list(void);
void alpm_uninstall(List *pkglist);
void alpm_install(List *list);

#endif 