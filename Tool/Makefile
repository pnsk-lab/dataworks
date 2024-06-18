# $Id$

.SUFFIXES: .c .o
.PHONY: all clean

all: docfmt test

docfmt: docfmt.o
	cc -std=c99 -o $@ docfmt.o

test: test.o
	cc -std=c99 -o $@ test.o

.c.o:
	cc -D_DEFAULT_SOURCE -std=c99 -c -o $@ $<

clean:
	rm -f docfmt *.o test
