# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = client.o

all: dataworks$(EXEC_SUFFIX)

dataworks$(EXEC_SUFFIX): $(OBJS) $(LINK_LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(LINK_LIB)

dataworks.res: dataworks.rc ../Binary/dataworks.ico
	$(WINDRES) ./dataworks.rc -O coff -o ./dataworks.res

.c.o:
	$(CC) -I../Library $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o dataworks *.exe *.com *.res
