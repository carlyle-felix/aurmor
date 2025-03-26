#include <stdio.h>
#include <ctype.h>
#include "../include/buffer.h"
#include "../include/update.h"
#include "../include/install.h"
#include "../include/memory.h"

void update(void) {
	
	char c, *placeholder, *cmd, *pkglist, pkgname[NAME_LEN];
	register int i;
	Buffer pacman_list, git;

	pkglist = mem_malloc(VSTR(pkglist), sizeof(char));
	cmd = mem_malloc(VSTR(cmd), sizeof(char));

	get_buffer("echo $(sudo pacman -Qmq)", &pacman_list);
	placeholder = pacman_list;
	while (*pacman_list != '\0') {
		for (i = 0; i < NAME_LEN; i++) {
			pkgname[i] = '\0';
		}
		for (i = 0; *pacman_list != ' ' && *pacman_list != '\n'; i++) {
			pkgname[i] = *pacman_list++;	
		}
		pacman_list++;

		cmd = mem_realloc(cmd, VSTR(cmd), strlen(pkgname) + 16);
		sprintf(cmd, "cd %s && git pull", pkgname);

		get_buffer(cmd , &git);
		if (strcmp(git, "Already up to date.\n") != 0) {
			pkglist = mem_realloc(pkglist, VSTR(pkglist), strlen(pkglist) + strlen(pkgname) + 2);
			strcat(pkglist, pkgname);
			strcat(pkglist, " ");
		}
	}
	free(git);
	free(placeholder);
	
	placeholder = pkglist;
	if (*pkglist == '\0') {
		printf(" Nothing to do.\n");
	} else {
		printf(":: Updates are available for: %s\n", pkglist);
		printf(":: Proceed with installation? [Y/n] ");
		for(;;) {
			c = tolower(getchar());
			if (c == 'y') {
				while(*pkglist != '\0') {
					for (i = 0; i < NAME_LEN; i++) {
						pkgname[i] = '\0';
					}
					for (i = 0; *pkglist != '\n'; i++) {
						if (*pkglist == ' ') {
							pkglist++;
							break;
						}
						pkgname[i] = *pkglist++;
					}
					resolve(pkgname);
				}
			} else if (c == 'n') {
				break;
			}
		}
	}
	free(placeholder);
	free(cmd);
}