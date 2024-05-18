# $Id$

COMPILE_FLAGS = CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" LIB_PREFIX="$(LIB_PREFIX)" LIB_SUFFIX="$(LIB_SUFFIX)" EXEC_SUFFIX="$(EXEC_SUFFIX)" PLATFORM_M="$(PLATFORM_M)" PLATFORM_P="$(PLATFORM_P)"

.PHONY: all no-doc replace format clean ./Library ./Client ./Document archive

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

archive: no-doc
	rm -f *.zip *.tar.gz
	rm -rf dataworks-dist
	mkdir -p dataworks-dist
	mkdir -p dataworks-dist/Client
	mkdir -p dataworks-dist/Library
	cp Library/*$(LIB_SUFFIX) dataworks-dist/Library/
	cp Library/*.h dataworks-dist/Library/
	cp Client/dataworks$(EXEC_SUFFIX) dataworks-dist/Client/
	zip -rv dataworks.zip dataworks-dist
	tar czvf dataworks.tar.gz dataworks-dist
	rm -rf dataworks-dist
