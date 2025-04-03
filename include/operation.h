#ifndef OPERATION_H
#define OPERATION_H

// Commands in one place - easier to change.
#define PKGBUILD_CMD(item) "echo -n $(cd %s && echo $(less PKGBUILD | grep " \
                            #item "= | cut -f2 -d '=') | tr -d \"'\\\"\")"
#define GIT_PULL_NULL "cd %s && git pull &> /dev/null"
#define GIT_CLONE "git clone %s" 
#define AUR_CLONE "git clone https://aur.archlinux.org/%s.git"
#define AUR_CLONE_NULL "git clone https://aur.archlinux.org/%s.git &> /dev/null"
#define AUR_SEARCH "https://aur.archlinux.org//rpc/v5/search/%s?by=name"
#define AUR_PKG "https://aur.archlinux.org/rpc/v5/info?arg[]=%s"
#define LESS_PKGBUILD "cd %s && less PKGBUILD"
#define MAKEPKG "cd %s && makepkg -sirc OPTIONS=-debug && git clean -dfx"
#define INSTALLED "echo -n $(pacman -Qmq)"
#define QUERY_INSTALLED "pacman -Qm"
#define DIR_LIST "echo -n $(ls)"
#define UNINSTALL "sudo pacman -Rsc %s"
#define RM_DIR "rm -rf %s"
#define CLEAN "cd %s && git clean -dfx"

void target_clone(const char *url);
void aur_clone(const char *pkgnmae);
void less_prompt(const char *pkgname);
void uninstall(char *pkgnames);
void clean(void);
void print_search(char *pkgname);
void list_packages(void);
void update(void);

#endif