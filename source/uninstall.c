#include <stdio.h>
#include "../include/uninstall.h"
#include "../include/buffer.h"
#include "../include/memory.h"
#include "../include/list.h"

void uninstall(char *pkgname) {

    char *cmd = NULL;
    
    get_str(&cmd, "sudo pacman -Rsc %s", pkgname);
    system(cmd);

    if (pkgname == NULL) {
        free(pkgname);
    }
    free(cmd);

    clean();
}

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
            printf("Removing %s from AUR directory...\n", dir->pkgname); 
            get_str(&cmd, "rm -rf %s", dir->pkgname);
            system(cmd);
        } else {
            get_str(&cmd, "cd %s && git clean -dfx", dir->pkgname);
            system(cmd);
            pacman = pacman->next;
            }
        dir = dir->next;
    }
    free(cmd);
    clear_list(temp1);
    clear_list(temp2);
}

void list_packages(void) {
    
    system("pacman -Qm");
}