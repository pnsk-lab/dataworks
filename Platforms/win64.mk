# $Id$

CC = x86_64-w64-mingw32-gcc
SHCC = x86_64-w64-mingw32-gcc
AR = x86_64-w64-mingw32-ar
RANLIB = x86_64-w64-mingw32-ranlib
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .exe
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = x86_64
PLATFORM_P = x86_64
LDFLAGS += -Wl,--out-implib,./dataworks.lib
