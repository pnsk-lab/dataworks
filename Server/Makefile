# $Id$

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = server.o $(SERVER_MODULE)

all: dataworks_server$(EXEC_SUFFIX)

dataworks_server$(EXEC_SUFFIX): $(OBJS) $(LINK_LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(LINK_LIB) $(SERVLINK_LIB)

dataworks.res: ../Client/dataworks.rc ../dataworks.ico
	$(WINDRES) ../Client/dataworks.rc -O coff -o ./dataworks.res

.c.o:
	$(CC) $(CFLAGS) -I../Library -c -o $@ $<

clean:
	rm -f *.o *.so *.dll *.exe *.com dataworks_server *.a *.lib *.res
