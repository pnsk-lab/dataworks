# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = install.o

all: install$(EXEC_SUFFIX)

install$(EXEC_SUFFIX): indep.h $(OBJS)
	$(CC) $(LDFLAGS) $(PC98) -o $@ $(OBJS)

indep.h: indep-$(INDEP).h
	iconv -f utf-8 -t $(INDEP_TO) < indep-$(INDEP).h > $@

.c.o:
	$(CC) $(CFLAGS) $(PC98) -c -o $@ $<

clean:
	rm -f *.o install *.exe *.com *.res indep.h
