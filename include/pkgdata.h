#ifndef DATA_H
#define DATA_H

#include <stdbool.h>


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
    char *arch;
    char *zst_path;
    Depends *makedepends;
    Depends *depends;
    Depends *optdepends;
} Srcinfo;


char *read_srcinfo(char *pkgname);
char *zst_path(Srcinfo *pkg);
Depends *add_data(Depends *list, const char *data);
bool is_foreign(char *pkgname);
bool is_installed(char *pkgname);
Srcinfo *populate_pkg(char *pkgname);
List *foreign_list(void);

#endif