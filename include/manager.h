#ifndef MANAGER_H
#define MANAGER_H

#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

typedef struct pkg Pkginfo;
typedef struct depends Depends;
typedef struct package List;

int alpm_uninstall(List *pkglist);
int alpm_install(List *list);
alpm_list_t *handle_init(alpm_handle_t **handle);

#endif 