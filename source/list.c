#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/list.h"
#include "../include/memory.h"
#include "../include/util.h"

List *get_pkglist(char *cmd) {

    char pkgname[NAME_LEN];
    Buffer pacman_list = NULL, temp_list = NULL;
    register int i;
    List *temp;
    
	pacman_list = get_buffer(cmd);
    if (pacman_list == NULL) {
        return NULL;
    }

    temp = list_malloc();
    temp_list = pacman_list;
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
		add_pkgname(temp, pkgname);
    }
    free(temp_list);

    return temp;
}

// Add packages in reversed order
List *add_pkgname(List *list, char *pkgname) {

    List *temp;
    
    if (list->pkgname == NULL) {
        temp = list;
    } else {
        List *cur, *prev;
        
        temp = list_malloc();
        for (cur = list, prev = NULL; \
            cur != NULL; prev = cur, \
            cur = cur->next);

        if (prev != NULL) {
            prev->next = temp;
        }
    }

    str_alloc(&temp->pkgname, strlen(pkgname) + 1);
    strcpy(temp->pkgname, pkgname);
    
    return list;
}

// add_pkgname() calls from uninstall() wont need versions so do this separately.
void add_pkgver(List *list) {
    
    char *cmd = NULL;

    while (list != NULL) {
       	get_str(&cmd, INSTALLED_PKGVER, list->pkgname);
        list->pkgver = get_buffer(cmd);

        list = list->next;
    }
    free(cmd);
}

// Store data retrieved from json in rpc.c
List *add_json_data(List *list, const char *pkgname, const char *pkgver, int pop) {

    List *temp;

    if (pkgname == NULL) {
        return list;
    } 
    
    if (list->pkgname == NULL) {
        temp = list;   
    } else {
        List *cur, *prev;

        temp = list_malloc();

        for (cur = list, prev = NULL; \
            cur != NULL && cur->pop >= pop; \
            prev = cur, cur = cur->next);

        temp->next = cur;
        if (prev == NULL) {
            list = temp;
        } else {
            prev->next = temp;
        }
    }

    str_alloc(&temp->pkgname, strlen(pkgname) + 1);
    strcpy(temp->pkgname, pkgname);
    str_alloc(&temp->pkgver, strlen(pkgver) + 1);
    strcpy(temp->pkgver, pkgver);
    temp->pop = pop;

    return list;
}

List *find_pkg(List *list, char *pkgname) {

    List *temp;

    for (temp = list; temp != NULL; temp = temp->next) {
        if (strcmp(temp->pkgname, pkgname) == 0) {
            break;
        }
    }
    if (temp == NULL) {
        return NULL;
    } else {
        return temp;
    }
}

List *check_status(List *list) {

    char *str = NULL;
    List *pkglist, *temp_list, *temp_pkglist;

    get_str(&str, INSTALLED, NULL);
	pkglist = get_pkglist(str);
    temp_pkglist = pkglist;

    for (temp_list = list; list != NULL; list = list->next) {
        for (pkglist = temp_pkglist; pkglist != NULL; pkglist = pkglist->next) {
            if (strcmp(list->pkgname, pkglist->pkgname) == 0) {
                list->installed = true;
            }
        }
    }
    free(str);
    clear_list(temp_pkglist);

    return temp_list;
}