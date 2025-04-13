#ifndef OPERATION_H
#define OPERATION_H

typedef struct package List;

void target_clone(char *url);
void aur_clone(char *pkgnmae);
//void uninstall(List *list);
void install(List *pkglist);
void clean(void);
void print_search(char *pkgname);
void print_installed(void);
void update(void);
void force_update(char *pkgname);

#endif