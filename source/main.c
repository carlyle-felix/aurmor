#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/operation.h"
#include "../include/memory.h"
#include "../include/rpc.h"
#include "../include/list.h"
#include "../include/util.h"

void set_dir(void);

int main(int argc, char *argv[]) {

	set_dir();

	if (argc == 1) {
		printf(" No operation specified, use -h for help.\n");
	} else if (strcmp(argv[1], "-h") == 0) {
		printf("Usage:\taurx <operation> [...]\nOperations:\n");
        printf(" -u\t\t\t\t\tupdate and upgrade.\n");
		printf(" -U [package(s)]\t\t\tforce update package(s).\n");
		printf(" -i [package(s)]\t\t\tinstall package(s).\n");
		printf(" -x [git clone URL]\t\t\tinstall specified target from a git repo");
		printf(" -c\t\t\t\t\tclean ~/.cache/aurx dir.\n");
		printf(" -q\t\t\t\t\tlist installed packages.\n");
		printf(" -r [package(s)]\t\t\tuninstall package(s).\n");
		printf(" -s [package]\t\t\t\tsearch package on AUR.\n");
	} else if (strcmp(argv[1], "-u") == 0) {
		update();
	}  else if (strcmp(argv[1], "-U") == 0) {		// Doesn't order updates alphabetically (would be nice).
		if (argc > 2) {
			while (--argc > 1) {
				force_update(argv[argc]);
			}
		} else {
			printf("Please specify package(s), use -h for help.\n");
		}
	} else if (strcmp(argv[1], "-i") == 0) {
		if (argc > 2) {
			while (--argc > 1) {
				aur_clone(argv[argc]);
			}
		} else {
			printf("Please specify package(s), use -h for help.\n");
		}
	} else if (strcmp(argv[1], "-x") == 0) {
		if (argc == 3) {
			target_clone(argv[2]);
		} else if (argc > 3) {
			printf("Please specify only one target URL, use -h for help.\n");
		} else {
			printf("Please specify a target URL, use -h for help.\n");
		}
	} else if (strcmp(argv[1], "-c") == 0) { 
		clean();
	} else if (strcmp(argv[1], "-q") == 0) {
		list_packages();
	} else if (strcmp(argv[1], "-r") == 0) {
		if (argc > 2) {
			while (--argc > 1) {
				uninstall(argv[argc]); 
			}
		} else {
			printf("Please specify package(s), use -h for help.\n");
		}
	} else if (strcmp(argv[1], "-s") == 0) {
		if (argc == 3) {
			print_search(argv[2]);
		} else if (argc > 3) {
			printf("Please specify only one keyword, use -h for help.\n");
		} else {
			printf("Please specify a search keyword, use -h for help.\n");
		}
	} else {
		printf("Unkown operation, use -h for help.\n");
	}

	return 0;
}

void set_dir(void) {

	char *home, *str = NULL;

	home = getenv("HOME");
	get_str(&str, "%s/.cache/aurx", home);
	
	if (is_dir(str) == false) {
		char *mkdir = NULL;
		printf("~/.cache/aurx directory not found, creating...\n");
		get_str(&mkdir, "mkdir -p %s", str);
		system(mkdir);
		free(mkdir);
	}
	chdir(str);

	free(str);
}