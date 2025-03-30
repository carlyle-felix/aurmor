#include <stdio.h>
#include <ctype.h>
#include "../include/install.h"
#include "../include/memory.h"
#include "../include/buffer.h"

void install(const char *pkgname) {
    
    char *cmd = NULL;

    get_cmd(&cmd, "cd %s && makepkg -sirc OPTIONS=-debug && git clean -dfx", pkgname); // don't build -debug packages for now.
    system(cmd);
    free(cmd);
}

void clone(const char *url) {

    char *cmd = NULL, pkgname[NAME_LEN] = {'\0'};
    register int i;

    get_cmd(&cmd, "git clone %s", url);
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

void resolve(const char *pkgname) {

	char c, *cmd = NULL;
	register int i;

    printf(":: View %s PKGBUILD in less? [Y/n] ", pkgname);
    for (;;) {
        c = tolower(getchar());
        if (c != '\n') {
			while (getchar() != '\n');
		}
        if (c == 'y' || c == '\n') {
            get_cmd(&cmd, "cd %s && less PKGBUILD", pkgname);
            system(cmd);
            free(cmd);
            printf(":: Continue to install? [Y/n] ");
            for(;;) {
                c = tolower(getchar());
                if (c != '\n') {
                    while (getchar() != '\n');
                }
                if (c == 'y' || c == '\n') {
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