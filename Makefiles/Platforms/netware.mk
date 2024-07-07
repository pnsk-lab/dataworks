# $Id$
# PLATFORM: Netware

WATCOM_PLATFORM=netware_clib_lite
include Makefiles/Platforms/watcom.mk
EXEC_SUFFIX=.exe
CFLAGS += -I$$NOVELLNDK/include/nlm
LDFLAGS +=
SERVER=
RCLI=
INSTALLER = ./Installer
