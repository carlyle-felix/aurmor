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
#define MAKEPKG "makepkg -sirc OPTIONS=-debug && git clean -dfx"    // don't build -debug packages for now.
#define META ".packages-meta-v1.json.gz"    // not being used yet
#define META_LINK "https://aur.archlinux.org/packages-meta-v1.json.gz"  // not being used yet

#define MAX_BUFFER 10240

typedef struct node List;

char *get_buffer(const char *cmd);
void get_str(char **str, const char *p, const char *str_var);
bool is_dir(const char *pkgname);
bool file_exists(char *path);
bool prompt(void);
void remove_dir(char *path);
List *get_dir_list(void);
int change_dir(const char *dir);
bool console_colour(void);

#endif