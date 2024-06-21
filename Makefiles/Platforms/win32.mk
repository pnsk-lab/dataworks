# $Id$
# PLATFORM: Windows 32-bit

CC = i686-w64-mingw32-gcc
SHCC = i686-w64-mingw32-gcc
AR = i686-w64-mingw32-ar
RANLIB = i686-w64-mingw32-ranlib
WINDRES = i686-w64-mingw32-windres
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .exe
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = i686
PLATFORM_P = i686
LDFLAGS += -Wl,--out-implib,./dataworks.lib
LINK_LIB += dataworks.res
SERVLINK_LIB += -lws2_32
RCLILINK_LIB += -lws2_32
TO = PROGRAMFILES32
