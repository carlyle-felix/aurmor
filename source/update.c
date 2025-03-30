#include <stdio.h>
#include <ctype.h>
#include "../include/buffer.h"
#include "../include/update.h"
#include "../include/install.h"
#include "../include/memory.h"
#include "../include/list.h"

#define PKGBUILD_CMD(item) "echo -n $(cd %s && echo $(less PKGBUILD | grep " #item "= | cut -f2 -d '=') | tr -d \"'\\\"\")"

void update(void) {
	
	char c, *cmd = NULL, *update_list = NULL, *full_ver = NULL;
	register int i;
	Buffer epoch = NULL, pkgver = NULL, pkgrel = NULL;  // All from source folder PKGBUILD
	List *pkglist, *temp, *test;

	mem_alloc(&update_list, sizeof(char)); 	// must malloc here in order to realloc later on with strlen(update_list)
	
	get_list(&pkglist, "echo -n $(pacman -Qmq)");
	add_pkgver(pkglist);
	for (temp = pkglist; pkglist != NULL; pkglist = pkglist->next) {
	
		// update pkgname source folder in ~/.aur
		get_cmd(&cmd, "cd %s && git pull &> /dev/null", pkglist->pkgname);
		system(cmd);

		// get epoch for current pkgname from pkgbuild (this only works if there was previously an epoch number, it probably fails if there was no epoch number previously and one is added.)
		get_cmd(&cmd, PKGBUILD_CMD(epoch), pkglist->pkgname);
		retrieve(cmd, &epoch);

		// get pkgver for current pkgname from pkgbuild
		get_cmd(&cmd, PKGBUILD_CMD(pkgver), pkglist->pkgname);
		retrieve(cmd, &pkgver);

		// get pkgrel for current pkgname from pkgbuild
		get_cmd(&cmd, PKGBUILD_CMD(pkgrel), pkglist->pkgname);
		retrieve(cmd, &pkgrel);

		// copy pkgver-pkgrel into full_ver
		mem_alloc(&full_ver, (strlen(epoch) + strlen(pkgver) + strlen(pkgrel)) + 3);
		if (epoch[0] == '\0') {
			sprintf(full_ver, "%s-%s", pkgver, pkgrel);
		} else if (pkgrel[0] == '\0') {
			sprintf(full_ver, "%s:%s", epoch, pkgver);
		} else if (epoch[0] == '\0' && pkgrel[0] == '\0') {
			sprintf(full_ver, "%s", pkgver);
		} else {
			sprintf(full_ver, "%s:%s-%s", epoch, pkgver, pkgrel);
		}
		
		if (strcmp(pkglist->pkgver, full_ver) != 0) {
			pkglist->update = true;
			mem_alloc(&cmd, (strlen(pkglist->pkgname) + strlen(pkglist->pkgver) + strlen(full_ver) + 37));
			sprintf(cmd, "	%-30s%s -> %s\n", pkglist->pkgname, pkglist->pkgver, full_ver);
			mem_alloc(&update_list, (strlen(update_list) + strlen(cmd) + 1));
			strcat(update_list, cmd);
		}
	}
	free(epoch);
	free(pkgver);
	free(pkgrel);
	free(full_ver); 
	free(cmd);

	pkglist = temp;
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
