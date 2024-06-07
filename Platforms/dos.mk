# $Id$

WATCOM_PLATFORM=dos
include Platforms/watcom.mk
EXEC_SUFFIX=.exe
STACK_SIZE = 8192
CFLAGS += -march=i86 -mstack-size=$(STACK_SIZE)
LDFLAGS += -march=i86 -mstack-size=$(STACK_SIZE)
SERVER_MODULE = hayes.o
