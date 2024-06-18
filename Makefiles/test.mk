# $Id$

.PHONY: test

test: no-doc
	./Tool/test -s "$(EXEC_SUFFIX)"
