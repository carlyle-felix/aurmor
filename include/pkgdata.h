#ifndef DATA_H
#define DATA_H

#include <stdbool.h>

typedef struct srcinfo Srcinfo;
typedef struct depends Depends;
typedef struct package List;

char *read_srcinfo(char *pkgname);
char *zst_path(Srcinfo *pkg);
Depends *add_data(Depends *list, const char *data);
bool is_foreign(char *pkgname);
bool is_installed(char *pkgname);
Srcinfo *populate_pkg(char *pkgname);
List *foreign_list(void);

#endif