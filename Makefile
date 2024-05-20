# $Id$

include common-decl.mk

.if "$(PLATFORM)" != ""
include Platforms/$(PLATFORM).mk
.endif

include common.mk
