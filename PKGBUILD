# $Id$

pkgname='dataworks'
pkgver='0.1.0'
pkgdesc='Database System'
arch=('i686' 'x86_64')
license=('BSD')
pkgrel='23'
makedepends=('byacc')
source=('dataworks::svn+http://sw.nishi.boats/svn/nishi-dataworks/trunk#revision=182')
sha256sums=('SKIP')

build() {
	cd dataworks
	make clean
	make no-doc YACC=byacc
}

package() {
	cd dataworks
	mkdir -p $pkgdir/usr/include
	mkdir -p $pkgdir/usr/lib
	mkdir -p $pkgdir/usr/bin
	cp -rf Library/*.h $pkgdir/usr/include/
	cp -rf Library/*.a $pkgdir/usr/lib/
	cp -rf Library/*.so $pkgdir/usr/lib/
	cp -rf Server/dataworks_server $pkgdir/usr/bin/
	cp -rf Client/dataworks $pkgdir/usr/bin/
	ln -sf dataworks_server $pkgdir/usr/bin/dwserv
	ln -sf dataworks $pkgdir/usr/bin/dw
}
