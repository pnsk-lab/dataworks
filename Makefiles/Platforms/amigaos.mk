# $Id$
# PLATFORM: AmigaOS

VBCC_PLATFORM=kick13
include Makefiles/Platforms/vbcc.mk
LIBS = -lmsoft
EXEC_SUFFIX=
RCLI=
SERVER=
SERVER_MODULE = hayes.o
RCLI_MODULE = hayes.o
CFLAGS += -DAMIGA
