#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

#include "../include/list.h"
#include "../include/memory.h"
#include "../include/util.h"
#include "../include/pm.h"

// Add packages in correct order
List *add_pkgname(List *list, const char *pkgname) {

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

void add_pkgver(List *list, const char *pkgname, const char *pkgver) {
    
    List *temp;

    temp = find_pkg(list, pkgname);
    str_alloc(&temp->pkgver, strlen(pkgver) + 1);
    strcpy(temp->pkgver, pkgver);
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

List *find_pkg(List *list, const char *pkgname) {

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

// check whether items on a list is installed.
// (for search output)
List *check_status(List *list) {

    List *pkglist, *temp_list, *temp_pkglist;

    pkglist = foreign_list();
    temp_pkglist = pkglist;

    for (temp_list = list; list != NULL; list = list->next) {
        for (pkglist = temp_pkglist; pkglist != NULL; pkglist = pkglist->next) {
            if (strcmp(list->pkgname, pkglist->pkgname) == 0) {
                list->installed = true;
            }
        }
    }
    clear_list(temp_pkglist);

    return temp_list;
}