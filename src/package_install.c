#include <stdio.h>
#include <ctype.h>
#include "../include/install.h"
#include "../include/memory.h"

void install(char *pkgname) {
    
    char *cmd = NULL;

    mem_alloc(&cmd, VSTR(cmd), strlen(pkgname) + 55);
	sprintf(cmd, "cd %s && makepkg -sirc OPTIONS=-debug && git clean -dfx", pkgname); // don't build -debug packages for now.
    system(cmd);
    free(cmd);
}

void clone(char *url) {

    char *cmd = NULL, pkgname[NAME_LEN] = {'\0'};
    register int i;

    mem_alloc(&cmd, VSTR(cmd), strlen(url) + 11);
    sprintf(cmd, "git clone %s", url);
    system(cmd);
    free(cmd);
    while (*url++ != '\0');
    while (*url != '/') {
        url--;
    }
    url++;
    for (i = 0; *url != '.'; i++) {
        pkgname[i] = *url++;
    }

    resolve(pkgname);
}

void resolve(char *pkgname) {

	char c, *cmd = NULL;
	register int i;
	
    mem_alloc(&cmd, VSTR(cmd), strlen(pkgname) + 21);

    printf(":: View %s PKGBUILD in less? [Y/n] ", pkgname);
    for (;;) {
        if ((c = tolower(getchar())) == 'y') {
            sprintf(cmd, "cd %s && less PKGBUILD", pkgname);
            system(cmd);
            free(cmd);
            printf(":: Continue to install? [Y/n] ");
            for(;;) {
                c = tolower(getchar());
                if (c == 'y') {
                    install(pkgname);
                    return;
                } else if (c == 'n') {
                    return;
                } 
            } 
        } else if (c == 'n') {
            free(cmd);
            install(pkgname);
            return;
        }
    }
}