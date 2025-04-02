#include <stdio.h>
#include <ctype.h>
#include "../include/install.h"
#include "../include/memory.h"
#include "../include/buffer.h"

void install(const char *pkgname) {
    
    char *cmd = NULL;

    get_str(&cmd, "cd %s && makepkg -sirc OPTIONS=-debug && git clean -dfx", pkgname); // don't build -debug packages for now.
    system(cmd);
    free(cmd);
}

void target_clone(const char *url) {

    char *cmd = NULL, pkgname[NAME_LEN] = {'\0'};
    register int i;

    get_str(&cmd, "git clone %s", url);
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

    less_prompt(pkgname);
}

void aur_clone(const char *pkgname) {

    char *cmd = NULL;

    get_str(&cmd, "git clone https://aur.archlinux.org/%s.git", pkgname);
    system(cmd);

    less_prompt(pkgname);

    free(cmd);
}

void less_prompt(const char *pkgname) {

	char c, *cmd = NULL;
	register int i;

    printf("\033[1;34m:: \e[0;1mView %s PKGBUILD in less? [Y/n]\e[0m ", pkgname);
    for (;;) {
        c = tolower(getchar());
        if (c != '\n') {
			while (getchar() != '\n');
		}
        if (c == 'y' || c == '\n') {
            get_str(&cmd, "cd %s && less PKGBUILD", pkgname);
            system(cmd);
            free(cmd);
            printf("\033[1;34m:: \e[0;1mContinue to install? [Y/n]\e[0m ");
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