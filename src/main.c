#include <stdio.h>
#include <unistd.h>
#include "../include/install.h"
#include "../include/update.h"
#include "../include/memory.h"

int main(int argc, char *argv[]) {
	
	char *home, *aur;
	
	home = getenv("HOME");
	aur = mem_malloc(VSTR(aur), strlen(home) + 6);
	sprintf(aur, "%s/.aur", home);
	chdir(aur);
	
	if (argc == 1 || argv[1] == "-h") {
        printf("-u              				update\n");
		printf("-i [git clone URL]				install\n");
	} else if (strcmp(argv[1], "-u") == 0) {
		update();
	} else if (strcmp(argv[1], "-i") == 0) { 
		clone(argv[2]);
	} 

	free(aur);
	return 0;
}
