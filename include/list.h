#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#define INSTALLED_PKGVER "echo -n $(pacman -Qm | grep %s | cut -f2 -d ' ')"

typedef struct node {
    char *pkgname;
    char *pkgver;
    int pop;
    bool installed;
    bool update;
    struct node *next;
} List;

List *get_installed_list(void);
List *add_pkgname(List *list, const char *pkgname);
void add_pkgver(List *list, const char *pkgname, const char *pkgver);
List *find_pkg(List *list, const char *pkgname);
List *add_json_data(List *list, const char *pkgname, const char *pkgver, int pop);
List *check_status(List *list);

#endif