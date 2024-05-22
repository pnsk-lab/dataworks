# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = main.o

all: dataworks$(EXEC_SUFFIX)

dataworks$(EXEC_SUFFIX): $(OBJS) $(LINK_LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(LINK_LIB)

.c.o:
	$(CC) -I../Library $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o dataworks *.exe *.dos *.dos4g *.com
