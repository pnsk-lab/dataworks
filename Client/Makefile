# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = main.o

all: dataworks$(EXEC_SUFFIX)

dataworks$(EXEC_SUFFIX): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) -ldataworks

.c.o:
	$(CC) -I../Library $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o dataworks *.exe
