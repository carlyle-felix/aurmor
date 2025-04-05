#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/operation.h"
#include "../include/memory.h"
#include "../include/util.h"
#include "../include/list.h"
#include "../include/rpc.h"

bool epoch_update(List *pkg, char *pkgver);
char *not_on_aur(char *pkgname);
void install(const char *pkgname);
void get_update(List *pkglist);

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
		printf(" Removing %s directory before clone...", pkgname);
		rmdir(pkgname);
	}
	get_str(&str, GIT_CLONE, url);
	
    system(str);
    free(str);
	
	less_prompt(pkgname);
}

void aur_clone(char *pkgname) {

    char *str = NULL;

	if (is_dir(pkgname) == true) {
		printf(" Removing %s directory before clone...", pkgname);
		rmdir(pkgname);
	}
	get_str(&str, AUR_CLONE, pkgname);
    
	system(str);
	free(str);
   
	less_prompt(pkgname);   
}

void update(void) {
	
	char c, *str = NULL, *update_list = NULL, *pkgver = NULL;
	register int i;
	List *pkglist, *rpc_pkg, *temp;

	str_malloc(&update_list, sizeof(char)); 	// must malloc here in order to realloc later on with strlen(update_list)

	pkglist = get_list(INSTALLED);
	add_pkgver(pkglist);

	printf(BBLUE"::"BOLD" Looking for updates...\n"RESET);
	for (temp = pkglist; pkglist != NULL; pkglist = pkglist->next) {
		get_str(&str, AUR_PKG , pkglist->pkgname);
		rpc_pkg = get_rpc_data(str);
		if (rpc_pkg->pkgname != NULL) {
			pkgver = rpc_pkg->pkgver;
		} else {
			pkgver = not_on_aur(pkglist->pkgname);
			pkglist->rpc_pkg = false;
		}
		
		if (strcmp(pkglist->pkgver, pkgver) < 0 || epoch_update(pkglist, pkgver)) { 
			pkglist->update = true;
			str_malloc(&str, (strlen(pkglist->pkgname) + strlen(pkglist->pkgver) + strlen(pkgver) + 68));
			sprintf(str, " %-30s"GREY"%-20s"RESET"-> "BGREEN"%s\n"RESET, pkglist->pkgname, pkglist->pkgver, pkgver);
			str_malloc(&update_list, (strlen(update_list) + strlen(str) + 1));
			strcat(update_list, str);
		}

		if (rpc_pkg->pkgname == NULL) {
			free(pkgver);
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
	if (prompt() == true) {
		get_update(pkglist);
		List *temp = pkglist;
		while (pkglist != NULL) {
			if (pkglist->update == true) {
				less_prompt(pkglist->pkgname);
			}
			pkglist = pkglist->next;
		}
		clear_list(temp);
	} else {
		clear_list(pkglist);
	}
}

void get_update(List *pkglist) {

	char *str = NULL;
	bool dir;

	while (pkglist != NULL) {
		dir = is_dir(pkglist->pkgname);
		if (pkglist->rpc_pkg == true && pkglist->update == true && dir == false) {
			printf(BBLUE" ->"BOLD" Fetching update for %s...\n"RESET, pkglist->pkgname);
			get_str(&str, AUR_CLONE_NULL, pkglist->pkgname);
		} else if (pkglist->update == true && dir == true) {
			printf(BBLUE" ->"BOLD" Fetching update for %s...\n"RESET, pkglist->pkgname);
			get_str(&str, GIT_PULL_NULL, pkglist->pkgname);
		} 
		system(str);
		pkglist = pkglist->next;
	}
	free(str);
}

void force_update(char *pkgname) {

	char *str = NULL;
	List *pkglist, *pkg;

	pkglist = get_list(INSTALLED);
	pkg = find_pkg(pkglist, pkgname);
	if (pkg == NULL) {
		printf(BRED"ERROR:"BOLD" %s is not installed."RESET);
		exit(EXIT_FAILURE);
	}

	printf(BBLUE" ->"BOLD" Fetching update for %s...\n"RESET, pkglist->pkgname);
	if (is_dir(pkgname) == false) {
		get_str(&str, AUR_CLONE_NULL, pkgname);
	} else {
		get_str(&str, GIT_PULL_NULL, pkgname);
	}
	system(str);
	free(str);

	less_prompt(pkgname);
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
	if (prompt() == true) {
		get_str(&str, LESS_PKGBUILD, pkgname);
		system(str);
		free(str);

		printf(BBLUE"::"BOLD" Continue to install? [Y/n] "RESET);
		if (prompt() == true) {
			install(pkgname);
			return;
		} else {
			return;
		} 
	} else {
		free(str);
		install(pkgname);
		return;
	}
}

void install(const char *pkgname) {
    
    char *str = NULL;

    get_str(&str, MAKEPKG, pkgname);	// don't build -debug packages for now.
    system(str);
    free(str);
}

void uninstall(char *pkgname) {

    char *str = NULL;
    
    get_str(&str, UNINSTALL, pkgname);
    system(str);

    if (pkgname == NULL) {
        free(pkgname);
    }
    free(str);
}

void clean(void) {

    char *str = NULL;
    List *dir, *pacman, *rpc_pkg, *temp1, *temp2;

    
    dir = get_list(DIR_LIST);
	if (dir == NULL) {
		printf("Nothing to do.\n");
		return;
	}

	pacman = get_list(INSTALLED);
	printf("Cleaning aurx cache dir...\n");
	temp1 = pacman;
    temp2 = dir;
    while (dir != NULL) {
        if (pacman == NULL || strcmp(dir->pkgname, pacman->pkgname) != 0) {
            get_str(&str, RM_DIR, dir->pkgname);
            system(str);
        } else if (strcmp(dir->pkgname, pacman->pkgname) == 0) {
			get_str(&str, AUR_PKG, dir->pkgname);
			rpc_pkg = get_rpc_data(str);
			if (rpc_pkg->pkgname != NULL) {
				get_str(&str, RM_DIR, dir->pkgname);
            	system(str);
			}
			clear_list(rpc_pkg);
		} else {
            get_str(&str, GIT_CLEAN, dir->pkgname);
            system(str);
        }
        dir = dir->next;
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
	} else {
		rpc_pkglist = check_status(rpc_pkglist);
		for (temp = rpc_pkglist; rpc_pkglist != NULL; rpc_pkglist = rpc_pkglist->next) {
			printf(BOLD"%s "BGREEN"%s"RESET, rpc_pkglist->pkgname, rpc_pkglist->pkgver);
			if (rpc_pkglist->installed == true) {
				printf(BCYAN"\t[installed]"RESET);
			}
			printf("\n");
		}
		rpc_pkglist = temp;
	}

	free(str);
    clear_list(rpc_pkglist);
}

void list_packages(void) {
    
    system(QUERY_INSTALLED);
}

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

char *not_on_aur(char *pkgname) {

	char *str = NULL, *full_ver = NULL;
	Buffer epoch = NULL, pkgver = NULL, pkgrel = NULL;

	// update pkgname source folder in ~/.config/aurx
	if (is_dir(pkgname) == true) {
		get_str(&str, GIT_PULL_NULL, pkgname);
		system(str);
	} else {
		printf(BRED"ERROR:"BOLD" No source directory found for %s.\n", pkgname);
	}	
	

	// get epoch for current pkgname from pkgbuild
	get_str(&str, PKGBUILD_CMD(epoch), pkgname);
	epoch = get_buffer(str);

	// get pkgver for current pkgname from pkgbuild
	get_str(&str, PKGBUILD_CMD(pkgver), pkgname);
	pkgver = get_buffer(str);

	// get pkgrel for current pkgname from pkgbuild
	get_str(&str, PKGBUILD_CMD(pkgrel), pkgname);
	pkgrel = get_buffer(str);	

	// copy epoch:pkgver-pkgrel into full_ver
	str_malloc(&full_ver, (strlen(epoch) + strlen(pkgver) + strlen(pkgrel)) + 3);
	if (epoch[0] == '\0') {
		sprintf(full_ver, "%s-%s", pkgver, pkgrel);
	} else if (pkgrel[0] == '\0') {
		sprintf(full_ver, "%s:%s", epoch, pkgver);
	} else if (epoch[0] == '\0' && pkgrel[0] == '\0') {
		sprintf(full_ver, "%s", pkgver);
	} else {
		sprintf(full_ver, "%s:%s-%s", epoch, pkgver, pkgrel);
	}
	free(str);
	free(epoch);
	free(pkgver);
	free(pkgrel);

	return full_ver;
}