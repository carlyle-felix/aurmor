#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct node {
    char *pkgname;
    char *pkgver;
    bool update;
    struct node *next;
} List;

List *struct_malloc(void);
List *add_pkgname(List *list, char *pkgname);
void add_pkgver(List *list);
List *find_pkg(List *list, char *pkgname);
void get_list(List **list, char *cmd);
void clear_list(List *list);

#endif
