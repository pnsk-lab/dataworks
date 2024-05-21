# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = parser.o database.o util.o dataworks.o

all: $(LIB_PREFIX)dataworks$(LIB_SUFFIX) $(STATICLIB_PREFIX)dataworks$(STATICLIB_SUFFIX)

$(LIB_PREFIX)dataworks$(LIB_SUFFIX): $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(STATICLIB_PREFIX)dataworks$(STATICLIB_SUFFIX): $(OBJS)
	$(AR) rcs $@ $(OBJS)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -DPLATFORM_M=\"$(PLATFORM_M)\" -DPLATFORM_P=\"$(PLATFORM_P)\" -fPIC -c -o $@ $<

clean:
	rm -f *.o *.so *.dll *.a *.lib
