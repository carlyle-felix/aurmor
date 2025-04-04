#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Console colours
#define RESET "\e[0m"
#define BOLD "\e[0;1m"
#define BYELLOW "\033[1;33m"
#define BRED "\033[1;31m"
#define BBLUE "\033[1;34m"
#define BGREEN "\e[1;32m"
#define GREY "\033[38;5;8m"

#define MAX_BUFFER 1024

typedef char *Buffer;

Buffer get_buffer(const char *cmd);
void get_str(char **cmd, const char *str, const char *str_var);
bool is_dir(char *pkgname);
bool file_exists(char *path);

#endif