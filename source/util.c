#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

#include "../include/util.h"
#include "../include/memory.h"
#include "../include/list.h"

// group these alpm functions and also make this one so that it 
// will only uninstall AUR packages, also try to print the hooks.
void alpm_uninstall(List *pkglist) {

	pu_config_t *pac_conf;
	alpm_handle_t *pacman = NULL;
	alpm_db_t *local_db;
	alpm_pkg_t *pkg;
	alpm_list_t *list, *deps, *error_list;
	alpm_errno_t err;
	List *temp;
	bool not_found = false, success = true;
	int n;

	pac_conf = pu_config_new();
    pu_ui_config_load(pac_conf, "/etc/pacman.conf");
    
	pacman = pu_initialize_handle_from_config(pac_conf);
	if (pacman == NULL) {
		printf("Failed\n");
	}
	
	local_db = alpm_get_localdb(pacman);
	n = alpm_trans_init(pacman, ALPM_TRANS_FLAG_CASCADE | ALPM_TRANS_FLAG_RECURSE | ALPM_TRANS_FLAG_NODEPVERSION);
	if (n != 0) {
		pu_config_free(pac_conf);
		alpm_release(pacman);
		clear_list(pkglist);
		printf("error at alpm_trans_init.\n");
		exit(EXIT_FAILURE);
	}

	//printf("checking dependencies...\n\n");
	printf(BOLD"Packages: "RESET);
	for (temp = pkglist; temp != NULL; temp = temp->next) {
		pkg = alpm_db_get_pkg(local_db, temp->pkgname);
		if (pkg == NULL) {
			printf(BRED"ERROR:"BOLD" %s not found.\n"RESET, temp->pkgname);
			not_found = true;
			continue;
		}
		n = alpm_remove_pkg(pacman, pkg);
		if (n != 0) {
			printf("error removing %s\n", temp->pkgname);
		}
		printf("%s"GREY"-%s "RESET, alpm_pkg_get_name(pkg), alpm_pkg_get_version(pkg));
	}

	if (not_found == true) {
		alpm_trans_release(pacman);
		alpm_release(pacman);
		pu_config_free(pac_conf);
		clear_list(pkglist);
		exit(EXIT_FAILURE);
	}

	list = alpm_trans_get_remove(pacman);
	// check deps

	printf("\n\n"BBLUE"::"BOLD" Do you want to remove these packages? [Y/n] "RESET);
	
	if (prompt() == false) {
		alpm_trans_release(pacman);
		alpm_release(pacman);
		pu_config_free(pac_conf);
		return;
	}
	n = alpm_trans_prepare(pacman, &error_list);
	if (n != 0) {
		printf("error at alpm_trans_prepare.\n");
		success = false;
	}

	n = alpm_trans_commit(pacman, &error_list);
	if (n != 0) {
		printf("error at alpm_trans_commit");
		success = false;
	}

	if (success == false) {
		printf("uninstall failed.\n");
	}
	printf("success!\n");

	alpm_trans_release(pacman);
	alpm_release(pacman);
	pu_config_free(pac_conf);
}

// pipe output of commands to a buffer and return the buffer.
char *get_buffer(const char *cmd) {
	
	char *temp_buffer = NULL, *temp = NULL;
	FILE *p;
	
	str_alloc(&temp_buffer, MAX_BUFFER);

	p = popen(cmd, "r");
	if (p == NULL) {
		printf(BRED"ERROR:"BOLD" Failed in buffer().\n"RESET);
		exit(EXIT_FAILURE);
	}
	fgets(temp_buffer, MAX_BUFFER, p);
	pclose(p);

	if (strlen(temp_buffer) == 0) {
		free(temp_buffer);
		return NULL;
	}

	str_alloc(&temp, (strlen(temp_buffer) + 1));
	strcpy(temp, temp_buffer);
	free(temp_buffer);

	return temp;
}

// compute how many bytes should be allocated to strings.
void get_str(char **p, const char *str, const char *str_var) {
	
	if (str_var != NULL) {
		str_alloc(p, strlen(str) + strlen(str_var) - 1);
		sprintf(*p, str, str_var);
	} else {
		str_alloc(p, strlen(str) + 1);
		sprintf(*p, str);
	}
}

bool prompt(void) {

	char c;

	for (;;) {
        c = tolower(getchar());
        if (c != '\n') {
			while (getchar() != '\n');
		}
        if (c == 'y' || c == '\n') {
			return true;
        } else if (c == 'n') {
			return false;
        }
    }
}

bool file_exists(char *path) {
	
	FILE *f;
	int result;

	f = fopen(path, "r");
	if (f != NULL) {
		fclose(f);
		return true;
	} 

	return false;
}

bool is_dir(char *pkgname) {

	DIR* dir = opendir(pkgname);
	if (dir != NULL) {
		closedir(dir);
		return true;
	}
		
	return false;
}

// remove directories recursively.
void remove_dir(char *path) {

	DIR *dir;
	struct dirent *p;
	struct stat buffer;
	int path_len;
	char temp_path[MAX_BUFFER];

	dir = opendir(path);
	if (dir == NULL) {
		if (file_exists(path) == true) {
			remove(path);
			return;
		}
		printf(BRED"ERROR:"BOLD"Failed to open %s directory.\n"RESET);
		exit(EXIT_FAILURE);
	}

	while ((p = readdir(dir)) != NULL) {
		if (strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0) {
			continue;
		}

		sprintf(temp_path, "%s/%s", path, p->d_name);
		if (is_dir(temp_path)) {
			remove_dir(temp_path);
		} else {
			remove(temp_path);
		}		
	}
	closedir(dir);
	rmdir(path);
}

// get list of items in the .cache/aur directory.
List *get_dir_list(void) {

	DIR *dir;
	struct dirent *p;
	List *dir_list;

	dir = opendir(".");
	if (dir == NULL) {
		printf(BRED"ERROR:"BOLD"Failed to open %s directory.\n"RESET);
		exit(EXIT_FAILURE);
	}
	
	dir_list = list_malloc();

	while ((p = readdir(dir)) != NULL) {
		if (strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0) {
			continue;
		}
		dir_list = add_pkgname(dir_list, p->d_name);
	}
	closedir(dir);

	if (dir_list->pkgname == NULL) {
		free(dir_list);
		return NULL;
	}
	
	return dir_list;
}