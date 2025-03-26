# AURMOR

An Arch User Repository manager written in C.

## MANUAL INSTALLATION

```
$ git clone https://github.com/carlyle-felix/aurmor.git
$ cd aurmor
$ make
# make install
```

## USAGE

| COMMAND | DESCRIPTION |
| ------- | ----------- |
|`aurmor -u` | update all packages. |
|`aurmor -i [git clone URL]` | clone and install. |
| `aurmor -c` | delete directories found in ~/.aur directory but not found in pacman's list of installed AUR packages. |
| `aurmor -r [package name]` | uninstall specified package. caveat: name must match the one found in `pacman -Qmq` |
| `aurmor -h` | help |

## NOTES

- The uninstall function requires the name of the target package as it is found in the output of `# pacman -Qmq`.
- Packages are built with `OPTIONS=-debug`.