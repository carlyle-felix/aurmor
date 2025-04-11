#ifndef MANAGER_H
#define MANAGER_H

#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

typedef struct package List;

typedef struct info {
    char *data;
    struct info *next;
} Srcinfo;

List *foreign_list(void);
void alpm_uninstall(List *pkglist);
void alpm_install(List *list);

#endif 