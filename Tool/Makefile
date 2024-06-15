# $Id$

.SUFFIXES: .c .o
.PHONY: all clean

all: docfmt

docfmt: docfmt.o
	cc -std=c99 -o $@ docfmt.o

.c.o:
	cc -std=c99 -c -o $@ $<

clean:
	rm -f docfmt *.o
