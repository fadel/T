# Maintainer: Samuel Fadel <samuelfadel at DOMAIN> (DOMAIN is gmail dot com)

pkgname=T
pkgver=0.5
pkgrel=1
pkgdesc="Lean Terminal emulator"
arch=('i686' 'x86_64')
license=('GPL')
depends=('gtk2' 'vte')
source=("http://domain.tld/${pkgname}-${pkgver}.tar.bz2")
sha1sums=('b0ff968aafa6e1c8ef1f331224171c0b52d3e08d')

build() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  make
}

package() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  make DESTDIR=${pkgdir} install
}
