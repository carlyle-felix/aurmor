#include <stdio.h>
#include <ctype.h>
#include "../include/buffer.h"
#include "../include/update.h"
#include "../include/install.h"
#include "../include/memory.h"
#include "../include/list.h"
#include "../include/rpc.h"

#define PKGBUILD_CMD(item) "echo -n $(cd %s && echo $(less PKGBUILD | grep " \
							#item "= | cut -f2 -d '=') | tr -d \"'\\\"\")"

bool epoch_update(List *pkg, char *pkgver);
char *not_on_aur(char *pkgname);

void update(void) {
	
	char c, *cmd = NULL, *update_list = NULL, *pkgver = NULL;
	register int i;
	List *pkglist, *rpc_pkg, *temp;

	str_malloc(&update_list, sizeof(char)); 	// must malloc here in order to realloc later on with strlen(update_list)

	get_list(&pkglist, "echo -n $(pacman -Qmq)");
	add_pkgver(pkglist);

	for (temp = pkglist; pkglist != NULL; pkglist = pkglist->next) {
	
		get_str(&cmd, "https://aur.archlinux.org/rpc/v5/info?arg[]=%s" , pkglist->pkgname);
		rpc_pkg = get_rpc_data(cmd);
		if (rpc_pkg->pkgname != NULL) {
			pkgver = rpc_pkg->pkgver;
		} else {
			pkgver = not_on_aur(pkglist->pkgname);
		}
		
		if (strcmp(pkglist->pkgver, pkgver) < 0 || epoch_update(pkglist, pkgver)) {  
			pkglist->update = true;
			str_malloc(&cmd, (strlen(pkglist->pkgname) + strlen(pkglist->pkgver) + strlen(pkgver) + 40));
			sprintf(cmd, "\t%-30s%-20s->\t%s\n", pkglist->pkgname, pkglist->pkgver, pkgver);
			str_malloc(&update_list, (strlen(update_list) + strlen(cmd) + 1));
			strcat(update_list, cmd);
		}

		if (rpc_pkg->pkgname == NULL) {
			free(pkgver);
		}
		clear_list(rpc_pkg);
	}
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
					less_prompt(pkglist->pkgname);
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

bool epoch_update(List *pkg, char *pkgver) {

	char *ins_pkgver, *upd_pkgver;
	
	ins_pkgver = pkg->pkgver;
	upd_pkgver = pkgver;

	while (*ins_pkgver != ':' && *ins_pkgver != '\0') {
		ins_pkgver++;
	}
	while (*upd_pkgver != ':' && *upd_pkgver != '\0') {
		upd_pkgver++;
	}

	if (*ins_pkgver == '\0' && *upd_pkgver == ':') {
		return true;
	} else {
		return false;
	}
}

char *not_on_aur(char *pkgname) {

	char *cmd = NULL, *full_ver = NULL;
	Buffer epoch = NULL, pkgver = NULL, pkgrel = NULL;

	// update pkgname source folder in ~/.config/aurmor
	get_str(&cmd, "cd %s && git pull &> /dev/null", pkgname);
	system(cmd);

	// get epoch for current pkgname from pkgbuild
	get_str(&cmd, PKGBUILD_CMD(epoch), pkgname);
	retrieve(cmd, &epoch);

	// get pkgver for current pkgname from pkgbuild
	get_str(&cmd, PKGBUILD_CMD(pkgver), pkgname);
	retrieve(cmd, &pkgver);

	// get pkgrel for current pkgname from pkgbuild
	get_str(&cmd, PKGBUILD_CMD(pkgrel), pkgname);
	retrieve(cmd, &pkgrel);	

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
	free(cmd);
	free(epoch);
	free(pkgver);
	free(pkgrel);

	return full_ver;
}