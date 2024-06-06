# $Id$

WATCOM_PLATFORM=dos
include Platforms/watcom.mk
EXEC_SUFFIX=.exe
CFLAGS += -march=i86 -mstack-size=8196
LDFLAGS += -march=i86 -mstack-size=8196
SERVER_MODULE = hayes.o
