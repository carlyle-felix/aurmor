#include <stdio.h>
#include <stdlib.h>

#include "../include/pkgdata.h"
#include "../include/memory.h"
#include "../include/manager.h"
#include "../include/util.h"
#include "../include/list.h"

Pkgbase *populate_pkgbase(char *buffer);
Pkginfo *populate_pkginfo(char *buffer);
Pkginfo *add_name(Pkginfo *pkg, const char *str);
Pkginfo *find_name(Pkginfo *pkg, const char *pkgname);

/*
*	populate package structs.
*/
Pkgbase *populate_pkg(char *pkgname) {
	
	Pkgbase *pkgbase;
	char *buffer = read_srcinfo(pkgname);

	pkgbase = populate_pkgbase(buffer);
	pkgbase->pkg = populate_pkginfo(buffer);

	free(buffer);
	return pkgbase;
}

/*
*	BASE
*	pkgbase->pkgbase = NULL;
*	pkgbase->arch = NULL;
*	pkgbase->epoch = NULL;
*	pkgbase->pkgver = NULL;
*	pkgbase->pkgrel = NULL;
*	pkgbase->makedepends = NULL;
*	pkgbase->pkg = NULL;
*/



Pkgbase *populate_pkgbase(char *buffer) {

	Pkgbase *pkgbase = pkgbase_malloc();
	const char *key[] = {"pkgbase", "arch", "epoch", "pkgver", "pkgrel", "makedepends"};
	char *temp_buffer, str[MAX_BUFFER];
	register int i, key_num; 
	int key_len;

	for (key_num = 0; key_num < 6; key_num++) {
		temp_buffer = buffer;
		key_len = strlen(key[key_num]);
		while (*temp_buffer != '\0') {
			while (*temp_buffer != key[key_num][0] && *temp_buffer != '\0') {
				temp_buffer++;
			}
			
			if (*temp_buffer == key[key_num][0]) {
				for (i = 0; *temp_buffer == key[key_num][i]; i++) {
					if (*temp_buffer != key[key_num][i]) {
						break;
					}
					temp_buffer++;
				}
			}

			if (i == key_len) {
				while ((*temp_buffer == ' ' || *temp_buffer == '=') && *temp_buffer != '\0') {
					temp_buffer++;
				}
				for (i = 0; *temp_buffer != '>' && *temp_buffer != '<' && *temp_buffer != '=' && *temp_buffer != '\n' && *temp_buffer != '\0'; i++) {
					str[i] = *temp_buffer++;
				}
				str[i] = '\0';
				switch (key_num) {
					case 0:		str_alloc(&pkgbase->pkgbase, strlen(str) + 1);
								strcpy(pkgbase->pkgbase, str);
								break;
					case 1:		str_alloc(&pkgbase->arch, strlen(str) + 1);
								strcpy(pkgbase->arch, str);
								break;
					case 2:		str_alloc(&pkgbase->epoch, strlen(str) + 1);
								strcpy(pkgbase->epoch, str);
								break;
					case 3:		str_alloc(&pkgbase->pkgver, strlen(str) + 1);
								strcpy(pkgbase->pkgver, str);
								break;
					case 4:		str_alloc(&pkgbase->pkgrel, strlen(str) + 1);
								strcpy(pkgbase->pkgrel, str);
								break;
					case 5:		pkgbase->makedepends = add_data(pkgbase->makedepends, str);
								break;
					default:	break;
				}
			}
		}
	}

	return pkgbase;
}

/*
*	PACKAGE
*	pkg->pkgname = NULL;
*	pkg->arch = NULL;
*	pkg->zst_path = NULL;
*	pkg->depends = NULL;
*	pkg->optdepends = NULL;
*	pkg->next = NULL;
*/
Pkginfo *populate_pkginfo(char *buffer) {

	Pkginfo *pkginfo, *temp_pkg = NULL;
	const char *key[] = {"pkgname", "arch", "depends", "optdepends"};
	char *temp_buffer, *split_buffer = NULL, str[MAX_BUFFER];
	register int i, key_num; 
	int key_len;

	// remove everything before the first occurrence of "pkgname" from the buffer.
	temp_buffer = buffer;
	key_len = strlen(key[0]);
	for (;;) {
		while (*temp_buffer != key[0][0]) {
			temp_buffer++;
		}

		if (*temp_buffer == key[0][0]) {
			for (i = 0; *temp_buffer == key[0][i]; i++) {
				if (*temp_buffer != key[0][i]) {
					break;
				}
				temp_buffer++;
			}
		}

		if (i == key_len) {
			while (i > 0) {
				i--;
				temp_buffer--;
			}
			break;
		}
	}
	str_alloc(&split_buffer, strlen(temp_buffer) + 1);
	strcpy(split_buffer, temp_buffer);

	for (key_num = 0; key_num < 4; key_num++) {
		temp_buffer = split_buffer;
		key_len = strlen(key[key_num]);
		while (*temp_buffer != '\0') {
		
			while (*temp_buffer != key[key_num][0] && *temp_buffer != '\0') {
				temp_buffer++;
				// after pkgnames are added to the list, check which package should be assigned to temp_pkg 
				if (key_num > 0 && *temp_buffer == key[0][0]) {
					for (i = 0; *temp_buffer == key[0][i]; i++) {
						if (*temp_buffer != key[0][i]) {
							while (i > 0) {
								i--;
								temp_buffer--;
							}
							break;
						}
						temp_buffer++;
					}

					if (i == 7) {
						while ((*temp_buffer == ' ' || *temp_buffer == '=') && *temp_buffer != '\0') {
							temp_buffer++;
						}
						for (i = 0; *temp_buffer != '>' && *temp_buffer != '<' && *temp_buffer != '=' && *temp_buffer != '\n' && *temp_buffer != '\0'; i++) {
							str[i] = *temp_buffer++;
						}
						str[i] = '\0';
						
						temp_pkg = find_name(pkginfo, str);
						printf("temp_pkg->pkgname: %s		str: %s\n\n", temp_pkg->pkgname, str);
					}
				}
			}
	
			if (*temp_buffer == key[key_num][0]) {
				for (i = 0; *temp_buffer == key[key_num][i]; i++) {
					if (*temp_buffer != key[key_num][i]) {
						break;
					}
					temp_buffer++;
				}
			}

			if (i == key_len) {
				while ((*temp_buffer == ' ' || *temp_buffer == '=') && *temp_buffer != '\0') {
					temp_buffer++;
				}
				for (i = 0; *temp_buffer != '>' && *temp_buffer != '<' && *temp_buffer != '=' && *temp_buffer != '\n' && *temp_buffer != '\0'; i++) {
					str[i] = *temp_buffer++;
				}
				str[i] = '\0';

				switch(key_num) {
					case 0:		temp_pkg = add_name(temp_pkg, str);
								break;
					case 1:		str_alloc(&temp_pkg->arch, strlen(str) + 1);
								strcpy(temp_pkg->arch, str);
								break;
					case 2:		temp_pkg->depends = add_data(temp_pkg->depends, str);
								break;
					case 3:		temp_pkg->optdepends = add_data(temp_pkg->optdepends, str);
								break;
					default:	break;
				}
			}
		}
		if (key_num == 0) {
			pkginfo = temp_pkg;
		}
	}
	free(split_buffer);

	return pkginfo;
}

char *read_srcinfo(char *pkgname) {

	FILE *srcinfo; 
	char *buffer = NULL;
	int read = 0, max = MAX_BUFFER;
	
	change_dir(pkgname);

	for (;;) {
		str_alloc(&buffer, max);
		srcinfo = fopen(".SRCINFO", "r");
		if (srcinfo == NULL) {
			printf(BRED"error:"RESET" failed to open %s/.SRCINFO\n", pkgname);
			free(buffer);
			return NULL;
		}
		read = fread(buffer, sizeof(char), max, srcinfo);
		if (read == max) {
			max = read * 2;
		} else {
			fclose(srcinfo);
			break;
		}
		fclose(srcinfo);
	}
	buffer[read] = '\0';
	if (buffer[0] == '\0') {
		return NULL;
	}
	
	return buffer;
}

/*
*	return the absolute path of package zst - FIX LATER
*/
/*
char *zst_path(Pkginfo *pkg) {
	
	char *cwd, *path = NULL;

	cwd = change_dir(pkg->pkgname);

	if (pkg->epoch == NULL) {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->pkgver) + strlen(pkg->pkgrel) + strlen(pkg->arch) + 18);
		sprintf(path, "%s/%s-%s-%s-%s.pkg.tar.zst", cwd, pkg->pkgname, pkg->pkgver, pkg->pkgrel, pkg->arch);
	} else if (pkg->pkgrel == NULL) {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->epoch) + strlen(pkg->pkgver) + strlen(pkg->arch) + 18);
		sprintf(path, "%s/%s-%s:%s-%s.pkg.tar.zst", cwd, pkg->pkgname, pkg->epoch, pkg->pkgver, pkg->arch);
	} else if (pkg->epoch == NULL && pkg->pkgrel == NULL) {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->pkgver) + strlen(pkg->arch) + 17);
		sprintf(path, "%s/%s-%s-%s.pkg.tar.zst", cwd, pkg->pkgname, pkg->pkgver, pkg->arch);
	} else {
		str_alloc(&path, strlen(cwd) + strlen(pkg->pkgname) + strlen(pkg->epoch) + strlen(pkg->pkgver) + strlen(pkg->pkgrel) + strlen(pkg->arch) + 19);
		sprintf(path, "%s/%s-%s:%s-%s-%s.pkg.tar.zst", cwd, pkg->pkgname, pkg->epoch, pkg->pkgver, pkg->pkgrel, pkg->arch);
	}

	return path;
}
*/

// please do something about add_pkgname in List.c., can this list be used there too?
Pkginfo *find_name(Pkginfo *pkg, const char *pkgname) {

	Pkginfo *temp = pkg;
	while (temp != NULL) {
		if (strcmp(temp->pkgname, pkgname) == 0) {
			break;
		}
		temp = temp->next;
	}

	return temp;
}

// please do something about add_pkgname in List.c.
Pkginfo *add_name(Pkginfo *pkg, const char *str) {

	Pkginfo *temp = pkginfo_malloc();

	str_alloc(&temp->pkgname, strlen(str) + 1);
	strcpy(temp->pkgname, str);
	temp->next = pkg;
	pkg = temp;

	return temp;
}

Depends *add_data(Depends *list, const char *data) {
	
	Depends *temp = depends_malloc();

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

	List *pkglist, *temp_list;
	bool foreign = false;
	
	pkglist = foreign_list();
	for (temp_list = pkglist; temp_list != NULL; temp_list = temp_list->next) {
		if (strcmp(temp_list->pkgname, pkgname) == 0) {
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