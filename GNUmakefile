# $Id$
CC = cc
CFLAGS = -std=c99
LDFLAGS = -L`pwd`/Library
LIBS =
LIB_PREFIX = lib
LIB_SUFFIX = .so
EXEC_SUFFIX =

ifneq ("$(PLATFORM)", "")
include Platforms/$(PLATFORM).mk
endif

COMPILE_FLAGS = CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" LIB_PREFIX="$(LIB_PREFIX)" LIB_SUFFIX="$(LIB_SUFFIX)" EXEC_SUFFIX="$(EXEC_SUFFIX)"

.PHONY: all no-doc replace format clean ./Library ./Client ./Document

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
	$(MAKE) -C ./Library clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Client clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Document clean $(COMPILE_FLAGS)
