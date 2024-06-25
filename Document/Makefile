# $Id$

.PHONY: all clean document document-jp
all: document README.DOC

document:
	sed "s/@VERSION@/$(VERSION)/g" index.tmpl.md > index.md
	doxygen

document-jp:
	sed "s/@VERSION@/$(VERSION)/g" indexjp.tmpl.md > indexjp.md
	doxygen Doxyfile-JP

README.DOC: ../Tool/docfmt README.FMT
	../Tool/docfmt README.FMT > README.DOC

clean:
	rm -rf doc README.DOC index.md indexjp.md
