# $Id$

.PHONY: dos-installer

dos-installer:
	if [ ! "$(FORMAT)" = "NO" ]; then $(MAKE) PLATFORM=dos clean ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then $(MAKE) PLATFORM=dos no-doc ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then rm -f install.img ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mformat -C -f $(FLOPPY_SIZE) -v DWINST -i install.img :: ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img Client/*.exe ::dw.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img Server/*.exe ::dwserv.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img RemoteClient/*.exe ::dwrcli.exe ; fi
	$(MAKE) PLATFORM=dos TARGET=clean ./Installer
	$(MAKE) PLATFORM=dos ./Installer
	mcopy -oi install.img Installer/*.exe ::install.exe
	$(MAKE) PLATFORM=dos TARGET=clean ./Installer
	$(MAKE) PLATFORM=dos INDEP=jp INDEP_TO=cp932 ./Installer
	$(MAKE) ./Tool
	$(MAKE) -C ./Document README.DOC
	mcopy -toi install.img Document/README.DOC ::
	mcopy -oi install.img Installer/*.exe ::instajp.exe
	if [ ! -e lha.exe ]; then wget http://f.nishi.boats/f/g/lha.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mcopy -i install.img lha.exe ::lha.exe ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then mkdir -p install-workdir ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "[cpu]" > install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "cycles=30000" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "[autoexec]" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "imgmount -t floppy a: ./install.img" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "a:" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "mount c: ./install-workdir" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "lha a C:\EXTRACT.LZH dw.exe dwserv.exe dwrcli.exe readme.doc" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del dw.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del dwserv.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del dwrcli.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "c:" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "a:\lha s extract.lzh" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "a:" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "copy C:\EXTRACT.EXE" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "del lha.exe" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then echo "exit" >> install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then dosbox -conf install-dosbox.conf ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then rm -rf install-workdir ; fi
	if [ ! "$(FORMAT)" = "NO" ]; then rm -f install-dosbox.conf ; fi
	$(MAKE) clean
