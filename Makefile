# $Id$
CC = cc
CFLAGS = -std=c99
LDFLAGS =
LIBS =
LIB_PREFIX = lib
LIB_SUFFIX = .so

.if "$(PLATFORM)" != ""
.include "Platforms/$(PLATFORM).mk"
.endif

COMPILE_FLAGS = CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" LIB_PREFIX="$(LIB_PREFIX)" LIB_SUFFIX="$(LIB_SUFFIX)"

.PHONY: all replace format clean ./Library

all: ./Library

./Library::
	$(MAKE) -C $@ $(COMPILE_FLAGS)

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
