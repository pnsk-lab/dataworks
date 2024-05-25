# $Id$

COMPILE_FLAGS = CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" LIB_PREFIX="$(LIB_PREFIX)" LIB_SUFFIX="$(LIB_SUFFIX)" EXEC_SUFFIX="$(EXEC_SUFFIX)" PLATFORM_M="$(PLATFORM_M)" PLATFORM_P="$(PLATFORM_P)" STATICLIB_PREFIX="$(STATICLIB_PREFIX)" STATICLIB_SUFFIX="$(STATICLIB_SUFFIX)" AR="$(AR)" RANLIB="$(RANLIB)" SHCC="$(SHCC)" LINK_LIB="$(LINK_LIB)" AR_ARGS="$(AR_ARGS)"

.PHONY: all no-doc replace format clean ./Library ./Client ./Document archive archive-prepare archive-cleanup archive-targz archive-zip dosbox

all: ./Library ./Client ./Document

no-doc: ./Library ./Client

./Library::
	$(MAKE) -C $@ $(COMPILE_FLAGS)

./Client:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS)

./Document::
	$(MAKE) -C ./Document $(COMPILE_FLAGS)

FILES = `find . -name "*.c" -or -name "*.h"`

replace:
	for i in $(FILES); do \
                echo -n "$$i ... "; \
                perl replace.pl < $$i > $$i.new; \
                mv $$i.new $$i; \
                echo "done"; \
        done

format:
	clang-format -i $(FILES)

clean:
	rm -f *.zip *.tar.gz
	$(MAKE) -C ./Library clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Client clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Document clean $(COMPILE_FLAGS)

archive-prepare: all
	rm -f *.zip *.tar.gz
	rm -rf dataworks-dist
	mkdir -p dataworks-dist
	mkdir -p dataworks-dist/Client
	mkdir -p dataworks-dist/Library
	mkdir -p dataworks-dist/Document
	-cp Library/*$(LIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*$(STATICLIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*.lib dataworks-dist/Library/
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

dosbox: no-doc
	echo 'create_table("test", "string:key", "double:value");' > op.txt
	echo '.tables' >> op.txt
	echo "[cpu]" > dosbox.conf
	echo "cycles=12000" >> dosbox.conf
	echo "[autoexec]" >> dosbox.conf
	echo "mount c: ." >> dosbox.conf
	echo "c:" >> dosbox.conf
	echo "copy Client\*$(EXEC_SUFFIX) dw$(EXEC_SUFFIX)" >> dosbox.conf
	echo "dw -NC -f op.txt --create db.dwf" >> dosbox.conf
	echo "pause" >> dosbox.conf
	echo "del db.dwf" >> dosbox.conf
	echo "del dw.exe" >> dosbox.conf
	echo "exit" >> dosbox.conf
	if [ ! -e "dos4gw.exe" ]; then wget "https://github.com/yetmorecode/dos32a-ng/releases/download/9.1.2/DOS32ANG.EXE" -O dos4gw.exe ; fi
	dosbox
	rm op.txt dosbox.conf
