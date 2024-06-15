# $Id$

.PHONY: all clean document
all: document README.DOC

document:
	doxygen

README.DOC: ../Tool/docfmt README.FMT
	../Tool/docfmt README.FMT > README.DOC

clean:
	rm -rf doc README.DOC
