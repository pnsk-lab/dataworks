# $Id$
# PLATFORM: Windows 64-bit

CC = x86_64-w64-mingw32-gcc
SHCC = x86_64-w64-mingw32-gcc
AR = x86_64-w64-mingw32-ar
RANLIB = x86_64-w64-mingw32-ranlib
WINDRES = x86_64-w64-mingw32-windres
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .exe
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = x86_64
PLATFORM_P = x86_64
LDFLAGS += -Wl,--out-implib,./dataworks.lib
LINK_LIB += dataworks.res
SERVLINK_LIB += -lws2_32
RCLILINK_LIB += -lws2_32
TO = PROGRAMFILES64
CUSTOM_NSIS = StrCpy \$$InstDir "\$$PROGRAMFILES64\\\DataWorks"
