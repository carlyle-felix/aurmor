#include <stdio.h>
#include <unistd.h>
#include "../include/install.h"
#include "../include/update.h"
#include "../include/uninstall.h"
#include "../include/memory.h"

void set_dir(void);

int main(int argc, char *argv[]) {

	set_dir();

	if (argc == 1) {
		printf(" No operation specified, use -h for help.");
	} else if (strcmp(argv[1], "-h") == 0) {
		printf("USAGE:\n\n");
        printf(" -u						update.\n");
		printf(" -i [git clone URL]				install.\n");
		printf(" -c						clean AUR dir.\n");
		printf(" -r [package name]				uninstall (remove).\n");
	} else if (strcmp(argv[1], "-u") == 0) {
		update();
	} else if (strcmp(argv[1], "-i") == 0) { 
		if (argc == 3) {
			clone(argv[2]);
		} else {
			printf("Incorrect amount of arguments, use -h for help.\n");
			printf("\n -i [git clone URL]				install.\n");
		}
	} else if (strcmp(argv[1], "-c") == 0) { 
		clean();
	} else if (strcmp(argv[1], "-r") == 0) {
		if (argc == 3) {
			uninstall(argv[2]);
		} else {
			printf("Incorrect amount of arguments, use -h for help.\n");
			printf("\n -r [package name]				uninstall (remove).\n");
		}
	}

	return 0;
}

void set_dir(void) {

	char *home, *aur = NULL;

	home = getenv("HOME");
	mem_alloc(&aur, VSTR(aur), strlen(home) + 6);
	sprintf(aur, "%s/.aur", home);
	chdir(aur);
	free(aur);
}