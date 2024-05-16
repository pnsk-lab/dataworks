# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = parser.o util.o dataworks.o

all: $(LIB_PREFIX)dataworks$(LIB_SUFFIX)

$(LIB_PREFIX)dataworks$(LIB_SUFFIX): $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f *.o *.so *.dll
