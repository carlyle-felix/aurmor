#include <stdio.h>
#include <stdlib.h>

#include "../include/pkgdata.h"
#include "../include/memory.h"
#include "../include/manager.h"
#include "../include/util.h"
#include "../include/list.h"

/*
*	populate package struct.
*/
Srcinfo *populate_pkg(char *pkgname) {

	Srcinfo *pkg = pkg_srcinfo_malloc();
	int key_len;
	register int i, key;
	char *buffer, *temp_buffer, key_item[MAX_BUFFER];
	char *keys[] = {"pkgname", "epoch", "pkgver", "pkgrel", 
					"makedepends", "depends", "optdepends"};

	buffer = read_srcinfo(pkgname);

	// traverse list of keys
	for (key = 0; key < 7; key++) {
		key_len = strlen(keys[key]);
		for (temp_buffer = buffer; *temp_buffer != '\0'; temp_buffer++) {
			// advance past the tab.
			if (*temp_buffer == '\t') {
				temp_buffer++;
			}

			// check the first letter, if no match, advance to newline char.
			if (*temp_buffer != keys[key][0]) {
				while (*temp_buffer != '\n') {
					temp_buffer++;
				}
				continue;
			}

			for (i = 0; i < key_len; i++) {
				if (*temp_buffer++ != keys[key][i]) {
					// if no match, skip line, advance to newline char.
					while (*temp_buffer != '\n') {
						temp_buffer++;
					}
					i = 0;
					break;
				}
			}
			
			if (i == (key_len)) {
				key_item[0] = '\0';
				while (*temp_buffer == ' ' || *temp_buffer == '=') {
					temp_buffer++;
				}
				for (i = 0; *temp_buffer != '\n' && *temp_buffer != '>'; i++) {
					key_item[i] = *temp_buffer++;
				}
				key_item[i] = '\0';
				if (*temp_buffer == '>') {
					while (*temp_buffer != '\n') {
						temp_buffer++;
					}
				}
				if (key_item[0] == '\0') {
					continue;
				}
				// put data in correct fields according to key.
				switch (key) {
					case 0:		
						str_alloc(&pkg->pkgname, strlen(key_item) + 1);
						strcpy(pkg->pkgname, key_item);
						break;
					case 1:
						str_alloc(&pkg->epoch, strlen(key_item) + 1);
						strcpy(pkg->epoch, key_item);
						break;
					case 2:
						str_alloc(&pkg->pkgver, strlen(key_item) + 1);
						strcpy(pkg->pkgver, key_item);
						break;
					case 3: 
						str_alloc(&pkg->pkgrel, strlen(key_item) + 1);
						strcpy(pkg->pkgrel, key_item);
						break;
					case 4:
						pkg->makedepends = add_data(pkg->makedepends, key_item);
						break;
					case 5: 
						pkg->depends = add_data(pkg->depends, key_item);
						break;
					case 6:
						pkg->optdepends = add_data(pkg->optdepends, key_item);
						break;
					default:
						break;
				}
			}
		}
	}
	pkg->zst_path = zst_path(pkg);

	change_dir("WD");
	free(buffer);

	return pkg;
}

/*
 * returns a list of keys found.
 * pkgname: as found in .SRCINFO.
 * key: .SRCINFO field to search for.
 * TODO: deal with version requirements.
 */
char *read_srcinfo(char *pkgname) {

	FILE *srcinfo; 
	char *buffer = NULL;
	int read = 0, max = MAX_BUFFER;
	
	change_dir(pkgname);

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

	return buffer;
}

/*
*	return the absolute path of package zst 
*/
char *zst_path(Srcinfo *pkg) {
	
	char *cwd, *path = NULL;

	cwd = change_dir(pkg->pkgname);

	if (pkg->epoch == NULL) {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->pkgver) + strlen(pkg->pkgrel) + 24);
		sprintf(path, "%s/%s-%s-%s-x86_64.pkg.tar.zst", cwd, pkg->pkgname, pkg->pkgver, pkg->pkgrel);
	} else if (pkg->pkgrel == NULL) {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->epoch) + strlen(pkg->pkgver) + 24);
		sprintf(path, "%s/%s-%s:%s-x86_64.pkg.tar.zst", cwd, pkg->pkgname, pkg->epoch, pkg->pkgver);
	} else if (pkg->epoch == NULL && pkg->pkgrel == NULL) {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->pkgver) + 23);
		sprintf(path, "%s/%s-%s-x86_64.pkg.tar.zst", cwd, pkg->pkgname, pkg->pkgver);
	} else {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->epoch) + strlen(pkg->pkgver) + strlen(pkg->pkgrel) + 25);
		sprintf(path, "%s/%s-%s:%s-%s-x86_64.pkg.tar.zst", cwd, pkg->pkgname, pkg->epoch, pkg->pkgver, pkg->pkgrel);
	}

	return path;
}

Depends *add_data(Depends *list, const char *data) {
	
	Depends *temp;

	temp = depends_malloc();
	str_alloc(&temp->data, strlen(data) + 1);
	strcpy(temp->data, data);
	temp->next = list;
	list = temp;

	return list;
}
/*
*	returns a list of foreign packages that are installed.
*/
List *foreign_list(void) {
    
    alpm_handle_t *handle = NULL;
	alpm_db_t *local_db;
    alpm_list_t *local_list, *repo_list, *reset;
    alpm_pkg_t *pkg;
    List *aur;
   
	repo_list = handle_init(&handle);	
	local_db = alpm_get_localdb(handle);
	local_list = alpm_db_get_pkgcache(local_db);
    
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

    alpm_release(handle);

    return aur;
}
/*
*	returns bool indicating whether or an installed package is foreign.
*/
bool is_foreign(char *pkgname) {

	List *pkglist, *temp;
	bool foreign = false;
	
	pkglist = foreign_list();
	for (temp = pkglist; temp != NULL; temp = temp->next) {
		if (strcmp(temp->pkgname, pkgname) == 0) {
			foreign = true;
			break;
		}
	}
	clear_list(pkglist);
	return foreign;
}

bool is_installed(char *pkgname) {

	alpm_handle_t *handle;
	alpm_db_t *local_db;
	alpm_pkg_t *pkg;
	bool installed = true;

	handle_init(&handle);
	local_db = alpm_get_localdb(handle);
	pkg = alpm_db_get_pkg(local_db, pkgname);
	if (pkg == NULL) {
		installed = false;
	}
	alpm_release(handle);

	return installed;
}