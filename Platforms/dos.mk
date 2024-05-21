# $Id$

WATCOM_PLATFORM=dos
include Platforms/watcom.mk
EXEC_SUFFIX=.exe
CFLAGS += -march=i86
LDFLAGS += -march=i86
