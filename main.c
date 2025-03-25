#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "update.h"
#include "memory.h"
#include "install.h"

int main(int argc, char *argv[]) {
	
	char *home, *aur;
	
	home = getenv("HOME");
	aur = mem_malloc(VSTR(aur), strlen(home) + 6);
	sprintf(aur, "%s/.aur", home);
	chdir(aur);
	
	if (strcmp(argv[1], "-u") == 0) {
		update();
	} else if (strcmp(argv[1], "-i") == 0) { 
		//install();
	} else if (argc == 1) {
        printf("-u              update\n");
		printf("-i				install\n");
	} 

	free(aur);
	return 0;
}
