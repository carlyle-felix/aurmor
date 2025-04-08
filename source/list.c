#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

#include "../include/list.h"
#include "../include/memory.h"
#include "../include/util.h"

List *get_installed_list(void) {
    
    pu_config_t *pac_conf;
    alpm_handle_t *pacman, *pacman_conf;
    alpm_errno_t err;
    alpm_db_t *installed_db;
    alpm_list_t *installed, *sync, *reset;
    alpm_pkg_t *pkg;
    List *aur;
   
    pac_conf = pu_config_new();
    pu_ui_config_load(pac_conf, "/etc/pacman.conf");
    pacman_conf = pu_initialize_handle_from_config(pac_conf);
    sync = pu_register_syncdbs(pacman_conf, pac_conf->repos);

    pacman = alpm_initialize("/", "/var/lib/pacman/", &err);
    if (pacman == NULL) {
        printf(BRED"ERROR:"BOLD" alpm_initialize %s\n"RESET, alpm_strerror(err));
        exit(EXIT_FAILURE);
    }
    installed_db = alpm_get_localdb(pacman);
    installed = alpm_db_get_pkgcache(installed_db);
    
    aur = list_malloc();
    for (reset = sync; installed != NULL; installed = alpm_list_next(installed)) {
        for (sync = reset; sync != NULL; sync = alpm_list_next(sync)) {
            pkg = alpm_db_get_pkg(sync->data, alpm_pkg_get_name(installed->data));
            if (pkg != NULL) {
                break;
            }
        }    

        if (pkg == NULL) {
            add_pkgname(aur, alpm_pkg_get_name(installed->data));
            add_pkgver(aur, alpm_pkg_get_name(installed->data), alpm_pkg_get_version(installed->data));
        }
    }

    alpm_release(pacman);
    alpm_release(pacman_conf);
    pu_config_free(pac_conf);

    return aur;
}

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

    pkglist = get_installed_list();
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