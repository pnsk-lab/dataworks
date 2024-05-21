# $Id$

WATCOM_PLATFORM=com
include Platforms/watcom.mk
EXEC_SUFFIX=.com
CFLAGS += -march=i86
LDFLAGS += -march=i86
