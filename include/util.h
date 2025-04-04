#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#define MAX_BUFFER 1024

typedef char *Buffer;

Buffer get_buffer(const char *cmd);
void get_str(char **cmd, const char *str, const char *str_var);
bool is_dir(char *pkgname);
bool file_exists(char *path);

#endif