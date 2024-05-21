# $Id$

CC = cc
SHCC = cc
AR = ar
AR_ARGS = rcs \$$@
RANLIB = ranlib
CFLAGS = -std=c99 -D_DEFAULT_SOURCE
LDFLAGS = -L`pwd`/Library
LIBS =
LIB_PREFIX = lib
LIB_SUFFIX = .so
STATICLIB_PREFIX = lib
STATICLIB_SUFFIX = .a
EXEC_SUFFIX =
PLATFORM_M = `uname -m`
PLATFORM_P = `uname -p | sed -E "s/[ \\(\\)]/-/g"`
LINK_LIB = ../Library/$(STATICLIB_PREFIX)dataworks$(STATICLIB_SUFFIX)
