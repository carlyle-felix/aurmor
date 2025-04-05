#ifndef OPERATION_H
#define OPERATION_H

void target_clone(char *url);
void aur_clone(char *pkgnmae);
void uninstall(char *pkgnames);
void clean(void);
void print_search(char *pkgname);
void list_packages(void);
void update(void);
void force_update(char *pkgname);

#endif