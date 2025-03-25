#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "collect.h"
#include "update.h"
#include "memory.h"
#include "install.h"


#define NAME_LEN 100

void resolve(char *pkglist);

void update(void) {
	
	char c, *placeholder, *placeholder_, *temp, *cmd, *pkgbuffer, *pkglist, *git, pkgname[NAME_LEN];
	register int i;

	pkglist = mem_malloc(VSTR(pkglist), sizeof(char));
	cmd = mem_malloc(VSTR(cmd), sizeof(char));

	collect("echo $(ls)", &pkgbuffer);
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

		collect(cmd , &git);
		if (!strcmp(git, "Already to date.")) {
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

void resolve(char *pkgname) {

	char c, *cmd;
	register int i;
	
		printf(":: View %s PKGBUILD in less? [Y/n/q] ", pkgname);
		if ((c = tolower(getchar())) == 'y') {
			cmd = mem_malloc(VSTR(cmd), strlen(pkgname) + 21);
			sprintf(cmd, "cd %s && less PKGBUILD", pkgname);
			system(cmd);
			printf(":: Continue to install? [Y/n] ");
			for(;;) {
				c = tolower(getchar());
				if (c == 'y') {
					cmd = mem_realloc(cmd, VSTR(cmd), strlen(pkgname) + 40);
					sprintf(cmd, "cd %s && makepkg -sirc && git clean -dfx", pkgname);
					system(cmd);
					return;
				} else if (c == 'n') {
					return;
				} 
			} 
		} else if (c == 'n') {
			cmd = mem_malloc(VSTR(cmd), strlen(pkgname) + 40);
			sprintf(cmd, "cd %s && makepkg -sirc && git clean -dfx", pkgname);
			system(cmd);
			return;
		}
}