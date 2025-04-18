
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/operation.h"
#include "../include/memory.h"
#include "../include/util.h"
#include "../include/list.h"
#include "../include/rpc.h"
#include "../include/manager.h"
#include "../include/pkgdata.h"

// prototypes
int aur_clone(char *pkgname);
bool epoch_update(List *pkg, char *pkgver);
List *check_update(List *pkglist);
void fetch_update(char *pkgname);

int aur_clone(char *pkgname) {

    char *str = NULL;
	int res;

	if (is_dir(pkgname) == true) {
		printf(BCYAN"info:"RESET" Removing %s directory before clone...\n", pkgname);
		remove_dir(pkgname);
	}
	
	get_str(&str, AUR_CLONE, pkgname); 
	res = system(str);
	free(str); 

	return res;
}

void target_clone(char *url) {

    char *str = NULL, pkgname[NAME_LEN] = {'\0'}, *temp;
    register int i;
	int res;
	List *target;

	temp = url;
	while (*temp != '\0') {
		temp++;
	}
    while (*temp != '/') {
        temp--;
    }
    temp++;

    for (i = 0; *temp != '.'; i++) {
        pkgname[i] = *temp++;
    }

	if (is_dir(pkgname) == true) {
		printf(BCYAN"info:"RESET" Removing %s directory before clone...\n", pkgname);
		remove_dir(pkgname);
	}
	
	get_str(&str, GIT_CLONE, url);
    res = system(str);
	if (res != 0) {
		printf(BRED"error:"RESET" failed to clone target package.\n");
		return;
	}
	change_owner(pkgname);
    free(str);
	
	if (less_prompt(pkgname) == true) {
		target = list_malloc();
		target = add_pkgname(target, pkgname);
		target->install = true;
		alpm_install(target);
		clear_list(target);
	}
}

void fetch_update(char *pkgname) {

	char *str = NULL;

	printf(BBLUE"=>"BOLD" Fetching update for %s...\n"RESET, pkgname);
	if (is_dir(pkgname) == false) {
		get_str(&str, AUR_CLONE_NULL, pkgname);
		system(str);
	} else {
		change_dir(pkgname);
		get_str(&str, GIT_PULL_NULL, pkgname);
		system(str);
		change_dir("WD");
	}
	change_owner(pkgname);

	free(str);
}

void install(List *pkglist) {
    
	List *temp_list;
	int res;

    for (temp_list = pkglist; temp_list != NULL; temp_list = temp_list->next) {
		res = aur_clone(temp_list->pkgname);
		if (res != 0) {
			printf(BRED"error:"RESET" failed to clone target package.\n");
			continue;
		}
		change_owner(temp_list->pkgname);
		if (less_prompt(temp_list->pkgname) == true) {
			temp_list->install = true;
		}
	}
	alpm_install(pkglist);
}

void update(void) {
	
	char *str = NULL, *update_str = NULL, *debug = NULL, *debug_temp;
	List *pkglist, *update_list, *rpc_pkg, *temp_list;
	bool found_debug = false;

	str_alloc(&update_str, sizeof(char)); 	// must malloc here in order to realloc later on with strlen(update_str)

	pkglist = foreign_list();

	printf(BBLUE"::"BOLD" Looking for updates...\n"RESET);
	for (temp_list = pkglist; pkglist != NULL; pkglist = pkglist->next) {

		// disregard -debug;
		get_str(&debug, "%s", pkglist->pkgname);
		for(debug_temp = debug; *debug_temp != '\0'; debug_temp++) {
			if(*debug_temp == '-' && strcmp(debug_temp, "-debug") == 0) {
				found_debug = true;
				break;
			}
		}
		if (found_debug == true) {
			found_debug = false;
			continue;
		}
		
		get_str(&str, AUR_PKG , pkglist->pkgname);
		rpc_pkg = get_rpc_data(str);

		if (strcmp(pkglist->pkgver, rpc_pkg->pkgver) < 0 || epoch_update(pkglist, rpc_pkg->pkgver)) { 
			pkglist->update = true;
			str_alloc(&str, (strlen(pkglist->pkgname) + strlen(pkglist->pkgver) + strlen(rpc_pkg->pkgver) + 69)); // get the counting correct.
			sprintf(str, " %-30s"GREY"%-20s"RESET"-> "BGREEN"%s\n"RESET, pkglist->pkgname, pkglist->pkgver, rpc_pkg->pkgver);
			str_alloc(&update_str, (strlen(update_str) + strlen(str) + 1));
			strcat(update_str, str);
		}	
		clear_list(rpc_pkg);
	}
	free(str);
	free(debug);

	pkglist = temp_list;
	if (update_str[0] == '\0') {
		printf("\n Nothing to do.\n");
		free(update_str);
		clear_list(pkglist);
		exit(EXIT_SUCCESS);
	} else {
		printf(BBLUE"::"BOLD" Updates are available for:"RESET"\n\n%s\n", update_str);
		free(update_str);
	}
 
	printf(BBLUE"::"BOLD" Proceed with installation? [Y/n] "RESET);
	if (prompt() == false) {
		clear_list(pkglist);
		return;
	}
	
	update_list = check_update(pkglist);
	clear_list(pkglist);
	for (temp_list = update_list; temp_list != NULL; temp_list = temp_list->next) {
		if (less_prompt(update_list->pkgname) == true) {
			temp_list->install = true;
		}
	}
	alpm_install(update_list);
	clear_list(update_list);
}

/* 
 * check if an epoch has been added to a PKGBUILD that wasnt present in
 * the installed version. without this, if the "pkgver" is 
 * higher than the "epoch" (1), the epoch update will be ignored.
 */
bool epoch_update(List *pkg, char *pkgver) {

	char *installed_pkgver, *update_pkgver;
	
	installed_pkgver = pkg->pkgver;
	update_pkgver = pkgver;

	while (*installed_pkgver != ':' && *installed_pkgver != '\0') {
		installed_pkgver++;
	}
	while (*update_pkgver != ':' && *update_pkgver != '\0') {
		update_pkgver++;
	}

	if (*installed_pkgver == '\0' && *update_pkgver == ':') {
		return true;
	} else {
		return false;
	}
}

List *check_update(List *pkglist) {

	List *update_list = list_malloc();

	while (pkglist != NULL) {
		if (pkglist->update == true) {
			fetch_update(pkglist->pkgname);
			update_list = add_pkgname(update_list, pkglist->pkgname);
		}
		pkglist = pkglist->next;
	}

	return update_list;
}

void force_update(List *pkglist) {

	List *temp_list;

	for (temp_list = pkglist; temp_list != NULL; temp_list = temp_list->next) {
		if (is_foreign(temp_list->pkgname) != true) {
			printf(BRED"error: "RESET" %s is not installed.\n", temp_list->pkgname);
			break;
		}
	}

	for (; temp_list == NULL && pkglist != NULL; pkglist = pkglist->next) {
		fetch_update(pkglist->pkgname);
		less_prompt(pkglist->pkgname);
	}
}

void clean(void) {

    char *str = NULL;
    List *dir, *pacman, *temp1, *temp2;
 
    dir = get_dir_list();
	if (dir == NULL) {
		printf("\n Nothing to do.\n");
		return;
	}

	pacman = foreign_list();
	printf(BGREEN"==>"BOLD"Cleaning aurx cache dir...\n"RESET);
    for (temp1 = pacman, temp2 = dir; dir != NULL; dir = dir->next) {
        remove_dir(dir->pkgname);
		if (pacman != NULL) {
			pacman = pacman->next;
		}
    }
	free(str);
    clear_list(temp1);
    clear_list(temp2);
}

void print_search(char *pkgname) {

    char *str = NULL;
    List *rpc_pkglist, *temp;
	 
    get_str(&str, AUR_SEARCH, pkgname);
    rpc_pkglist = get_rpc_data(str);

	if (rpc_pkglist == NULL) {
		printf("\n No results found for: %s.\n", pkgname);
		free(str);
    	clear_list(rpc_pkglist);
		exit(EXIT_SUCCESS);
	}

	rpc_pkglist = check_status(rpc_pkglist);
	for (temp = rpc_pkglist; rpc_pkglist != NULL; rpc_pkglist = rpc_pkglist->next) {
		printf(BOLD"%s "BGREEN"%s"RESET, rpc_pkglist->pkgname, rpc_pkglist->pkgver);
		if (rpc_pkglist->installed == true) {
			printf(BCYAN"\t[installed]"RESET);
		}
		printf("\n");
	}
	free(str);
    clear_list(temp);
}

void print_installed(void) {
    
    List *installed, *temp;

	installed = foreign_list();
	if (installed == NULL) {
		printf("\n No installed AUR packages found.\n");
		exit(EXIT_SUCCESS);
	}

	for (temp = installed; installed != NULL; installed = installed->next) {
		printf(BOLD"%s "BGREEN"%s\n"RESET, installed->pkgname, installed->pkgver);
	}
	clear_list(temp);
}