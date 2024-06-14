# $Id$

.PHONY: all no-doc replace format clean ./Library ./Client ./Document ./Server ./Grammar ./RemoteClient ./Installer ./Package/PKGBUILD archive archive-prepare archive-cleanup archive-targz archive-zip dosbox prepare-dosbox dosbox-x cleanup-dosbox get-version thanks-banner dos-installer

all: ./Grammar ./Library ./Client $(SERVER) $(RCLI) $(INSTALLER) ./Document
	@$(MAKE) thanks-banner

no-doc: ./Grammar ./Library ./Client $(SERVER) $(RCLI) $(INSTALLER)
	@$(MAKE) thanks-banner

thanks-banner:
	@echo "+-----------------------------------------+"
	@echo "|                                         |"
	@echo "|  Thanks for choosing DataWorks.         |"
	@echo "|            - Nishi <nishi@nishi.boats>  |"
	@echo "|                                         |"
	@echo "+-----------------------------------------+"

./Grammar::
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Library:: ./Grammar
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Client:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Server:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./RemoteClient:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Installer:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Document::
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

FILES = `find . -name "*.c" -or -name "*.h"`

replace:
	for i in $(FILES) ./Grammar/dw.y ./Grammar/dw.l; do \
                echo -n "$$i ... "; \
                perl ./Tool/replace.pl < $$i > $$i.new; \
                mv $$i.new $$i; \
                echo "done"; \
        done

format:
	clang-format -i $(FILES)

clean:
	$(MAKE) -C ./Grammar clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Library clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Client clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Server clean $(COMPILE_FLAGS)
	$(MAKE) -C ./RemoteClient clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Installer clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Document clean $(COMPILE_FLAGS)

dos-installer:
	if [ ! "$(FORMAT)" = "NO" ]; then $(MAKE) clean ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then $(MAKE) PLATFORM=dos no-doc ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then rm -f install.img ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mformat -C -f 1440 -v DWINST -i install.img :: ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img Client/*.exe ::dw.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img Server/*.exe ::dwserv.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img RemoteClient/*.exe ::dwrcli.exe ; fi
	$(MAKE) PLATFORM=dos TARGET=clean ./Installer
	$(MAKE) PLATFORM=dos ./Installer
	mcopy -oi install.img Installer/*.exe ::install.exe
	$(MAKE) PLATFORM=dos TARGET=clean ./Installer
	$(MAKE) PLATFORM=dos PC98="-DPC98 -zk0" INDEP=jp INDEP_TO=cp932 ./Installer
	mcopy -oi install.img Installer/*.exe ::insta98.exe
	if [ ! -e lha.exe ]; then wget http://f.nishi.boats/f/g/lha.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img lha.exe ::lha.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mkdir -p install-workdir ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "[cpu]" > install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "cycles=30000" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "[autoexec]" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "imgmount -t floppy a: ./install.img" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "a:" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "mount c: ./install-workdir" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "lha a C:\EXTRACT.LZH dw.exe dwserv.exe dwrcli.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del dw.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del dwserv.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del dwrcli.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "c:" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "a:\lha s extract.lzh" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "a:" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "copy C:\EXTRACT.EXE" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del lha.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "exit" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then dosbox -conf install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then rm -rf install-workdir ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then rm -f install-dosbox.conf ; fi
	$(MAKE) clean

archive-prepare: all
	rm -f dataworks.zip dataworks.tar.gz
	rm -rf dataworks-dist
	mkdir -p dataworks-dist
	mkdir -p dataworks-dist/RemoteClient
	mkdir -p dataworks-dist/Server
	mkdir -p dataworks-dist/Client
	mkdir -p dataworks-dist/Library
	mkdir -p dataworks-dist/Document
	-cp Library/*$(LIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*$(STATICLIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*.lib dataworks-dist/Library/
	-cp Server/dataworks_server$(EXEC_SUFFIX) dataworks-dist/Server/
	-rmdir dataworks-dist/Server
	-cp RemoteClient/dataworks_remote_client$(EXEC_SUFFIX) dataworks-dist/RemoteClient/
	-rmdir dataworks-dist/RemoteClient
	cp Library/*.h dataworks-dist/Library/
	cp Client/dataworks$(EXEC_SUFFIX) dataworks-dist/Client/
	cp -rf Document/doc/html dataworks-dist/Document/html

archive-cleanup:
	rm -rf dataworks-dist

archive-targz:
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-prepare ; fi
	tar czvf dataworks.tar.gz dataworks-dist
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-cleanup ; fi

archive-zip:
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-prepare ; fi
	zip -rv dataworks.zip dataworks-dist
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-cleanup ; fi

archive:
	$(MAKE) archive-prepare
	-$(MAKE) archive-targz PREP=NO
	-$(MAKE) archive-zip PREP=NO
	$(MAKE) archive-cleanup

prepare-dosbox: no-doc
	echo 'create_table("test", "string:key", "floating:value");' > op.txt
	echo '.tables' >> op.txt
	echo "[serial]" > dosbox.conf
	echo "serial1=modem listenport:4096" >> dosbox.conf
	echo "[sdl]" >> dosbox.conf
	echo "output=openglpp" >> dosbox.conf
	echo "windowresolution=640x400" >> dosbox.conf
	echo "[render]" >> dosbox.conf
	echo "aspect=true" >> dosbox.conf
	echo "aspect_ratio=9:5" >> dosbox.conf
	echo "[cpu]" >> dosbox.conf
	echo "cycles=12000" >> dosbox.conf
	echo "[autoexec]" >> dosbox.conf
	echo "mount c: ." >> dosbox.conf
	echo "c:" >> dosbox.conf
	echo "copy Server\*$(EXEC_SUFFIX) dwserv$(EXEC_SUFFIX)" >> dosbox.conf
	echo "copy Client\*$(EXEC_SUFFIX) dw$(EXEC_SUFFIX)" >> dosbox.conf
	echo "dwserv -p COM1 db.dwf" >> dosbox.conf
	echo "dw /NC /f op.txt /create db.dwf" >> dosbox.conf
	echo "dw /NC /f op.txt db.dwf" >> dosbox.conf
	echo "pause" >> dosbox.conf
	echo "del db.dwf" >> dosbox.conf
	echo "del dw.exe" >> dosbox.conf
	echo "del dwserv.exe" >> dosbox.conf
	echo "exit" >> dosbox.conf
	if [ ! -e "dos4gw.exe" ]; then wget "https://github.com/yetmorecode/dos32a-ng/releases/download/9.1.2/DOS32ANG.EXE" -O dos4gw.exe ; fi

dosbox: prepare-dosbox
	dosbox
	$(MAKE) cleanup-dosbox

dosbox-x: prepare-dosbox
	dosbox-x -conf dosbox.conf
	$(MAKE) cleanup-dosbox

cleanup-dosbox:
	rm -f op.txt dosbox.conf DW.* DWSERV.* dw.* dwserv.* dosbox.core

get-version:
	@echo $(VERSION)

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
