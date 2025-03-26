# AURMGR

An Arch User Repository manager written in C.

## MANUAL INSTALLATION

```
$ git clone https://github.com/carlyle-felix/aur-manager.git
$ cd aurmgr
$ make
# make install
```

## USAGE

| COMMAND | DESCRIPTION |
| ------- | ----------- |
|`aurmgr -u` | update all packages. |
|`aurmgr -i [git clone URL]` | clone and install. |
| `aurmgr -c` | delete directories found in ~/.aur directory but not found in pacman's list of installed AUR packages. |
| `aurmgr -r [package name]` | uninstall specified package. caveat: name must match the one found in `pacman -Qmq` |
| `aurmgr -h` | help |

## NOTES

- The uninstall function requires the name of the target package as it is found in the output of `# pacman -Qmq`.
- Packages are built with `OPTIONS=-debug`.