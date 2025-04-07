#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/operation.h"
#include "../include/memory.h"
#include "../include/util.h"
#include "../include/list.h"
#include "../include/rpc.h"

bool epoch_update(List *pkg, char *pkgver);
void install(const char *pkgname);
void check_update(List *pkglist);
void fetch_update(char *pkgname);
void less_prompt(const char *pkgname);

void target_clone(char *url) {

    char *str = NULL, pkgname[NAME_LEN] = {'\0'}, *temp;
    register int i;

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
		printf("Removing %s directory before clone...\n", pkgname);
		remove_dir(pkgname);
	}
	
	get_str(&str, GIT_CLONE, url);
    system(str);
    free(str);
	
	less_prompt(pkgname);
}


void aur_clone(char *pkgname) {

    char *str = NULL;

	if (is_dir(pkgname) == true) {
		printf("Removing %s directory before clone...\n", pkgname);
		remove_dir(pkgname);
	}
	
	get_str(&str, AUR_CLONE, pkgname); 
	system(str);
	free(str);
   
	less_prompt(pkgname);   
}

void update(void) {
	
	char c, *str = NULL, *update_list = NULL;
	register int i;
	List *pkglist, *rpc_pkg, *temp;

	str_alloc(&update_list, sizeof(char)); 	// must malloc here in order to realloc later on with strlen(update_list)

	pkglist = get_installed_list();

	printf(BBLUE"::"BOLD" Looking for updates...\n"RESET);
	for (temp = pkglist; pkglist != NULL; pkglist = pkglist->next) {
		
		get_str(&str, AUR_PKG , pkglist->pkgname);
		rpc_pkg = get_rpc_data(str);

		if (strcmp(pkglist->pkgver, rpc_pkg->pkgver) == 0 || epoch_update(pkglist, rpc_pkg->pkgver)) { 
			pkglist->update = true;
			str_alloc(&str, (strlen(pkglist->pkgname) + strlen(pkglist->pkgver) + strlen(rpc_pkg->pkgver) + 69));
			sprintf(str, " %-30s"GREY"%-20s"RESET"-> "BGREEN"%s\n"RESET, pkglist->pkgname, pkglist->pkgver, rpc_pkg->pkgver);
			str_alloc(&update_list, (strlen(update_list) + strlen(str) + 1));
			strcat(update_list, str);
		}

		clear_list(rpc_pkg);
	}
	free(str);

	pkglist = temp;
	if (update_list[0] == '\0') {
		printf(" Nothing to do.\n");
		free(update_list);
		clear_list(pkglist);
		exit(EXIT_SUCCESS);
	} else {
		printf(BBLUE"::"BOLD" Updates are available for:"RESET"\n\n%s\n", update_list);
		free(update_list);
	}

	printf(BBLUE"::"BOLD" Proceed with installation? [Y/n] "RESET);
	if (prompt() == false) {
		clear_list(pkglist);
		return;
	}
	
	check_update(pkglist);
	for (List *temp = pkglist; pkglist != NULL; pkglist = pkglist->next) {
		if (pkglist->update == true) {
			less_prompt(pkglist->pkgname);
		}
	}
	clear_list(temp);
}

void check_update(List *pkglist) {

	while (pkglist != NULL) {
		if (pkglist->update == true) {
			fetch_update(pkglist->pkgname);
		}
		pkglist = pkglist->next;
	}
}

void force_update(char *pkgname) {

	List *pkglist, *pkg;

	pkglist = get_installed_list();
	pkg = find_pkg(pkglist, pkgname);
	

	if (pkg == NULL) {
		printf(BRED"ERROR:"BOLD" %s is not installed."RESET);
		exit(EXIT_FAILURE);
	}
	clear_list(pkglist);
	
	fetch_update(pkgname);
	less_prompt(pkgname);
}

void fetch_update(char *pkgname) {

	char *str = NULL;

	printf(BBLUE"=>"BOLD" Fetching update for %s...\n"RESET, pkgname);
	if (is_dir(pkgname) == false) {
		get_str(&str, AUR_CLONE_NULL, pkgname);
	} else {
		get_str(&str, GIT_PULL_NULL, pkgname);
	}

	system(str);
	free(str);
}

void less_prompt(const char *pkgname) {

	char c, *str = NULL;
	register int i;

	get_str(&str, "%s/PKGBUILD", pkgname);
	if (file_exists(str) != true) {
		printf(BRED"ERROR:"BOLD" PKGBUILD for %s not found\n"RESET, pkgname);
		free(str);
		return;
	}

    printf(BBLUE"::"BOLD" View %s PKGBUILD in less? [Y/n] "RESET, pkgname);

	if (prompt() == false) {
		free(str);
		install(pkgname);
		return;
	}
	
	get_str(&str, LESS_PKGBUILD, pkgname);
	system(str);
	free(str);

	printf(BBLUE"::"BOLD" Continue to install? [Y/n] "RESET);
	if (prompt() == true) {
		install(pkgname);
	}	
}

void install(const char *pkgname) {
    
    char *str = NULL;

    get_str(&str, MAKEPKG, pkgname);	// don't build -debug packages for now.
    system(str);
    free(str);
}

void uninstall(List *list) {

    char *str = NULL;
    
	get_str(&str, UNINSTALL, NULL);
	while (list != NULL) {
		str_alloc(&str, strlen(str) + strlen(list->pkgname) + 2);
		strcat(str, " ");
		strcat(str, list->pkgname);
		if (is_dir(list->pkgname) == true) {
			remove_dir(list->pkgname);
		}
		list = list->next;
	}
	system(str);

	free(str);
}

void clean(void) {

    char *str = NULL;
    List *dir, *pacman, *rpc_pkg, *temp1, *temp2;

    
    dir = get_dir_list();
	if (dir == NULL) {
		printf("Nothing to do.\n");
		return;
	}

	pacman = get_installed_list();
	printf("Cleaning aurx cache dir...\n");
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

	if (rpc_pkglist->pkgname == NULL) {
		printf("No results found for: %s.\n", pkgname);
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

	installed = get_installed_list();
	if (installed == NULL) {
		printf("No installed AUR packages found.\n");
		exit(EXIT_SUCCESS);
	}

	for (temp = installed; installed != NULL; installed = installed->next) {
		printf(BOLD"%s "BGREEN"%s\n"RESET, installed->pkgname, installed->pkgver);
	}
	
	clear_list(temp);
}


// check if an epoch has been added to a PKGBUILD that wasnt present in
// the installed version. without this, if the "pkgver" is 
// higher than the "epoch" (1), the epoch update will be ignored.
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