#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>

#include "../include/util.h"
#include "../include/memory.h"
#include "../include/list.h"

void traverse_dir(char *path, char *func, int uid, int gid); // make it use va_args?

/* 
* 	No longer in use! might replace system() with this function.
* 	pipe output of commands to a buffer and return the buffer.
*/
char *get_buffer(const char *cmd) {
	
	char *buffer = NULL;
	int read, max = MAX_BUFFER;
	FILE *p;

	for (;;) {
		str_alloc(&buffer, max);
		p = popen(cmd, "r");
		if (p == NULL) {
			printf(BRED"error:"RESET" failed in buffer().\n");
			free(buffer);
			return NULL;
		}
		read = fread(buffer, sizeof(char), max, p);
		if (read == max) {
			max *= 2;
		} else {
			pclose(p);
			break;
		}
	}
	buffer[read] = '\0';
	if (buffer[0] == '\0') {
		free(buffer);
		return NULL;
	}

	return buffer;
}

// copy a string into a block of memory.
void get_str(char **p, const char *str, const char *str_var) {
	
	if (str_var != NULL) {
		str_alloc(p, strlen(str) + strlen(str_var) - 1);
		sprintf(*p, str, str_var);
	} else {
		str_alloc(p, strlen(str) + 1);
		sprintf(*p, str);
	}
}

bool prompt(void) {

	char c;

	for (;;) {
        c = tolower(getchar());
        if (c != '\n') {
			while (getchar() != '\n');
		}
        if (c == 'y' || c == '\n') {
			return true;
        } else if (c == 'n') {
			return false;
        }
    }
}

bool file_exists(char *path) {
	
	FILE *f;

	f = fopen(path, "r");
	if (f != NULL) {
		fclose(f);
		return true;
	} 

	return false;
}

bool is_dir(const char *path) {

	DIR* dir = opendir(path);
	if (dir != NULL) {
		closedir(dir);
		return true;
	}
		
	return false;
}

// remove directories recursively.
void remove_dir(char *path) {

	traverse_dir(path, "rm", ROOT, ROOT);
	rmdir(path);
}

void change_owner(char *path) {

	int uid, gid;

	uid = geteuid();
	gid = getegid();

	gain_root();
	traverse_dir(path, "chown", uid, gid);
	chown(path, uid, gid);
	drop_root();
}

void traverse_dir(char *path, char *func, int uid, int gid) {

	DIR *dir;
	struct dirent *p;
	char temp_path[MAX_BUFFER];

	dir = opendir(path);
	if (dir == NULL) {
		if (file_exists(path) == true) {
			remove(path);
			return;
		}
		printf(BRED"error:"RESET"Failed to open %s directory.\n", path);
		exit(EXIT_FAILURE);
	}

	while ((p = readdir(dir)) != NULL) {
		if (strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0) {
			continue;
		}

		sprintf(temp_path, "%s/%s", path, p->d_name);
		if (strcmp(func, "rm") == 0) {
			if (is_dir(temp_path)) {
				remove_dir(temp_path);
			} else {
				remove(temp_path);
			}		
		} else if (strcmp(func, "chown") == 0){
			if (is_dir(temp_path)) {
				traverse_dir(temp_path, "chown", uid, gid);
				chown(temp_path, uid, gid);
			} else {
				chown(temp_path, uid, gid);
			}	
		}
		
	}
	closedir(dir);
}

// get list of items in the .cache/aur directory.
List *get_dir_list(void) {

	DIR *dir;
	struct dirent *p;
	List *dir_list;

	dir = opendir(".");
	if (dir == NULL) {
		printf(BRED"error:"RESET"Failed to open aurx cache directory.\n");
		exit(EXIT_FAILURE);
	}
	
	dir_list = list_malloc();

	while ((p = readdir(dir)) != NULL) {
		if (strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0) {
			continue;
		}
		dir_list = add_pkgname(dir_list, p->d_name);
	}
	closedir(dir);

	if (dir_list->pkgname == NULL) {
		free(dir_list);
		return NULL;
	}
	
	return dir_list;
}

char *change_dir(const char *dir) {

	static char *home = NULL, wd[MAX_BUFFER], cwd[MAX_BUFFER];

	if (home == NULL) {
		home = getenv("HOME");
		
		strcpy(wd, home);
		strcat(wd, "/.cache/aurx");

		if (is_dir(wd) == false) {
			printf("~/.cache/aurx directory not found, creating...\n");
			mkdir(wd, 0755);
		}
	}

	// only those needed by the program.
	if (strcmp(dir, "WD") == 0) {
		chdir(wd);
	} else if (strcmp(dir, "HOME") == 0) {
		chdir(home);
	} else {
		chdir(wd);

		if (is_dir(dir) == false) {
			printf(BRED"error:"RESET" %s dir not found.\n", dir);
			return NULL;
		}
		
		chdir(dir);
	}

	return getcwd(cwd, MAX_BUFFER);
}

void gain_root(void) {

	int res;

	res = seteuid(0);
	if (res != 0) {
		printf("error: gain_root() failed\n");
	}
	printf("gained root, euid: %d\n", geteuid());
}

void drop_root(void) {

	int res;
	struct passwd *pw;
	uid_t uid;

	pw = getpwnam(getlogin());
	uid = pw->pw_uid;
	res = seteuid(uid);
	if (res != 0) {
		printf("error: drop_root() failed\n");
	}

	pw = getpwuid(uid);
	setenv("HOME", pw->pw_dir, 1);
	printf("root dropped, euid: %d\n", geteuid());
}

int build(char *pkgname) {

	int pid;
	int res;
	struct passwd *pw;

	pid = fork();
	if (pid < 0) {
		printf("error: failed to create new process.\n");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		pw = getpwnam(getlogin());
		res = setuid(pw->pw_uid);
		if (res == 0) {
			printf("build root dropped, euid: %d\n", getuid());
		}
		
		change_dir(pkgname);
		res = system(MAKEPKG);
		if (res != 0) {
			printf("system(makepkg) failed\n");
		}
		change_dir("WD");

		exit(0);
	} else {
		waitpid(pid, &res, 0);
		if (WIFEXITED(res) == false) {
			return -1;
        }
		return 0;
	}
}