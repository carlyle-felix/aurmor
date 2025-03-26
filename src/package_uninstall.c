#include <stdio.h>
#include "../include/uninstall.h"
#include "../include/buffer.h"
#include "../include/memory.h"

void clean(void) {

    char *temp1, *temp2, *cmd, pkgname[NAME_LEN], dirname[NAME_LEN];
    Buffer pacman_list, aur_dir;
    register int i;

    get_buffer("echo $(sudo pacman -Qmq)", &pacman_list);
    get_buffer("echo $(ls)", &aur_dir);
	temp1 = pacman_list;
    temp2 = aur_dir;
    
    if (strcmp(pacman_list, aur_dir) == 0) {
        printf(" Nothing to do.");
        free(temp1);
        free(temp2);
        exit(EXIT_SUCCESS);
    }

    cmd = mem_malloc(VSTR(cmd), sizeof(char));
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
                break;
            } else {
                printf(" deleting %s...\n", dirname); 
                cmd = mem_realloc(cmd, VSTR(cmd), strlen(dirname) + 8);
                sprintf(cmd, "rm -rf %s", dirname);
                system(cmd);
            }
		}       
    }
    free(cmd);
    free(temp1);
    free(temp2);
}