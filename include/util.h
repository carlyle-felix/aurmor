#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Commands in one place - easier to change.
#define GIT_CLONE "git clone %s" 
#define GIT_PULL_NULL "git pull &> /dev/null"
#define AUR_CLONE "git clone https://aur.archlinux.org/%s.git"
#define AUR_CLONE_NULL "git clone https://aur.archlinux.org/%s.git &> /dev/null"
#define AUR_SEARCH "https://aur.archlinux.org/rpc/v5/search/%s?by=name"
#define AUR_PKG "https://aur.archlinux.org/rpc/v5/info?arg[]=%s"
#define LESS_PKGBUILD "less PKGBUILD"
#define MAKEPKG "makepkg"
#define NO_DEBUG "OPTIONS=-debug"
#define FORCE "-f"
#define META ".packages-meta-v1.json.gz"    // not being used yet
#define META_LINK "https://aur.archlinux.org/packages-meta-v1.json.gz"  // not being used yet

#define ROOT 0

// Console colours
#define RESET "\033[0m"
#define BOLD "\033[0;1m"
#define BYELLOW "\033[1;33m"
#define BRED "\033[1;31m"
#define BBLUE "\033[1;34m"
#define BCYAN "\033[1;36m"
#define BGREEN "\033[1;32m"
#define GREY "\033[38;5;8m"

typedef struct package List;

char *get_buffer(const char *cmd);
void get_str(char **str, const char *p, const char *str_var);
bool is_dir(const char *path);
bool file_exists(char *path);
bool prompt(void);
bool less_prompt(const char *pkgname);
void remove_dir(char *path);
void change_owner(char *path);
List *get_dir_list(void);
char *change_dir(const char *dir);
void gain_root(void);
void drop_root(void);
int build();

#endif