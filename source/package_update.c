#include <stdio.h>
#include <ctype.h>
#include "../include/buffer.h"
#include "../include/update.h"
#include "../include/install.h"
#include "../include/memory.h"

void update(void) {
	
	char c, *temp, *cmd = NULL, *pkglist = NULL, pkgname[NAME_LEN];
	register int i;
	Buffer pacman_list = NULL, git = NULL;

	mem_alloc(&pkglist, VSTR(pkglist), sizeof(char));		// must malloc here in order to realloc later on with strlen()

	get_buffer("echo $(sudo pacman -Qmq)", &pacman_list);
	temp = pacman_list;
	while (*pacman_list != '\0') {
		for (i = 0; i < NAME_LEN; i++) {
			pkgname[i] = '\0';
		}
		for (i = 0; *pacman_list != ' ' && *pacman_list != '\n'; i++) {
			pkgname[i] = *pacman_list++;	
		}
		pacman_list++;

		mem_alloc(&cmd, VSTR(cmd), sizeof(char) * (strlen(pkgname) + 16));
		sprintf(cmd, "cd %s && git pull", pkgname);

		get_buffer(cmd , &git);
		if (strcmp(git, "Already up to date\n") != 0) {  //test
			mem_alloc(&pkglist, VSTR(pkglist), sizeof(char) * (strlen(pkglist) + strlen(pkgname) + 2));
			strcat(pkglist, pkgname);
			strcat(pkglist, " ");
		}
	}
	free(git);
	free(temp);
	
	temp = pkglist;

	/*Fix this section, it looks like if i choose to not proceed with installation right now, 
	  I won't be prompted to update my out of date packages next time.*/
	if (*pkglist == '\0') {
		printf(" Nothing to do.\n");
	} else {
		printf(":: Updates are available for: %s\n", pkglist);
		printf(":: Proceed with installation? [Y/n] ");
		while(*pkglist != '\0') {
			c = tolower(getchar());
			while(getchar() != '\n');
			if (c == 'y' || c == '\n') {
				while(*pkglist != '\n' && *pkglist != '\0') {
					for (i = 0; i < NAME_LEN; i++) {
						pkgname[i] = '\0';
					}
					for (i = 0; *pkglist != '\n'; i++) {
						if (*pkglist != ' ' && *pkglist != '\n') {
							pkgname[i] = *pkglist++;
						} else {
							pkglist++;
							break;
						}
					}
					resolve(pkgname);
				}
			} else if (c == 'n') {
				break;
			}
		}
	}
	free(temp);
	free(cmd);
}