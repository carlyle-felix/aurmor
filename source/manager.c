#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/manager.h"
#include "../include/list.h"
#include "../include/util.h"
#include "../include/memory.h"

void rm_req_depends(alpm_handle_t *local, alpm_pkg_t *pkg);
void list_free(char *data);		// for alpm_list_fn_free
alpm_list_t *alpm_local(alpm_handle_t **local, alpm_errno_t *err);
alpm_list_t *alpm_repos(alpm_handle_t **repo);
bool is_foreign(char *pkgname);
bool is_installed(char *pkgname);
void resolve_deps(alpm_handle_t *local, alpm_list_t *repo_db_list, alpm_pkg_t *pkg);
Srcinfo *read_srcinfo(char *pkgname, char *key);
void clear_deps(Srcinfo *list);

// return list of packages in localdb - probably don't need this.
alpm_list_t *alpm_local(alpm_handle_t **local, alpm_errno_t *err) {

    alpm_db_t *local_db;
    alpm_list_t *local_list;

    *local = alpm_initialize("/", "/var/lib/pacman", err);
    local_db = alpm_get_localdb(*local);
    local_list = alpm_db_get_pkgcache(local_db);

    return local_list; 
}

// list of repos in pacman.conf
alpm_list_t *alpm_repos(alpm_handle_t **repo) {

	pu_config_t *conf;
	alpm_db_t *repos_db;
	alpm_list_t *repo_list;

	conf = pu_config_new();
    pu_ui_config_load(conf, "/etc/pacman.conf");
    *repo = pu_initialize_handle_from_config(conf);
	if (*repo == NULL) {
		printf("pu_initialize_buffer_from_config failed.\n");
	}
    repo_list = pu_register_syncdbs(*repo, conf->repos);

    pu_config_free(conf);

	return repo_list;
}

List *foreign_list(void) {
    
    alpm_handle_t *local = NULL, *repo = NULL;
    alpm_errno_t err;
    alpm_list_t *local_list, *repo_list, *reset;
    alpm_pkg_t *pkg;
    List *aur;
   
	repo_list = alpm_repos(&repo);	
    local_list = alpm_local(&local, &err);
    
    aur = list_malloc();
    for (reset = repo_list; local_list != NULL; local_list = alpm_list_next(local_list)) {
        for (repo_list = reset; repo_list != NULL; repo_list = alpm_list_next(repo_list)) {
            pkg = alpm_db_get_pkg(repo_list->data, alpm_pkg_get_name(local_list->data));
            if (pkg != NULL) {
                break;
            }
        }    

        if (pkg == NULL) {
            add_pkgname(aur, alpm_pkg_get_name(local_list->data));
            add_pkgver(aur, alpm_pkg_get_name(local_list->data), alpm_pkg_get_version(local_list->data));
        }
    }

    alpm_release(local);
    alpm_release(repo);

    return aur;
}

bool is_foreign(char *pkgname) {

	List *list, *temp;
	
	list = foreign_list();
	for (temp = list; temp != NULL; temp = temp->next) {
		if (strcmp(temp->pkgname, pkgname) == 0) {
			break;
		}
	}
	clear_list(list);
	return (temp != NULL);
}

bool is_installed(char *pkgname) {

	alpm_handle_t *local;
	alpm_db_t *local_db;
	alpm_errno_t err;
	alpm_pkg_t *pkg;
	bool installed = true;

	alpm_local(&local, &err);
	local_db = alpm_get_localdb(local);
	pkg = alpm_db_get_pkg(local_db, pkgname);
	if (pkg == NULL) {
		installed = false;
	}
	alpm_release(local);

	return installed;
}

void list_free(char *data) {
	
	if (data == NULL){
		return;
	}
	free(data);
}

/*	
 * 	check if any packages besides the one being removed requires one of its deps
 * 	before removing the dep, if required the dep is still needed return true.
 */
void rm_req_depends(alpm_handle_t *local, alpm_pkg_t *pkg) {
	
	alpm_db_t *local_db;
	alpm_pkg_t *dep_pkg;
	alpm_depend_t *dep;
	alpm_list_t *dep_list, *req_list, *temp;

	local_db = alpm_get_localdb(local);
	
	// search localdb for pkgname and use it to get a list of
	// depenencies for package and traverse the list.
	dep_list = alpm_pkg_get_depends(pkg);
	for (dep_list; dep_list != NULL; dep_list = alpm_list_next(dep_list)) {
		dep = dep_list->data;

		// find the dependency in the localdb
		dep_pkg = alpm_db_get_pkg(local_db, dep->name);

		// get a list of packages that requires the dependency
		req_list = alpm_pkg_compute_requiredby(dep_pkg);
		
		// check if theres only one package in the required list
		// if this is true, there are no other packages that requires it.
		if (alpm_list_count(req_list) == 1 && strcmp(req_list->data, alpm_pkg_get_name(pkg)) == 0) {
			alpm_remove_pkg(local, dep_pkg);
			rm_req_depends(local, dep_pkg);
		}

		alpm_list_free_inner(req_list, (alpm_list_fn_free) list_free);
		alpm_list_free(req_list);
	}
}

void alpm_uninstall(List *pkglist) {

	alpm_handle_t *local;
	alpm_db_t *local_db;
	alpm_pkg_t *pkg;
	alpm_list_t *list, *opt, *error_list;
	alpm_errno_t err;
	List *temp;
	bool proceed = true, success = true;
	int res;

	list = alpm_local(&local, &err);
	local_db = alpm_get_localdb(local);

	res = alpm_trans_init(local, ALPM_TRANS_FLAG_CASCADE | ALPM_TRANS_FLAG_NODEPVERSION);
	if (res != 0) {
		printf(BOLD"Elevated privilage required to perform this operation ("BRED"root"BOLD").\n"RESET);
		printf(BRED"error:"RESET" alpm_trans_init: %s\n", alpm_strerror(alpm_errno(local)));
		alpm_release(local);
		clear_list(pkglist);
		exit(EXIT_FAILURE);
	}
	
	printf("Checking dependencies...\n\n");
	for (temp = pkglist; temp != NULL; temp = temp->next) {
		if (is_installed(temp->pkgname) == false) {
			printf(BRED"error:"RESET" target not found: %s.\n", temp->pkgname);
			proceed = false;
			break;
		} else if (is_foreign(temp->pkgname) == false) {
			printf(BRED"error:"RESET" %s is not an AUR package.\n", temp->pkgname);
			proceed = false;
			break;
		}
		
		pkg = alpm_db_get_pkg(local_db, temp->pkgname);
		if (pkg == NULL) {
			printf(BRED"error:"RESET" target not found in local_db: %s.\n", temp->pkgname);
			proceed = false;
			continue;
		}
		rm_req_depends(local, pkg);
		res = alpm_remove_pkg(local, pkg);
		if (res != 0) {
			printf(BRED"error:"RESET" alpm_remove_pkg: %s\n", alpm_strerror(alpm_errno(local)));
		}
	}

	if (proceed == false) {
		alpm_trans_release(local);
		alpm_release(local);
		clear_list(pkglist);
		exit(EXIT_FAILURE);
	}
	
	printf(BOLD"Packages: "RESET);
	list = alpm_trans_get_remove(local);
	for (; list != NULL; list = alpm_list_next(list)) {
		printf("%s"GREY"-%s  "RESET, alpm_pkg_get_name(list->data), alpm_pkg_get_version(list->data));
	}

	printf("\n\n"BBLUE"::"BOLD" Do you want to remove these packages? [Y/n] "RESET);
	if (prompt() == false) {
		alpm_trans_release(local);
		alpm_release(local);
		return;
	}

	res = alpm_trans_prepare(local, &error_list);
	if (res != 0) {
		printf(BRED"error:"RESET" alpm_trans_prepare: %s\n", alpm_strerror(alpm_errno(local)));
	}

	res = alpm_trans_commit(local, &error_list);
	if (res != 0) {
		printf(BRED"error:"RESET" alpm_trans_commit: %s\n", alpm_strerror(alpm_errno(local)));
		success = false;
	}

	if (success == true) {
		printf(BGREEN"=>"BOLD" Success\n"RESET);
	}
	
	alpm_trans_release(local);
	alpm_release(local);
}

void alpm_install(List *list) {

	alpm_handle_t *repo, *local;
	alpm_list_t *repo_db_list, *db_temp, *local_list, *add_list, *error_list;
	alpm_pkg_t *pkg;
	alpm_errno_t err;
	Srcinfo *deps, *temp_deps;
	int res;
	
	repo_db_list = alpm_repos(&local);
	

	res = alpm_trans_init(local, ALPM_TRANS_FLAG_NODEPVERSION);
	if (res != 0) {
		printf("error: trans_init: %s\n", alpm_strerror(alpm_errno(local)));
	}

	// get the dep list from srcinfo.
	for (;list != NULL; list = list->next) {
		deps = read_srcinfo(list->pkgname, "depends");

		// ignore deps that are already installed
		if (is_installed(list->pkgname) == true) {
			clear_deps(deps);
			continue;
		}

		// pass deps one by one to resolve_deps
		for (temp_deps = deps; temp_deps != NULL; temp_deps = temp_deps->next) {
			for (db_temp = repo_db_list; db_temp != NULL; db_temp = alpm_list_next(db_temp)) {
				pkg = alpm_db_get_pkg(db_temp->data, temp_deps->data);

				if (pkg != NULL) {
					if (is_installed(temp_deps->data) == false) {
						res = alpm_add_pkg(local, pkg);
						if (res != 0) {
							printf("error: alpm_add_pkg (install): %s\n", alpm_strerror(alpm_errno(local)));
						}
					}
					resolve_deps(local, repo_db_list, pkg);
				}

			}
		}
		
		clear_deps(deps);
	}

	// list deps for test.
	add_list = alpm_trans_get_add(local);
	printf("\nadd_list size: %d\n", alpm_list_count(add_list));
	while (add_list != NULL) {
		printf("package add: %s\n", alpm_pkg_get_name(add_list->data));
		add_list = alpm_list_next(add_list);
	}

	res = alpm_trans_prepare(local, &error_list);
	printf("prep res: %d\n", res);
	if (res != 0) {
		printf("error: trans_prep: %s\n", alpm_strerror(alpm_errno(local)));
	}
	res = alpm_trans_commit(local, &error_list);
	if (res != 0) {
		printf("error: trans_commit: %s\n", alpm_strerror(alpm_errno(local)));
	}
	if (res == 0) {
		printf("success.\n");
	}
	
	alpm_trans_release(local);
	alpm_release(local);
}

void resolve_deps(alpm_handle_t *local, alpm_list_t *repo_db_list, alpm_pkg_t *pkg) {

	alpm_list_t *db_temp, *req_deps;
	alpm_pkg_t *dep_pkg;
	alpm_depend_t *dep;
	int res;

	req_deps = alpm_pkg_get_depends(pkg);

	for (; req_deps != NULL; req_deps = alpm_list_next(req_deps)) {
		dep = req_deps->data;
		for (db_temp = repo_db_list; db_temp != NULL; db_temp = alpm_list_next(db_temp)) {
			dep_pkg = alpm_db_get_pkg(db_temp->data, dep->name);
			if (dep_pkg != NULL && is_installed(dep->name) == false) {
				res = alpm_add_pkg(local, dep_pkg);
				printf("resolve: %s\n", alpm_pkg_get_name(dep_pkg));
				if (res != 0) {
					printf("error: alpm_add_pkg: %s\n", alpm_strerror(alpm_errno(local)));
				}

				alpm_pkg_set_reason(dep_pkg, ALPM_PKG_REASON_DEPEND);
				resolve_deps(local, repo_db_list, dep_pkg);
			}
		}
	}
}

/*
 * returns a list of keys found.
 * pkgname: as found in .SRCINFO.
 * key: .SRCINFO field to search for.
 * TODO: deal with version requirements.
 */
Srcinfo *read_srcinfo(char *pkgname, char *key) {

	Srcinfo *list, *temp_list;
	FILE *srcinfo; 
	char *buffer = NULL, *temp, *str = key, dep[MAX_BUFFER] = {'\0'}, *data = NULL; //excessive.
	int read = 0, max = MAX_BUFFER, key_len;
	register int i;
	
	chdir("/home/carlyle/.cache/aurx/spotify");

	str_alloc(&buffer, max);
	for (;;) {
		srcinfo = fopen(".SRCINFO", "r");
		if (srcinfo == NULL) {
			printf(BRED"error:"RESET" failed to open %s/.SRCINFO", pkgname);
		}
		read = fread(buffer, sizeof(char), max, srcinfo);
		if (read == max) {
			max = read * 2;
			str_alloc(&buffer, max);
		} else {
			fclose(srcinfo);
			break;
		}
		fclose(srcinfo);
	}
	buffer[read] = '\0';

	list = malloc(sizeof(Srcinfo));
	list->data = NULL;
	list->next = NULL;

	key_len = strlen(key);
	//str_alloc(&str, key_len + 1);
	for (temp = buffer; *temp != '\0'; temp++) {
		// advance past the tab.
		if (*temp == '\t') {
			temp++;
		}

		// check the first letter, if no match, advance to newline char.
		if (*temp != key[0]) {
			while (*temp != '\n') {
				temp++;
			}
			continue;
		}

		for (i = 0; i < key_len; i++) {
			if (*temp++ != str[i]) {
				// if no match, skip line, advance to newline char.
				while (*temp != '\n') {
					temp++;
				}
				i = 0;
				break;
			}
		}
		
		if (i == (key_len)) {
			dep[0] = '\0';
			while (*temp == ' ' || *temp == '=') {
				temp++;
			}
			for (i = 0; *temp != '\n' && *temp != '>'; i++) {
				dep[i] = *temp++;
			}
			printf("dep: %s\n", dep);
			temp_list = malloc(sizeof(Srcinfo));
			temp_list->data = NULL;
			temp_list->next = NULL;
			str_alloc(&temp_list->data, strlen(dep) + 1);
			strcpy(temp_list->data, dep);
			temp_list->next = list;
			list = temp_list;
		}
	}
	change_dir("WD");
	free(buffer);

	return list;
}

void clear_deps(Srcinfo *list) {

	Srcinfo *temp_list;

	while (list != NULL) {
		// off by one.
		printf("clear_list: data: %s\n", list->data);
		temp_list = list;
		list = list->next;
		free(temp_list->data);
		free(temp_list);
	}
}