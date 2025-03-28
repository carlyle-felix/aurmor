#include <stdio.h>
#include "../include/uninstall.h"
#include "../include/buffer.h"
#include "../include/memory.h"

void uninstall(char *pkgname) {

    char *cmd = NULL;
    char *pkglist = NULL;

    if (pkgname == NULL) {
        mem_alloc(&pkglist, MAX_BUFFER);
        printf(":: Fetching list of installed AUR packages...\n");
        system("pacman -Qmq");
        printf(":: Enter package name(s) to uninstall: ");
        fgets(pkglist, MAX_BUFFER, stdin);
    } else {
        pkglist = pkgname;
    }
    
    get_cmd(&cmd, "sudo pacman -Rsc %s", pkglist);
    system(cmd);

    if (pkgname == NULL) {
        free(pkglist);
    }
    free(cmd);

    //ask before cleaning...
    clean();
}

// rewrite to use linked lists.
void clean(void) {

    char *temp1, *temp2, *cmd = NULL, pkgname[NAME_LEN], dirname[NAME_LEN];
    Buffer pacman_list = NULL, aur_dir = NULL;
    register int i;

    retrieve("echo $(sudo pacman -Qmq)", &pacman_list);
    retrieve("echo $(ls)", &aur_dir);
    temp1 = pacman_list;
    temp2 = aur_dir;
    
    if (strcmp(pacman_list, aur_dir) == 0) {
        printf(" Nothing to do.\n");
        free(temp1);
        free(temp2);
        exit(EXIT_SUCCESS);
    }

    mem_alloc(&cmd, sizeof(char));
	while (*pacman_list != '\0') {
		for (i = 0; i < NAME_LEN; i++) {
			pkgname[i] = '\0';
		}
		for (i = 0; *pacman_list != ' ' && *pacman_list != '\n'; i++) {
			pkgname[i] = *pacman_list++;	
		}
		pacman_list++;

        while (*aur_dir != '\0') {
            for (i = 0; i < NAME_LEN; i++) {
                dirname[i] = '\0';
            }
			for (i = 0; *aur_dir != ' ' && *aur_dir != '\n'; i++) {
                dirname[i] = *aur_dir++;
            }
            aur_dir++;
            if (strcmp(pkgname, dirname) == 0) {
                get_cmd(&cmd, "cd %s && git clean -dfx", dirname);
                system(cmd);    // won't run if pkglist == aurdir
                break;
            } else {
                printf(" Removing %s from AUR directory...\n", dirname); 
                get_cmd(&cmd, "rm -rf %s", dirname);
                system(cmd);
            }
		}       
    }
    free(cmd);
    free(temp1);
    free(temp2);
}