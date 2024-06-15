# $Id$

.PHONY: all clean document
all: document README.DOC

document:
	doxygen

README.DOC: README.FMT
	../Tool/docfmt README.FMT

clean:
	rm -rf doc
