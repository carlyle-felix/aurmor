#include <stdio.h>
#include "../include/list.h"
#include "../include/memory.h"
#include "../include/buffer.h"

void get_list(List **list, char *cmd) {

    char pkgname[NAME_LEN];
    Buffer pacman_list = NULL, temp = NULL;
    register int i;

    *list = struct_malloc();
    
	retrieve(cmd, &pacman_list);
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
        //printf("GET_LIST: %s.\n", pkgname);
		add_pkgname(*list, pkgname);
    }
    free(temp);
}

List *add_pkgname(List *list, char *pkgname) {
    
    if (list->pkgname == NULL) {
        mem_alloc(&list->pkgname, ((strlen(pkgname) + 1)));
        strcpy(list->pkgname, pkgname);  
    } else {
        List *cur, *prev, *temp;
        
        temp = struct_malloc();
        mem_alloc(&temp->pkgname, ((strlen(pkgname) + 1)));
        strcpy(temp->pkgname, pkgname);
        for (cur = list, prev = NULL; cur != NULL; prev = cur, cur = cur->next);
        if (prev != NULL) {
            temp->next = NULL;
            prev->next = temp;
        }
    }
    
    return list;
}

void add_pkgver(List *list) {
    
    char *cmd = NULL;

    while (list != NULL) {
       	get_cmd(&cmd, "echo -n $(pacman -Qm | grep %s | cut -f2 -d ' ')", list->pkgname);
        retrieve(cmd, &list->pkgver);

        list = list->next;
    }
    free(cmd);
}

List *find_pkg(List *pkglist, char *pkgname) {

    List *temp;
    for (temp = pkglist; temp != NULL; pkglist = temp, temp = temp->next) {
        if (strcmp(temp->pkgname, pkgname) == 0) {
            return temp;
        }
    }
    return NULL;
}

void clear_list(List *list) {
    
    List *temp;

    while (list != NULL) {
        temp = list;
        list = list->next;
        free(temp->pkgname);
        free(temp->pkgver);
        free(temp);
    }
}