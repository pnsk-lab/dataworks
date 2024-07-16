# $Id$

.PHONY: archive archive-prepare archive-cleanup archive-targz archive-zip

archive-prepare: no-doc
	rm -f dataworks.zip dataworks.tar.gz
	rm -rf dataworks-dist
	mkdir -p dataworks-dist
	mkdir -p dataworks-dist/RemoteClient
	mkdir -p dataworks-dist/Server
	mkdir -p dataworks-dist/Client
	mkdir -p dataworks-dist/Library
	-mkdir -p dataworks-dist/Document
	-cp Library/*$(LIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*$(STATICLIB_SUFFIX) dataworks-dist/Library/
	-cp Library/*.lib dataworks-dist/Library/
	-cp Server/dataworks_server$(EXEC_SUFFIX) dataworks-dist/Server/
	-rmdir dataworks-dist/Server
	-cp RemoteClient/dataworks_remote_client$(EXEC_SUFFIX) dataworks-dist/RemoteClient/
	-rmdir dataworks-dist/RemoteClient
	cp Library/*.h dataworks-dist/Library/
	cp Client/dataworks$(EXEC_SUFFIX) dataworks-dist/Client/
	-cp -rf Document/doc/html dataworks-dist/Document/html

archive-cleanup:
	rm -rf dataworks-dist

archive-targz:
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-prepare ; fi
	tar czvf dataworks.tar.gz dataworks-dist
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-cleanup ; fi

archive-zip:
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-prepare ; fi
	zip -rv dataworks.zip dataworks-dist
	if [ "$(PREP)" = "" ]; then $(MAKE) archive-cleanup ; fi

archive:
	$(MAKE) archive-prepare
	-$(MAKE) archive-targz PREP=NO
	-$(MAKE) archive-zip PREP=NO
	$(MAKE) archive-cleanup
