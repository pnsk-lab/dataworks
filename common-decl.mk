# $Id$

CC = cc
CFLAGS = -std=c99 -D_DEFAULT_SOURCE
LDFLAGS = -L`pwd`/Library
LIBS =
LIB_PREFIX = lib
LIB_SUFFIX = .so
EXEC_SUFFIX =
PLATFORM_M = `uname -m`
PLATFORM_P = `uname -p`
