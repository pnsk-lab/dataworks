# $Id$

CC = owcc
SHCC = true
AR = wlib
AR_ARGS = -b -n -fo \$$@
RANLIB = true
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .$(WATCOM_PLATFORM)
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = $(WATCOM_PLATFORM)
PLATFORM_P = $(WATCOM_PLATFORM)
CFLAGS += -b $(WATCOM_PLATFORM)
LDFLAGS += -b $(WATCOM_PLATFORM)