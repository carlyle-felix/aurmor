#include <stdio.h>
#include "../include/memory.h"
#include "../include/list.h"
#include "../include/buffer.h"

List *add_pkg(List *pkglist, char *pkgname, char *pkgver) {

    List *temp = struct_malloc();
    mem_alloc(&temp->pkgname, VSTR(temp->pkgname), ((strlen(pkgname) + 1)));
    mem_alloc(&temp->pkgver, VSTR(temp->pkgver), ((strlen(pkgver) + 1)));
    strcpy(temp->pkgname, pkgname);
    strcpy(temp->pkgver, pkgver);
    temp->update = false;
    
    temp->next = pkglist;
    pkglist = temp;
    return temp;
}

List *remove_pkg(List *pkglist, char *pkgname) {

    List *prev, *cur;
    for (cur = pkglist, prev = NULL; cur != NULL; prev = cur, cur = cur->next) {
        if (strcmp(cur->pkgname, pkgname) == 0) {
            break;
        } 
    }
    if (cur == NULL) {
        printf("Package %s not found in pkglist", pkgname);
        return pkglist;
    } 
    if (prev == NULL) {
        pkglist = cur->next;
    } else {
        prev->next = cur->next;
    }
    
    free(cur->pkgname);
    free(cur->pkgver);
    free(cur);

    return pkglist;
}

List *find_pkg(List *pkglist, char *pkgname) {

    List *temp;
    for (temp = pkglist; temp != NULL; pkglist = temp, temp = temp->next) {
        if (strcmp(temp->pkgname, pkgname) == 0) {
            return temp;
        }
    }
    printf("Package %s not found in pkglist", pkgname);
    return NULL;
}

void clear_list(List *pkglist) {
    
    List *temp;

    while (pkglist != NULL) {
        temp = pkglist;
        pkglist = pkglist->next;
        free(temp->pkgname);
        free(temp->pkgver);
        free(temp);
    }
}

void sort_list(List **pkglist) {
    
    List *cur, *prev, *next;
    prev = struct_malloc();
    cur = *pkglist;
    while (cur != NULL) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    *pkglist = prev->next;
    free(prev);
}

void get_pkglist(List **pkglist) {

    char pkgname[NAME_LEN], *pkgver = NULL;
    Buffer pacman_list = NULL, cmd = NULL, temp = NULL;
    register int i;

    *pkglist = struct_malloc();
    
    // get pkgname and pkgver of installed packages and store in pkglist
	get_buffer("echo -n $(pacman -Qmq)", &pacman_list);
    temp = pacman_list;
    while (*pacman_list != '\0') {
		for (i = 0; i < NAME_LEN; i++) {
			pkgname[i] = '\0';
		}
		for (i = 0; *pacman_list != ' ' && *pacman_list != '\0'; i++) {
			pkgname[i] = *pacman_list++;	
		}
		if (*pacman_list != '\0') {
			pacman_list++;
		}
		
		// add pkgname and pkgver to pkglist
		mem_alloc(&cmd, VSTR(cmd), (sizeof(char) * (strlen(pkgname) + 47)));
		sprintf(cmd, "echo -n $(pacman -Qm | grep %s | cut -f2 -d ' ')", pkgname);
		get_buffer(cmd, &pkgver);
		*pkglist = add_pkg(*pkglist, pkgname, pkgver);
    }
    free(temp);
    free(cmd);
    free(pkgver);

    sort_list(pkglist);
}