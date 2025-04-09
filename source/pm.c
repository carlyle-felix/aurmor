#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <alpm.h>
#include <alpm_list.h>
#include <pacutils.h>

#include "../include/pm.h"
#include "../include/list.h"
#include "../include/util.h"
#include "../include/memory.h"

void check_local_depends(alpm_handle_t *local, alpm_pkg_t *pkg);
void list_free(char *data);		// for alpm_list_fn_free
alpm_list_t *alpm_local(alpm_handle_t **local, alpm_errno_t *err);
alpm_list_t *alpm_repos(alpm_handle_t **repo);
bool is_foreign(char *pkgname);
bool is_installed(char *pkgname);

// return list of packages in localdb - dont need pu.
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

}

bool is_installed(char *pkgname) {

	alpm_handle_t *local;
	alpm_db_t *local_db;
	alpm_errno_t err;
	alpm_pkg_t *pkg;

	alpm_local(&local, &err);
	local_db = alpm_get_localdb(local);
	pkg = alpm_db_get_pkg(local_db, pkgname);
	
	if (pkg != NULL) {
		alpm_release(local);
		return true;
	} else {
		alpm_release(local);
		return false;
	}

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
void check_local_depends(alpm_handle_t *local, alpm_pkg_t *pkg) {
	
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
			check_local_depends(local, dep_pkg);
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
	bool not_found = false, success = true;
	int res;

	list = alpm_local(&local, &err);
	local_db = alpm_get_localdb(local);

	res = alpm_trans_init(local, ALPM_TRANS_FLAG_CASCADE | ALPM_TRANS_FLAG_NODEPVERSION);
	if (res != 0) {
		printf("Elevated privilage required to perform this operation (root).\n");
		printf("alpm_trans_init: %s\n", alpm_strerror(alpm_errno(local)));
		alpm_release(local);
		clear_list(pkglist);
		exit(EXIT_FAILURE);
	}

	for (temp = pkglist; temp != NULL; temp = temp->next) {
		pkg = alpm_db_get_pkg(local_db, temp->pkgname);
		if (pkg == NULL) {
			printf(BRED"ERROR:"BOLD" %s not found.\n"RESET, temp->pkgname);
			not_found = true;
			continue;
		}
		res = alpm_remove_pkg(local, pkg);
		if (res != 0) {
			printf("alpm_remove_pkg: %s\n", alpm_strerror(alpm_errno(local)));
		}
	}

	if (not_found == true) {
		alpm_trans_release(local);
		alpm_release(local);
		clear_list(pkglist);
		exit(EXIT_FAILURE);
	}

	printf("checking dependencies...\n\n");
	for (temp = pkglist; temp != NULL; temp = temp->next) {
		check_local_depends(local, pkg);
		
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
		printf("alpm_trans_prepare: %s\n", alpm_strerror(alpm_errno(local)));
	}

	res = alpm_trans_commit(local, &error_list);
	if (res != 0) {
		printf("alpm_trans_commit: %s\n", alpm_strerror(alpm_errno(local)));
		success = false;
	}

	if (success == true) {
		printf("success!\n");
	} else {
		printf("uninstall failed.\n");
	}
	
	alpm_trans_release(local);
	alpm_release(local);
}