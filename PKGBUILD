# Maintainer: carlyle

pkgname=aurx
pkgver=1
pkgrel=1
pkgdesc="An AUR helper written in C."
arch=('x86_64')
url="https://github.com/carlyle-felix/aurx"
license=( 'GPL-3.0-or-later' )
depends=(
        'base-devel'
        'git'
        'pacman'
        'pacutils'
        'json-c'
        'libcurl-gnutls'
)
source=("${pkgname}::git+${url}.git")
pkgver() {
        cd "$pkgname"
        git describe --long --tags --abbrev=7 | sed 's/\([^-]*-g\)/r\1/;s/-/./g'
}
sha256sums=('SKIP')
build() {
	cd "${srcdir}/${pkgname}"
        make
}
package() {
        cd "${srcdir}/${pkgname}"
        make DESTDIR="$pkgdir" install
}