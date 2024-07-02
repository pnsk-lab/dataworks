# $Id$

.PHONY: english japanese dosv clean

english: resource-en.c
	cp resource-en.c resource.c

japanese: resource-jp.c
	cp resource-jp.c resource.c

dosv: resource-jp.c
	iconv -f utf-8 -t sjis < resource-jp.c > resource.c

clean:
	rm -f resource.c
