#ifndef DATA_H
#define DATA_H

#include <stdbool.h>


typedef struct package List;

typedef struct depends {
    char *data;
    struct depends *next;
} Depends;

typedef struct pkg {
    char *pkgname;
    char *arch;
    char *zst_path;
    Depends *depends;
    Depends *optdepends;
    struct pkg *next;
} Pkginfo;

typedef struct base {
    char *pkgbase;
    char *arch;
    char *epoch;
    char *pkgver;
    char *pkgrel;
    Depends *makedepends;
    Pkginfo *pkg;
} Pkgbase;


char *read_srcinfo(char *pkgname);
//char *zst_path(Pkginfo *pkg);
Depends *add_data(Depends *list, const char *data);
bool is_foreign(char *pkgname);
bool is_installed(char *pkgname);
Pkgbase *populate_pkg(char *pkgname);
List *foreign_list(void);

#endif