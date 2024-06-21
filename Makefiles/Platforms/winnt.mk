# $Id$
# PLATFORM: Windows NT

WATCOM_PLATFORM=nt
include Makefiles/Platforms/watcom.mk
CFLAGS += -I$$INCLUDE/nt
EXEC_SUFFIX=.exe
RCLI=
SERVLINK_LIB += $$WATCOM/lib386/nt/ws2_32.lib
RCLILINK_LIB += $$WATCOM/lib386/nt/ws2_32.lib
