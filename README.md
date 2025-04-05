# AURX

An AUR helper written in C.

## MANUAL INSTALLATION

```
$ git clone https://github.com/carlyle-felix/aurx.git
$ cd aurx
$ make
# make install
```

## USAGE

| OPERATION | DESCRIPTION |
| ------- | ----------- |
| `aurx -u` | update all packages. |
| `aurx -U [package(s)]`| force update package(s).|
| `aurx -i [package(s)]` | install from [AUR](https://aur.archlinux.org/). |
| `aurx -x [git clone URL]` | clone and install from a specified git repo with PKGBUILD.|
| `aumor -r [package(s)]` | uninstall package(s). |
| `aurx -c` | delete all source directories in cache besides source directories from installed packages not found on the [AUR](https://aur.archlinux.org/). |
| `aurx -r [package(s)]` | uninstall specified AUR package(s). |
| `aurx -q` | list installed AUR packages. |
| `aurx -h` | help. |
| `aurx -s` | search package on [AUR](https://aur.archlinux.org/). |

## NOTES

- The uninstall function requires the name of the target package as it is found in the output of `aurx -q`.
- Packages are built with `OPTIONS=-debug`.
