# $Id$

.PHONY: all clean document document-jp document-sr
all: document README.DOC

document:
	doxygen

document-jp:
	doxygen Doxyfile-JP

document-sr:
	doxygen Doxyfile-SR

README.DOC: ../Tool/docfmt README.FMT
	../Tool/docfmt README.FMT > README.DOC

clean:
	rm -rf doc README.DOC
