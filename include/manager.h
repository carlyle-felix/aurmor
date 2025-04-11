#ifndef MANAGER_H
#define MANAGER_H
#include <alpm_list.h>
typedef struct package List;

typedef struct info {
    char *data;
    struct info *next;
} Srcinfo;

List *foreign_list(void);
void alpm_uninstall(List *pkglist);
Srcinfo *srcinfo(char *pkgname, char *key);

#endif 