# $Id$

CC = owcc
SHCC = /bin/true
AR = wlib
AR_ARGS = -b -n -fag -o=\$$@
RANLIB = /bin/true
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .$(WATCOM_PLATFORM)
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = $(WATCOM_PLATFORM)
PLATFORM_P = $(WATCOM_PLATFORM)
CFLAGS += -DDOS -b $(WATCOM_PLATFORM)
LDFLAGS += -b $(WATCOM_PLATFORM)
LINK_LIB = `find Library -name "*.o"`
