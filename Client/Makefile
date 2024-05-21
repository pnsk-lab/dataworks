# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = main.o

all: dataworks$(EXEC_SUFFIX)

dataworks$(EXEC_SUFFIX): $(OBJS) ../Library/*.o
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)  ../Library/*.o

.c.o:
	$(CC) -I../Library $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o dataworks *.exe *.dos *.dos4g
