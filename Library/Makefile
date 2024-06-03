# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = parser.o database.o util.o dataworks.o database_table.o database_db.o database_record.o exec.o ../Grammar/dw.tab.o ../Grammar/dw.yy.o

all: $(LIB_PREFIX)dataworks$(LIB_SUFFIX) $(STATICLIB_PREFIX)dataworks$(STATICLIB_SUFFIX)

$(LIB_PREFIX)dataworks$(LIB_SUFFIX): $(OBJS)
	$(SHCC) -shared $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(STATICLIB_PREFIX)dataworks$(STATICLIB_SUFFIX): $(OBJS)
	$(AR) $(AR_ARGS) $(OBJS)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -DPLATFORM_M=\"$(PLATFORM_M)\" -DPLATFORM_P=\"$(PLATFORM_P)\" -fPIC -c -o $@ $<

clean:
	rm -f *.o *.so *.dll *.a *.lib
