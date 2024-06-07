# $Id$

.PHONY: all no-doc replace format clean ./Library ./Client ./Document ./Grammar archive archive-prepare archive-cleanup archive-targz archive-zip dosbox prepare-dosbox dosbox-x cleanup-dosbox

all: ./Grammar ./Library ./Client $(SERVER) ./Document

no-doc: ./Grammar ./Library ./Client $(SERVER)

./Grammar::
	$(MAKE) -C $@ $(COMPILE_FLAGS)

./Library:: ./Grammar
	$(MAKE) -C $@ $(COMPILE_FLAGS)

./Client:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS)

./Server:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS)

./Document::
	$(MAKE) -C ./Document $(COMPILE_FLAGS)

FILES = `find . -name "*.c" -or -name "*.h"`

replace:
	for i in $(FILES) ./Grammar/dw.y ./Grammar/dw.l; do \
                echo -n "$$i ... "; \
                perl replace.pl < $$i > $$i.new; \
                mv $$i.new $$i; \
                echo "done"; \
        done

format:
	clang-format -i $(FILES)

clean:
	rm -f *.zip *.tar.gz PKGBUILD
	$(MAKE) -C ./Grammar clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Library clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Client clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Server clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Document clean $(COMPILE_FLAGS)

archive-prepare: all
	rm -f *.zip *.tar.gz
	rm -rf dataworks-dist
	mkdir -p dataworks-dist
	mkdir -p dataworks-dist/Server
	mkdir -p dataworks-dist/Client
	mkdir -p dataworks-dist/Library
	mkdir -p dataworks-dist/Document
	-cp Library/*$(LIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*$(STATICLIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*.lib dataworks-dist/Library/
	-cp Server/dataworks_server$(EXEC_SUFFIX) dataworks-dist/Server/
	-rmdir dataworks-dist/Server
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
	echo "windowresolution=720x500" >> dosbox.conf
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

./PKGBUILD:
	echo "# \$$Id\$$" > $@
	echo >> $@
	echo "pkgname='dataworks'" >> $@
	echo "pkgver='"`cat Library/dataworks.c | grep " dataworks_version " | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+(-[a-zA-Z0-9]+)?'`"'" >> $@
	echo "pkgdesc='Database System'" >> $@
	echo "arch=('i686' 'x86_64')" >> $@
	echo "license=('BSD')" >> $@
	echo "pkgrel='1'" >> $@
	echo "makedepends=('byacc')" >> $@
	echo "source=('dataworks::svn+http://sw.nishi.boats/svn/nishi-dataworks/trunk#revision="`svn info | grep "Revision" | grep -Eo "[0-9]+"`"')" >> $@
	echo "sha256sums=('SKIP')" >> $@
	echo >> $@
	echo "build() {" >> $@
	echo "	cd dataworks" >> $@
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
