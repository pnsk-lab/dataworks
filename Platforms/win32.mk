# $Id$

CC = i686-w64-mingw32-gcc
SHCC = i686-w64-mingw32-gcc
AR = i686-w64-mingw32-ar
RANLIB = i686-w64-mingw32-ranlib
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .exe
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = i686
PLATFORM_P = i686
LDFLAGS += -Wl,--out-implib,./dataworks.lib
