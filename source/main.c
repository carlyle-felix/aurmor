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
		printf(" No operation specified, use -h for help.\n");
	} else if (strcmp(argv[1], "-h") == 0) {
		printf("USAGE:\n");
        printf("\n -u						update.\n");
		printf("\n -i [git clone URL]				install.\n");
		printf("\n -c						clean AUR dir.\n");
		printf("\n -r						get a list of installed\n\t\t\t\t\t\tpackages before uninstall.\n");
		printf("\n -r [package name]				uninstall package.\n");
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
		if (argc == 2) {
			uninstall(NULL);
		} else if (argc > 2) {		// fix this, uninstall multiple packages.
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
	mem_alloc(&aur, (strlen(home) + 15));
	sprintf(aur, "%s/.cache/aurmor", home);
	chdir(aur);
	free(aur);
}