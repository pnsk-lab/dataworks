# $Id$

.PHONY: ./Package/dataworks.deb

./Package/dataworks.deb: no-doc
	rm -rf workdir-deb
	mkdir workdir-deb
	mkdir workdir-deb/DEBIAN
	mkdir -p workdir-deb/usr/lib
	mkdir -p workdir-deb/usr/include
	mkdir -p workdir-deb/usr/bin
	cp Library/*.so workdir-deb/usr/lib/
	cp Library/*.a workdir-deb/usr/lib/
	cp Library/*.h workdir-deb/usr/include/
	cp Client/dataworks workdir-deb/usr/bin/
	cp Server/dataworks_server workdir-deb/usr/bin/
	cp RemoteClient/dataworks_remote_client workdir-deb/usr/bin/
	cd workdir-deb && find usr -type f | xargs $(MD5SUM) > DEBIAN/md5sums
	echo "Package: dataworks" > workdir-deb/DEBIAN/control
	echo "Version: $(VERSION)" >> workdir-deb/DEBIAN/control
	echo "Architecture: $(DEB_ARCH)" >> workdir-deb/DEBIAN/control
	echo "Maintainer: Nishi <nishi@nishi.boats" >> workdir-deb/DEBIAN/control
	echo "Depends: libc6 (>= 2.14)" >> workdir-deb/DEBIAN/control
	echo "Homepage: http://nishi.boats/dataworks" >> workdir-deb/DEBIAN/control
	echo "Installed-Size: `du -k workdir-deb/usr | tail -n1 | grep -Eo "^[0-9]+"`" >> workdir-deb/DEBIAN/control
	echo "Description: Simple DBMS written in C" >> workdir-deb/DEBIAN/control
	fakeroot dpkg-deb --build workdir-deb $@
	rm -rf workdir-deb
