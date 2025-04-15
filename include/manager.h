#ifndef MANAGER_H
#define MANAGER_H

#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

typedef struct package List;

typedef struct depends {
    char *data;
    struct depends *next;
} Depends;

typedef struct srcinfo {
    char *pkgname;
    char *epoch;
    char *pkgver;
    char *pkgrel;
    char *zst_path;
    Depends *makedepends;
    Depends *depends;
    Depends *optdepends;
} Srcinfo;

int alpm_uninstall(List *pkglist);
int alpm_install(List *list);
alpm_list_t *handle_init(alpm_handle_t **handle);

#endif 