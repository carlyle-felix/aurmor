#include <stdio.h>
#include "../include/uninstall.h"
#include "../include/buffer.h"
#include "../include/memory.h"
#include "../include/list.h"

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

    clean();
}

// rewrite to use linked lists.
void clean(void) {

    char *cmd = NULL;
    List *dir, *pacman, *temp1, *temp2;

    get_list(&pacman, "echo -n $(pacman -Qmq)");
    get_list(&dir, "echo -n $(ls)");
    temp1 = pacman;
    temp2 = dir;
    
	temp1 = pacman;
    temp2 = dir;
    while (dir != NULL) {
        if (pacman == NULL || strcmp(dir->pkgname, pacman->pkgname) != 0) {
            printf(" Removing %s from AUR directory...\n", dir->pkgname); 
            get_cmd(&cmd, "rm -rf %s", dir->pkgname);
            system(cmd);
        } else {
            get_cmd(&cmd, "cd %s && git clean -dfx", dir->pkgname);
            system(cmd);
            pacman = pacman->next;
            }
        dir = dir->next;
    }
    free(cmd);
    clear_list(temp1);
    clear_list(temp2);
}
