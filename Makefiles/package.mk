# $Id: common.mk 312 2024-06-15 02:13:12Z nishi $

.PHONY: ./Package/PKGBUILD

./Package/PKGBUILD:
	echo "# \$$Id\$$" > $@
	echo >> $@
	echo "pkgname='dataworks'" >> $@
	echo "pkgver='"$(VERSION)"'" >> $@
	echo "pkgdesc='Database System'" >> $@
	echo "arch=('i686' 'x86_64')" >> $@
	echo "license=('BSD')" >> $@
	echo "pkgrel='`cat ./Package/increment-PKGBUILD`'" >> $@
	echo "makedepends=('byacc')" >> $@
	echo "source=('dataworks::svn+http://sw.nishi.boats/svn/nishi-dataworks/trunk#revision="`svn info -r HEAD | grep "Revision" | grep -Eo "[0-9]+" | xargs expr 1 +`"')" >> $@
	echo "sha256sums=('SKIP')" >> $@
	echo >> $@
	echo "build() {" >> $@
	echo "	cd dataworks" >> $@
	echo "	make clean" >> $@
	echo "	make no-doc YACC=byacc" >> $@
	echo "}" >> $@
	echo >> $@
	echo "package() {" >> $@
	echo "	cd dataworks" >> $@
	echo "	mkdir -p \$$pkgdir/usr/include" >> $@
	echo "	mkdir -p \$$pkgdir/usr/lib" >> $@
	echo "	mkdir -p \$$pkgdir/usr/bin" >> $@
	echo "	cp -rf Library/*.h \$$pkgdir/usr/include/" >> $@
	echo "	cp -rf Library/*.a \$$pkgdir/usr/lib/" >> $@
	echo "	cp -rf Library/*.so \$$pkgdir/usr/lib/" >> $@
	echo "	cp -rf Server/dataworks_server \$$pkgdir/usr/bin/" >> $@
	echo "	cp -rf Client/dataworks \$$pkgdir/usr/bin/" >> $@
	echo "	ln -sf dataworks_server \$$pkgdir/usr/bin/dwserv" >> $@
	echo "	ln -sf dataworks \$$pkgdir/usr/bin/dw" >> $@
	echo "}" >> $@
	expr `cat ./Package/increment-PKGBUILD` + 1 > ./Package/increment-PKGBUILD
