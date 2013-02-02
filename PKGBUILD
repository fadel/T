# Maintainer: Samuel Fadel <samuelfadel at DOMAIN> (DOMAIN is gmail dot com)

pkgname=T
pkgver=0.6
pkgrel=1
pkgdesc="Lean Terminal emulator"
arch=('i686' 'x86_64')
license=('GPL')
depends=('gtk2' 'vte')
source=("http://domain.tld/${pkgname}-${pkgver}.tar.bz2")
sha1sums=('1a0170b0e48eead516dfdb7e9c8ac34e36c5177d')

build() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  make
}

package() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  make DESTDIR=${pkgdir} install
}
