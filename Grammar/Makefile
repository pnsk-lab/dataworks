# $Id$

.SUFFIXES: .c .o
.PHONY: all clean

all: dw.tab.o dw.yy.o

.c.o:
	$(CC) $(CFLAGS) $(PIC) -c -o $@ $<

dw.tab.c: dw.y
	$(YACC) $(YFLAGS) -o $@ dw.y

dw.yy.c: dw.l
	$(LEX) $(LFLAGS) -o $@ dw.l

clean:
	rm -f *.o dw.tab.c dw.tab.h dw.yy.c
