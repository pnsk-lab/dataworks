# $Id: Makefile 251 2024-06-12 05:57:04Z nishi $

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = rcli.o $(RCLI_MODULE)

all: dataworks_server$(EXEC_SUFFIX)

dataworks_remote_client$(EXEC_SUFFIX): $(OBJS) $(LINK_LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(LINK_LIB) $(RCLILINK_LIB)

dataworks.res: ../Client/dataworks.rc ../Binary/dataworks.ico
	$(WINDRES) ../Client/dataworks.rc -O coff -o ./dataworks.res

.c.o:
	$(CC) $(CFLAGS) -I../Library -c -o $@ $<

clean:
	rm -f *.o *.so *.dll *.exe *.com dataworks_remote_client *.a *.lib *.res
