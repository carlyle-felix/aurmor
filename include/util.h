#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Commands in one place - easier to change.
#define PKGBUILD_CMD(item) "echo -n $(cd %s && echo $(less PKGBUILD | grep " \
#item "= | cut -f2 -d '=') | tr -d \"'\\\"\")"
#define GIT_PULL_NULL "cd %s && git pull &> /dev/null"
#define GIT_CLONE "git clone %s" 
#define AUR_CLONE "git clone https://aur.archlinux.org/%s.git"
#define AUR_CLONE_NULL "git clone https://aur.archlinux.org/%s.git &> /dev/null"
#define AUR_SEARCH "https://aur.archlinux.org/rpc/v5/search/%s"
#define AUR_PKG "https://aur.archlinux.org/rpc/v5/info?arg[]=%s"
#define LESS_PKGBUILD "cd %s && less PKGBUILD"
#define MAKEPKG "cd %s && makepkg -sirc OPTIONS=-debug && git clean -dfx"
#define INSTALLED "echo -n $(pacman -Qmq)"
#define QUERY_INSTALLED "pacman -Qm"
#define DIR_LIST "echo -n $(ls)"
#define UNINSTALL "sudo pacman -Rsc %s"
#define RM_DIR "rm -rf %s"
#define GIT_CLEAN "cd %s && git clean -dfx"

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

typedef char *Buffer;

Buffer get_buffer(const char *cmd);
void get_str(char **cmd, const char *str, const char *str_var);
bool is_dir(char *pkgname);
bool file_exists(char *path);
bool prompt(void);

#endif