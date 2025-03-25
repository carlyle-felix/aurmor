#include <stdio.h>
#include <ctype.h>
#include "../include/install.h"
#include "../include/memory.h"

void install(char *pkgname) {
    
    char *cmd;

    cmd = mem_malloc(VSTR(cmd), strlen(pkgname) + 40);
	sprintf(cmd, "cd %s && makepkg -sirc && git clean -dfx", pkgname);
    system(cmd);
    free(cmd);
}

void clone(char *url) {

    char *cmd, pkgname[NAME_LEN] = {'\0'};
    register int i;

    cmd = mem_malloc(VSTR(cmd), strlen(url) + 11);
    sprintf(cmd, "git clone %s", url);
    system(cmd);

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

	char c, *cmd;
	register int i;
	
    printf(":: View %s PKGBUILD in less? [Y/n] ", pkgname);
    for (;;) {
        if ((c = tolower(getchar())) == 'y') {
            cmd = mem_malloc(VSTR(cmd), strlen(pkgname) + 21);
            sprintf(cmd, "cd %s && less PKGBUILD", pkgname);
            system(cmd);
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
            free(cmd);
        } else if (c == 'n') {
            install(pkgname);
            return;
        }
    }
}