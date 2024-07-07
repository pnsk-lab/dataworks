# $Id$
# PLATFORM: Netware

WATCOM_PLATFORM=netware_clib_lite
include Makefiles/Platforms/watcom.mk
EXEC_SUFFIX=.exe
CFLAGS += -I$$NOVELLNDK/include/nlm
LDFLAGS +=
RCLI=
SERVER=
SERVER_MODULE = hayes.o
RCLI_MODULE = hayes.o
INSTALLER = ./Installer
