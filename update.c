#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collect.h"
#include "update.h"
#include "memory.h"
#include "install.h"

#define NAME_LEN 100

void update(void) {
	
	char *placeholder, *placeholder_, *temp, *cmd, *pkgbuffer, *pkglist, *git, pkgname[NAME_LEN];
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
		if (strcmp(git, "Already to date") != 0) {
			pkglist = mem_realloc(pkglist, VSTR(pkglist), strlen(pkglist) + strlen(pkgname) + 2);
			strcat(pkglist, pkgname);
			strcat(pkglist, " ");
		}
	}
	printf(":: Updates are available for: %s\n", pkglist);
	
	free(placeholder);
	free(cmd);
	free(git);
}
