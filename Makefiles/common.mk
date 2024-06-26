# $Id$

.PHONY: all no-doc replace format clean ./Library ./Client ./Document ./Server ./Grammar ./RemoteClient ./Installer ./Tool get-version thanks-banner dos-installer

all: ./Tool ./Grammar ./Library ./Client $(SERVER) $(RCLI) ./Document
	@$(MAKE) thanks-banner

no-doc: ./Tool ./Grammar ./Library ./Client $(SERVER) $(RCLI)
	@$(MAKE) thanks-banner

thanks-banner:
	@echo "+-----------------------------------------+"
	@echo "|                                         |"
	@echo "|  Thanks for choosing DataWorks.         |"
	@echo "|            - Nishi <nishi@nishi.boats>  |"
	@echo "|                                         |"
	@echo "+-----------------------------------------+"

./Grammar::
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Library:: ./Grammar
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Client:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Server:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./RemoteClient:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Installer:: ./Library
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Tool::
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

./Document:: ./Tool
	$(MAKE) -C $@ $(COMPILE_FLAGS) $(TARGET)

FILES = `find . -name "*.c" -or -name "*.h"`

replace:
	for i in $(FILES) ./Grammar/dw.y ./Grammar/dw.l; do \
                echo -n "$$i ... "; \
                perl ./Tool/replace.pl < $$i > $$i.new; \
                mv $$i.new $$i; \
                echo "done"; \
        done

format:
	clang-format --verbose -i $(FILES)

clean:
	$(MAKE) -C ./Grammar clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Library clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Client clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Server clean $(COMPILE_FLAGS)
	$(MAKE) -C ./RemoteClient clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Installer clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Document clean $(COMPILE_FLAGS)
	$(MAKE) -C ./Tool clean $(COMPILE_FLAGS)

get-version:
	@echo $(VERSION)

include Makefiles/archive.mk
include Makefiles/dosbox.mk
include Makefiles/package.mk
include Makefiles/test.mk
include Makefiles/installer.mk
