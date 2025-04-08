#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Commands in one place - easier to change.
#define GIT_CLONE "git clone %s" 
#define GIT_PULL "cd %s && git pull"
#define GIT_PULL_NULL "cd %s && git pull &> /dev/null"
#define AUR_CLONE "git clone https://aur.archlinux.org/%s.git"
#define AUR_CLONE_NULL "git clone https://aur.archlinux.org/%s.git &> /dev/null"
#define GIT_CLEAN "cd %s && git clean -dfx"
#define AUR_SEARCH "https://aur.archlinux.org/rpc/v5/search/%s?by=name"
#define AUR_PKG "https://aur.archlinux.org/rpc/v5/info?arg[]=%s"
#define LESS_PKGBUILD "cd %s && less PKGBUILD"
#define MAKEPKG "cd %s && makepkg -sirc OPTIONS=-debug && git clean -dfx"
#define UNINSTALL "sudo pacman -Rsc"
#define META ".packages-meta-v1.json.gz"
#define META_LINK "https://aur.archlinux.org/packages-meta-v1.json.gz"

// Console colours
#define RESET "\e[0m"
#define BOLD "\e[0;1m"
#define BYELLOW "\033[1;33m"
#define BRED "\033[1;31m"
#define BBLUE "\033[1;34m"
#define BCYAN "\033[1;36m"
#define BGREEN "\e[1;32m"
#define GREY "\033[38;5;8m"

#define MAX_BUFFER 1024

typedef struct node List;

char *get_buffer(const char *cmd);
void get_str(char **str, const char *p, const char *str_var);
bool is_dir(char *pkgname);
bool file_exists(char *path);
bool prompt(void);
void remove_dir(char *path);
List *get_dir_list(void);

#endif