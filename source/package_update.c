#include <stdio.h>
#include <ctype.h>
#include "../include/buffer.h"
#include "../include/update.h"
#include "../include/install.h"
#include "../include/memory.h"
#include "../include/list.h"

void update(void) {
	
	char c, *temp, *cmd = NULL, pkgname[NAME_LEN], *update_list = NULL, *pkgbuild_full = NULL;
	register int i;
	Buffer pacman_list = NULL, pkgver = NULL, pkgbuild_epoch = NULL, pkgbuild_pkgver = NULL, pkgbuild_pkgrel = NULL;
	List *pkglist, *pkg_node;

	pkglist = struct_malloc();
	mem_alloc(&update_list, VSTR(update_list), sizeof(char)); 	// must malloc here in order to realloc later on with strlen(update_list)
	//mem_alloc(&pkgbuild_full, VSTR(pkgbuild_full), sizeof(char));

	// get pkgname and pkgver of installed packages and store in pkglist
	get_buffer("echo -n $(pacman -Qmq)", &pacman_list);

	temp = pacman_list;
	while (*pacman_list != '\0') {
		for (i = 0; i < NAME_LEN; i++) {
			pkgname[i] = '\0';
		}
		for (i = 0; *pacman_list != ' ' && *pacman_list != '\0'; i++) {
			pkgname[i] = *pacman_list++;	
		}
		if (*pacman_list != '\0') {
			pacman_list++;
		}
		
		// add pkgname and pkgver to pkglist
		mem_alloc(&cmd, VSTR(cmd), (sizeof(char) * (strlen(pkgname) + 47)));
		sprintf(cmd, "echo -n $(pacman -Qm | grep %s | cut -f2 -d ' ')", pkgname);
		get_buffer(cmd, &pkgver);
		pkglist = add_pkg(pkglist, pkgname, pkgver);

		// update pkgname source folder in ~/.aur
		mem_alloc(&cmd, VSTR(cmd), (strlen(pkgname) + 29));
		sprintf(cmd, "cd %s && git pull &> /dev/null", pkgname);
		system(cmd);

		// get epoch for current pkgname from pkgbuild
		mem_alloc(&cmd, VSTR(cmd), (strlen(pkgname) + 85));
		sprintf(cmd, "echo -n $(cd %s && echo $(less PKGBUILD | grep epoch= | cut -f2 -d '=') | tr -d \"'\\\"\")", pkgname);
		get_buffer(cmd, &pkgbuild_epoch);
	
		// get pkgver for current pkgname from pkgbuild
		mem_alloc(&cmd, VSTR(cmd), (strlen(pkgname) + 86));
		sprintf(cmd, "echo -n $(cd %s && echo $(less PKGBUILD | grep pkgver= | cut -f2 -d '=') | tr -d \"'\\\"\")", pkgname);
		get_buffer(cmd, &pkgbuild_pkgver);
	
		// get pkgrel for current pkgname from pkgbuild
		mem_alloc(&cmd, VSTR(cmd), (strlen(pkgname) + 86));
		sprintf(cmd, "echo -n $(cd %s && echo $(less PKGBUILD | grep pkgrel= | cut -f2 -d '=') | tr -d \"'\\\"\")", pkgname);
		get_buffer(cmd, &pkgbuild_pkgrel);
	
		// copy pkgver-pkgrel into pkgbuild_full
		mem_alloc(&pkgbuild_full, VSTR(pkgver), (strlen(pkgbuild_epoch) + strlen(pkgbuild_pkgver) + strlen(pkgbuild_pkgrel)) + 3);
		if (pkgbuild_epoch[0] == '\0') {
			sprintf(pkgbuild_full, "%s-%s", pkgbuild_pkgver, pkgbuild_pkgrel);
		} else if (pkgbuild_pkgrel[0] == '\0') {
			sprintf(pkgbuild_full, "%s:%s", pkgbuild_epoch, pkgbuild_pkgver);
		} else if (pkgbuild_epoch[0] == '\0' && pkgbuild_pkgrel[0] == '\0') {
			sprintf(pkgbuild_full, "%s", pkgbuild_pkgver);
		} else {
			sprintf(pkgbuild_full, "%s:%s-%s", pkgbuild_epoch, pkgbuild_pkgver, pkgbuild_pkgrel);
		}
		
		// retrieve pkgname node from pkglist
		pkg_node = find_pkg(pkglist, pkgname);
		if (strcmp(pkg_node->pkgver, pkgbuild_full) != 0) {
			pkg_node->update = true;
			mem_alloc(&cmd, VSTR(update_list), (strlen(pkgname) + strlen(pkg_node->pkgver) + strlen(pkgbuild_full) + 37));
			sprintf(cmd, "	%-30s%s -> %s\n", pkgname, pkg_node->pkgver, pkgbuild_full);
			mem_alloc(&update_list, VSTR(update_list), (strlen(update_list) + strlen(cmd) + 1));
			strcat(update_list, cmd);
		}
		
	}
	free(pkgver);
	free(pkgbuild_epoch);
	free(pkgbuild_pkgver);
	free(pkgbuild_pkgrel);
	free(pkgbuild_full);
	free(temp);
	free(cmd);

	if (update_list[0] == '\0') {
		printf(" Nothing to do.\n");
		free(update_list);
		clear_list(pkglist);
		exit(EXIT_SUCCESS);
	} else {
		printf(":: Updates are available for:\n\n%s\n", update_list);
		free(update_list);
	}

	printf(":: Proceed with installation? [Y/n] ");
	for(;;) {
		c = tolower(getchar());
		if (c != '\n') {
			while (getchar() != '\n');
		}
		if (c == 'y' || c == '\n') {
			List *temp = pkglist;
			while (pkglist != NULL) {
				if (pkglist->update == true) {
					pkglist->update = false;
					resolve(pkglist->pkgname);
				}
				pkglist = pkglist->next;
			}
			clear_list(temp);
			break;
		} else if (c == 'n') {
			clear_list(pkglist);
			break;
		}
	}
}