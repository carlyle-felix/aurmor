#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../include/operation.h"
#include "../include/memory.h"
#include "../include/rpc.h"
#include "../include/list.h"
#include "../include/util.h"
#include "../include/manager.h"

int main(int argc, char *argv[]) {

	register int i;

	change_dir("WD");

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
			for (i = 2; i < argc; i++) {
				force_update(argv[i]);
			}
		} else {
			printf("Please specify package(s), use -h for help.\n");
		}
	} else if (strcmp(argv[1], "-i") == 0) {
		if (argc > 2) {
			for (i = 2; i < argc; i++) {
				aur_clone(argv[i]);
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
		print_installed();
	} else if (strcmp(argv[1], "-r") == 0) {
		if (argc > 2) {
			List *list;
			
			list = list_malloc();
			for (i = 2; i < argc; i++) {
				list = add_pkgname(list, argv[i]);
			}
			alpm_uninstall(list);
			clear_list(list);
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