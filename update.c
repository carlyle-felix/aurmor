#include <stdio.h>
#include <ctype.h>
#include "buffer.h"
#include "update.h"
#include "install.h"
#include "memory.h"

void update(void) {
	
	char c, *placeholder, *placeholder_, *temp, *cmd, *pkgbuffer, *pkglist, *git, pkgname[NAME_LEN];
	register int i;

	pkglist = mem_malloc(VSTR(pkglist), sizeof(char));
	cmd = mem_malloc(VSTR(cmd), sizeof(char));

	get_buffer("echo $(ls)", &pkgbuffer);
	placeholder = pkgbuffer;
	while (*pkgbuffer != '\0') {
		for (i = 0; i < NAME_LEN; i++) {
			pkgname[i] = '\0';
		}
		for (i = 0; *pkgbuffer != ' ' && *pkgbuffer != '\n'; i++) {
			pkgname[i] = *pkgbuffer++;	
		}
		pkgbuffer++;

		cmd = mem_realloc(cmd, VSTR(cmd), strlen(pkgname) + 16);
		sprintf(cmd, "cd %s && git pull", pkgname);

		get_buffer(cmd , &git);
		if (strcmp(git, "Already to date")) {
			pkglist = mem_realloc(pkglist, VSTR(pkglist), strlen(pkglist) + strlen(pkgname) + 2);
			strcat(pkglist, pkgname);
			strcat(pkglist, " ");
		}
	}
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
					if (c == 'y') {
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
						getchar();
						resolve(pkgname);
					}
				}
			} else if (c == 'n') {
				break;
			}
		}
	}
	
	free(placeholder);
	free(cmd);
	free(git);
}