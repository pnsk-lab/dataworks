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

./install.exe: no-doc
	convert -size 150x57 xc:white canvas.bmp
	convert Binary/dataworks-black.png dw.bmp
	convert canvas.bmp dw.bmp -geometry +97+5 -composite dataworks.bmp
	rm canvas.bmp dw.bmp
	echo "!include MUI2.nsh" > install.nsi
	echo "!define MUI_ABORTWARNING" >> install.nsi
	echo "!define MUI_ICON \"Binary/dataworks.ico\"" >> install.nsi
	echo "!define MUI_HEADERIMAGE" >> install.nsi
	echo "!define MUI_HEADERIMAGE_BITMAP \"dataworks.bmp\"" >> install.nsi
	echo "!define MUI_HEADERIMAGE_RIGHT" >> install.nsi
	echo "!include nsDialogs.nsh" >> install.nsi
	echo "!include LogicLib.nsh" >> install.nsi
	echo "!include x64.nsh" >> install.nsi
	echo "Name \"DataWorks\"" >> install.nsi
	echo "OutFile \"$@\"" >> install.nsi
	echo "SetCompressor lzma" >> install.nsi
	echo "InstallDir \"\$$$(TO)\\DataWorks\"" >> install.nsi
	echo "XPStyle on" >> install.nsi
	echo "!insertmacro MUI_PAGE_WELCOME" >> install.nsi
	echo "!insertmacro MUI_PAGE_DIRECTORY" >> install.nsi
	echo "!insertmacro MUI_PAGE_INSTFILES" >> install.nsi
	echo "!insertmacro MUI_PAGE_FINISH" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_WELCOME" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_CONFIRM" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_INSTFILES" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_FINISH" >> install.nsi
	echo "!insertmacro MUI_LANGUAGE \"English\"" >> install.nsi
	echo "Section" >> install.nsi
	echo "  SetOutPath \"\$$INSTDIR\"" >> install.nsi
	echo "  File \"Client/dataworks.exe\"" >> install.nsi
	echo "  File \"Server/dataworks_server.exe\"" >> install.nsi
	echo "  File \"RemoteClient/dataworks_remote_client.exe\"" >> install.nsi
	echo "  CreateDirectory \"\$$SMPROGRAMS\\DataWorks\"" >> install.nsi
	echo "  SetOutPath \"\$$INSTDIR\"" >> install.nsi
	echo "  CreateShortcut \"\$$SMPROGRAMS\\DataWorks\\DataWorks Client.lnk\" \"\$$INSTDIR\\dataworks.exe\" \"\"" >> install.nsi
	echo "  CreateShortcut \"\$$SMPROGRAMS\\DataWorks\\DataWorks Remote Client.lnk\" \"\$$INSTDIR\\dataworks_remote_client.exe\" \"\"" >> install.nsi
	echo "  CreateShortcut \"\$$SMPROGRAMS\\DataWorks\\DataWorks Server.lnk\" \"\$$INSTDIR\\dataworks_server.exe\" \"\"" >> install.nsi
	echo "  WriteUninstaller \"\$$INSTDIR\\Uninstall.exe\"" >> install.nsi
	echo "  \$${If} \$${RunningX64}" >> install.nsi
	echo "    SetRegView 64" >> install.nsi
	echo "  \$${Else}" >> install.nsi
	echo "    SetRegView 32" >> install.nsi
	echo "  \$${EndIf}" >> install.nsi
	echo "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\" \"DisplayIcon\" \"\$$INSTDIR\\dataworks.exew\""
	echo "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\" \"DisplayName\" \"Dataworks\""
	echo "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\" \"UninstallString\" \"\$$INSTDIR\\Uninstall.exe\""
	echo "SectionEnd" >> install.nsi
	echo "Section Uninstall" >> install.nsi
	echo "  SetAutoClose false" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\dataworks.exe\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\dataworks_server.exe\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\dataworks_remote_client.exe\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\Uninstall.exe\"" >> install.nsi
	echo "  RMDir \"\$$INSTDIR\"" >> install.nsi
	echo "SectionEnd" >> install.nsi
	makensis install.nsi
	rm install.nsi dataworks.bmp
