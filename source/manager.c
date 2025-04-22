#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/manager.h"
#include "../include/list.h"
#include "../include/util.h"
#include "../include/memory.h"
#include "../include/pkgdata.h"

// prototypes
int trans_init(alpm_handle_t *handle, alpm_transflag_t flags);
int trans_complete(alpm_handle_t *handle);
void rm_depends(alpm_handle_t *handle, alpm_pkg_t *pkg);
void clean_up(Depends *rm_list);
int install_depends(Depends *deps);
int resolve_deps(alpm_handle_t *handle, alpm_list_t *repo_db_list, alpm_pkg_t *pkg);
int rm_makedepends(Depends *deps);

// return list of repos in pacman.conf
alpm_list_t *handle_init(alpm_handle_t **handle) {

	pu_config_t *conf;
	alpm_list_t *repo_list;

	conf = pu_config_new();
    pu_ui_config_load(conf, "/etc/pacman.conf");
    *handle = pu_initialize_handle_from_config(conf);
	if (*handle == NULL) {
		printf("pu_initialize_buffer_from_config failed.\n");
	}
    repo_list = pu_register_syncdbs(*handle, conf->repos);

    pu_config_free(conf);

	return repo_list;
}

int trans_init(alpm_handle_t *handle, alpm_transflag_t flags) {
	
	int res;

	gain_root();
	res = alpm_trans_init(handle, flags);
	if (res != 0) {
		printf(BRED"error:"RESET" alpm_trans_init: %s\n", alpm_strerror(alpm_errno(handle)));
	}
	drop_root();

	if (res != 0) {
		return -1;
	}

	return 0;
}

int trans_complete(alpm_handle_t *handle) {

	int res;
	alpm_list_t *error_list;

	gain_root();
	res = alpm_trans_prepare(handle, &error_list);
	if (res != 0) {
		printf(BRED"error:"RESET" alpm_trans_prepare: %s\n", alpm_strerror(alpm_errno(handle)));
	}
	res = alpm_trans_commit(handle, &error_list);
	if (res != 0) {
		printf(BRED"error:"RESET" alpm_trans_commit: %s\n", alpm_strerror(alpm_errno(handle)));
		return -1;
	}
	alpm_trans_release(handle);	
	drop_root();
	alpm_release(handle);

	if (res != 0) {
		return -1;
	}

	return 0;
}

int trans_abort(alpm_handle_t *handle, int code) {
	
	gain_root();
	alpm_trans_release(handle);
	drop_root();
	alpm_release(handle);

	return code;
}

/*	
 * 	check if any packages besides the one being removed requires one of its deps
 * 	before removing the dep, if required the dep is still needed return true.
 */
int alpm_install(List *pkglist, alpm_pkgreason_t reason) {

	alpm_handle_t *handle;
	alpm_list_t *add_list;
	alpm_pkg_t *pkg;
	Pkgbase *pkgbase;
	Pkginfo *pkginfo;

	int res;

	handle_init(&handle);
	// depends and makedepends must be installed before package.
	for (;pkglist != NULL; pkglist = pkglist->next) {
		// ignore packages user rejected
		if (pkglist->install == false) {
			continue;
		} 
		if (is_installed(pkglist->pkgname) == true && pkglist->update == false) {
			printf(BCYAN"info:"RESET" %s is already installed.\n"RESET, pkglist->pkgname);
			continue;
		}
		
		pkgbase = populate_pkg(pkglist->pkgname);
		pkginfo = pkgbase->pkg;

		if (pkgbase == NULL || pkginfo == NULL) {
			printf(BYELLOW"warn"RESET" Skipping %s...", pkglist->pkgname);
			continue;
		}
		
		// check for aur dependencies here

		// init deps tx then traverse pkginfo here?
		printf(BGREEN"==>"BOLD" Checking build dependencies...\n\n"RESET);
		res = install_depends(pkgbase->makedepends);
		if (res != 0) {
			printf(BRED"error:"RESET" failed to install build dependencies\n");
			break;
		}
		
		printf(BGREEN"==>"BOLD" Checking dependencies...\n\n"RESET);
		res = install_depends(pkginfo->depends);
		if (res != 0) {
			printf(BRED"error:"RESET" failed to install dependencies\n");
			break;
		}

		change_dir(pkgbase->pkgbase);
		gain_root();
		res = build();
		if (res != 0) {
			printf(BRED"error:"RESET" failed to build package: %s\n", pkgbase->pkgbase);
			drop_root();
			break;
		}
		drop_root();
	
		// remove makedepends
		res = rm_makedepends(pkgbase->makedepends);
		if (res != 0) {
			printf(BGREEN"==>"BOLD"Keeping make dependencies.\n"RESET);
		}

		res = trans_init(handle, ALPM_TRANS_FLAG_NODEPVERSION | reason);
		if (res != 0) {
			break;
		}
		
		res = alpm_pkg_load(handle, pkginfo->zst_path, 1, 0, &pkg);
		if (res != 0) {
			printf(BRED"error:"RESET" failed to add local package: %s.\n", alpm_strerror(alpm_errno(handle)));
		}
		res = alpm_add_pkg(handle, pkg);
		if (res != 0) {
			printf(BRED"error:"RESET" failed to add package.\n");
		}		
		clear_pkgbase(pkgbase);

		// print package list
		add_list = alpm_trans_get_add(handle);
		printf(BOLD"\nPackages (%ld): "RESET, alpm_list_count(add_list));
		while (add_list != NULL) {
			printf("%s"GREY"-%s  "RESET, alpm_pkg_get_name(add_list->data), alpm_pkg_get_version(add_list->data));
			add_list = alpm_list_next(add_list);
		}

		printf(BBLUE"\n\n::"BOLD" Proceed with installation? [Y/n] "RESET);	
		if (prompt() == false) {
			return trans_abort(handle, 0);
		}

		res = trans_complete(handle);
		if (res != 0) {
			break;
		}
	}	

	if (pkglist != NULL) {
		clear_pkgbase(pkgbase);
		return -1;
	}
	return 0;
}

int alpm_uninstall(List *pkglist) {

	alpm_handle_t *handle;
	alpm_db_t *local_db;
	alpm_pkg_t *pkg;
	alpm_list_t *local_list;
	Depends *post_rm = NULL;
	const char *pkgname;
	int res;

	handle_init(&handle);

	local_db = alpm_get_localdb(handle);
	res = trans_init(handle, ALPM_TRANS_FLAG_CASCADE | ALPM_TRANS_FLAG_NODEPVERSION);

	printf("Checking dependencies...\n\n");
	for (; pkglist != NULL && res == 0; pkglist = pkglist->next) {
		if (is_installed(pkglist->pkgname) == false) {
			printf(BRED"error:"RESET" target not found: %s.\n", pkglist->pkgname);
			break;
		} else if (is_foreign(pkglist->pkgname) == false) {
			printf(BRED"error:"RESET" %s is not an AUR package.\n", pkglist->pkgname);
			break;
		}
		
		pkg = alpm_db_get_pkg(local_db, pkglist->pkgname);
		if (pkg == NULL) {
			printf(BRED"error:"RESET" target not found in local_db: %s.\n", pkglist->pkgname);
			break;
		}
		
		rm_depends(handle, pkg);
		res = alpm_remove_pkg(handle, pkg);
		if (res != 0) {
			printf(BRED"error:"RESET" alpm_remove_pkg: %s\n", alpm_strerror(alpm_errno(handle)));
		}
	}

	if (pkglist != NULL) {
		return trans_abort(handle, -1);
	}
	
	local_list = alpm_trans_get_remove(handle);
	printf(BOLD"Packages (%ld): "RESET, alpm_list_count(local_list));
	for (; local_list != NULL; local_list = alpm_list_next(local_list)) {
		pkgname = alpm_pkg_get_name(local_list->data);
		printf("%s"GREY"-%s  "RESET, pkgname, alpm_pkg_get_version(local_list->data));
		post_rm = add_data(post_rm, pkgname);
	}

	printf("\n\n"BBLUE"::"BOLD" Do you want to remove these packages? [Y/n] "RESET);
	if (prompt() == false) {
		return trans_abort(handle, 0);
	}

	res = trans_complete(handle);
	if (res != 0) {
		return -1;
	}

	clean_up(post_rm);
	printf(BGREEN"=>"BOLD" Success\n"RESET);
	return 0;
}

int install_depends(Depends *deps) {

	alpm_handle_t *handle;
	alpm_list_t *repo_db_list, *db_temp, *add_list, *reset, *db_pkglist;
	alpm_pkg_t *pkg;
	Depends *temp_deps;
	bool missing_dep = false;
	int res;

	if (deps == NULL) {
		return 0;
	}

	repo_db_list = handle_init(&handle);
	res = trans_init(handle, ALPM_TRANS_FLAG_ALLDEPS);

	// pass deps one by one to resolve_deps
	for (temp_deps = deps; temp_deps != NULL && res == 0; temp_deps = temp_deps->next) {
		for (db_temp = repo_db_list; db_temp != NULL; db_temp = alpm_list_next(db_temp)) {
			pkg = alpm_db_get_pkg(db_temp->data, temp_deps->data);
			if (pkg == NULL) {
				reset = db_temp;
				for (db_temp = repo_db_list; db_temp != NULL; db_temp = alpm_list_next(db_temp)) {
					db_pkglist = alpm_db_get_pkgcache(db_temp->data);
					pkg = alpm_find_satisfier(db_pkglist, temp_deps->data);
					if (pkg != NULL) {
						break;
					}
				}
				db_temp = reset;
			}
			if (pkg != NULL && is_installed(temp_deps->data) == false) {
				missing_dep = true;
				res = alpm_add_pkg(handle, pkg);
				if (res != 0) {
					printf(BRED"error:"RESET" alpm_add_pkg (install): %s\n", alpm_strerror(alpm_errno(handle)));
					break;
				}
				res = resolve_deps(handle, repo_db_list, pkg);
				if (res != 0) {
					printf(BRED"error:"RESET" unable to resolve dependencies.\n");
					break;
				}
			}
		}
	} 

	if (temp_deps != NULL) {
		return trans_abort(handle, -1);
	}

	if (missing_dep == false) {
		return trans_abort(handle, 0);
	}

	// print package list
	add_list = alpm_trans_get_add(handle);
	printf(BOLD"\nPackages (%ld): "RESET, alpm_list_count(add_list));
	while (add_list != NULL) {
		printf("%s"GREY"-%s  "RESET, alpm_pkg_get_name(add_list->data), alpm_pkg_get_version(add_list->data));
		add_list = alpm_list_next(add_list);
	}

	// dont gain root before prompt.
	printf(BBLUE"\n\n::"BOLD" Proceed with installation? [Y/n] "RESET);
	if (prompt() == false) {
		return trans_abort(handle, 0);
	}
	
	res = trans_complete(handle);
	if (res != 0) {
		return -1;
	}

	printf(BGREEN"==>"BOLD" Deps resolved...\n"RESET);
	return 0;
}

/*
*	add dependencies to transaction
*/
int resolve_deps(alpm_handle_t *handle, alpm_list_t *repo_db_list, alpm_pkg_t *pkg) {

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
				res = alpm_add_pkg(handle, dep_pkg);
				if (res != 0) {
					printf(BRED"error: "RESET"alpm_add_pkg: %s\n", alpm_strerror(alpm_errno(handle)));
					return -1;
				}

				alpm_pkg_set_reason(dep_pkg, ALPM_PKG_REASON_DEPEND);
				resolve_deps(handle, repo_db_list, dep_pkg);
			}
		}
	}
	
	return 0;
}

int rm_makedepends(Depends *deps) {
	
	alpm_handle_t *handle;
	alpm_db_t *local_db;
	alpm_list_t *list, *req_list, *pkglist;
	alpm_pkg_t *pkg;
	int res, count;

	if (deps == NULL) {
		return 0;
	}
	
	handle_init(&handle);
	local_db = alpm_get_localdb(handle);
	res = trans_init(handle, ALPM_TRANS_FLAG_NODEPVERSION);
	
	for (; deps != NULL && res == 0; deps = deps->next) {
		pkg = alpm_db_get_pkg(local_db, deps->data);
		if (pkg == NULL) {
			pkglist = alpm_db_get_pkgcache(local_db);
			pkg = alpm_find_satisfier(pkglist, deps->data);
		}

		req_list = alpm_pkg_compute_requiredby(pkg);
		if (alpm_pkg_get_reason(pkg) == ALPM_PKG_REASON_DEPEND && alpm_list_count(req_list) == 0) {
			rm_depends(handle, pkg);
			res = alpm_remove_pkg(handle, pkg);
			if (res != 0) {
				printf(BYELLOW"warn:"RESET" unable to remove makedepend: %s\n", alpm_pkg_get_name(pkg));
				res = 0;
			}
		}
		alpm_list_free_inner(req_list, (alpm_list_fn_free) list_free);
		alpm_list_free(req_list);
	}

	list = alpm_trans_get_remove(handle);
	count = alpm_list_count(list);
	if (count == 0) {
		return trans_abort(handle, 0);
	}
	printf(BOLD"Packages (%d): "RESET, count);
	for (; list != NULL; list = alpm_list_next(list)) {
		printf("%s"GREY"-%s  "RESET, alpm_pkg_get_name(list->data), alpm_pkg_get_version(list->data));
	}

	printf("\n\n"BBLUE"::"BOLD" Do you want to remove these packages? [Y/n] "RESET);
	if (prompt() == false) {
		return trans_abort(handle, 0);
	}

	res = trans_complete(handle);
	if (res != 0) {
		return -1;
	}

	printf(BGREEN"=>"BOLD" Makedepends removed...\n"RESET);
	return 0;
}

void rm_depends(alpm_handle_t *handle, alpm_pkg_t *pkg) {
	
	alpm_db_t *local_db;
	alpm_pkg_t *dep_pkg;
	alpm_depend_t *dep;
	alpm_list_t *dep_list, *req_list, *local_db_list;
	int res;

	local_db = alpm_get_localdb(handle);
	
	// search localdb for pkgname and use it to get a list of
	// depenencies for package and traverse the list.
	dep_list = alpm_pkg_get_depends(pkg);
	for (; dep_list != NULL; dep_list = alpm_list_next(dep_list)) {
		dep = dep_list->data;
		// find the dependency in the localdb
		dep_pkg = alpm_db_get_pkg(local_db, dep->name);
		if (dep_pkg == NULL) {
			local_db_list = alpm_db_get_pkgcache(local_db);
			dep_pkg = alpm_find_satisfier(local_db_list, dep->name);
		}

		// get a list of packages that requires the dependency
		req_list = alpm_pkg_compute_requiredby(dep_pkg);
		// check if theres only one package in the required list
		// if this is true, there are no other packages that requires it.
		if (alpm_list_count(req_list) == 1 && strcmp(req_list->data, alpm_pkg_get_name(pkg)) == 0) {
			if (alpm_pkg_get_reason(dep_pkg) == ALPM_PKG_REASON_DEPEND) {
				res = alpm_remove_pkg(handle, dep_pkg);
				if (res != 0) {
					printf(BYELLOW"warn:"RESET" unable to remove package: %s\n", alpm_pkg_get_name(dep_pkg));
				}
				rm_depends(handle, dep_pkg);
			}
		}

		alpm_list_free_inner(req_list, (alpm_list_fn_free) list_free);
		alpm_list_free(req_list);
	}
}

// cleaning up system files not needed, pivot to cleaning config and cache dirs. 
void clean_up(Depends *post_rm) {

	const char *pkgname;
	char config[MAX_BUFFER], cache[MAX_BUFFER];

	change_dir("HOME");
	
	for (; post_rm != NULL; post_rm = post_rm->next) {
		strcpy(config, ".config/");
		strcpy(cache, ".cache/");

		pkgname = post_rm->data;
		strcat(config, pkgname);
		if (is_dir(config) == true) {
			remove_dir(config);
		}
		strcat(cache, pkgname);
		if (is_dir(cache) == true) {
			remove_dir(cache);
		}
	}
	
	change_dir("WD");
}