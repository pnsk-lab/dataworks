# $Id$

WATCOM_PLATFORM=os2
include Makefiles/Platforms/watcom.mk
EXEC_SUFFIX=.exe
STACK_SIZE = 8192
RCLI=
SERVER=
CFLAGS += -march=i86 -mstack-size=$(STACK_SIZE)
LDFLAGS += -march=i86 -mstack-size=$(STACK_SIZE)
SERVER_MODULE = hayes.o
RCLI_MODULE = hayes.o
