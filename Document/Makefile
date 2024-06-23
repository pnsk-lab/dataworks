# $Id$

.PHONY: all clean document document-jp
all: document README.DOC

document:
	doxygen

document-jp:
	doxygen Doxyfile-JP

README.DOC: ../Tool/docfmt README.FMT
	../Tool/docfmt README.FMT > README.DOC

clean:
	rm -rf doc README.DOC
