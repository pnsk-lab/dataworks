# $Id$

include Makefiles/common-decl.mk

.if "$(PLATFORM)" != ""
include Makefiles/Platforms/$(PLATFORM).mk
.endif

include Makefiles/common.mk
