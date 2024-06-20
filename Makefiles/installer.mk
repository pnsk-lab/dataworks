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
	echo "!define MUI_WELCOMEFINISHPAGE_BITMAP Binary/nsis.bmp" >> install.nsi
	echo "!include nsDialogs.nsh" >> install.nsi
	echo "!include LogicLib.nsh" >> install.nsi
	echo "!include x64.nsh" >> install.nsi
	echo "Name \"DataWorks\"" >> install.nsi
	echo "OutFile \"$@\"" >> install.nsi
	echo "SetCompressor lzma" >> install.nsi
	echo "InstallDir \"\$$$(TO)\\DataWorks\"" >> install.nsi
	echo "XPStyle on" >> install.nsi
	echo "!define MUI_LICENSEPAGE_RADIOBUTTONS" >> install.nsi
	echo "!insertmacro MUI_RESERVEFILE_LANGDLL" >> instal.nsi
	echo "!insertmacro MUI_PAGE_WELCOME" >> install.nsi
	echo "!insertmacro MUI_PAGE_LICENSE LICENSE" >> install.nsi
	echo "!insertmacro MUI_PAGE_DIRECTORY" >> install.nsi
	echo "!insertmacro MUI_PAGE_COMPONENTS" >> install.nsi
	echo "!insertmacro MUI_PAGE_INSTFILES" >> install.nsi
	echo "!insertmacro MUI_PAGE_FINISH" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_WELCOME" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_CONFIRM" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_INSTFILES" >> install.nsi
	echo "!insertmacro MUI_UNPAGE_FINISH" >> install.nsi
	echo "!insertmacro MUI_LANGUAGE \"English\"" >> install.nsi
	echo "!insertmacro MUI_LANGUAGE \"Japanese\"" >> install.nsi
	echo "LangString APP_SET_DESC \$${LANG_ENGLISH} \"Application set\"" >> install.nsi
	echo "LangString APP_SET_DESC \$${LANG_JAPANESE} \"アプリケーションセット\"" >> install.nsi
	echo "LangString CLIENT_SET_DESC \$${LANG_ENGLISH} \"Client\"" >> install.nsi
	echo "LangString CLIENT_SET_DESC \$${LANG_JAPANESE} \"クライアント\"" >> install.nsi
	echo "LangString SERVER_SET_DESC \$${LANG_ENGLISH} \"Server\"" >> install.nsi
	echo "LangString SERVER_SET_DESC \$${LANG_JAPANESE} \"サーバー\"" >> install.nsi
	echo "LangString RCLI_SET_DESC \$${LANG_ENGLISH} \"Remote Client\"" >> install.nsi
	echo "LangString RCLI_SET_DESC \$${LANG_JAPANESE} \"リモートクライアント\"" >> install.nsi
	echo "LangString DEV_SET_DESC \$${LANG_ENGLISH} \"Development set\"" >> install.nsi
	echo "LangString DEV_SET_DESC \$${LANG_JAPANESE} \"開発用セット\"" >> install.nsi
	echo "LangString DLL_SET_DESC \$${LANG_ENGLISH} \"Library\"" >> install.nsi
	echo "LangString DLL_SET_DESC \$${LANG_JAPANESE} \"ライブラリ\"" >> install.nsi
	echo "LangString HEADERS_SET_DESC \$${LANG_ENGLISH} \"Headers\"" >> install.nsi
	echo "LangString HEADERS_SET_DESC \$${LANG_JAPANESE} \"ヘッダー\"" >> install.nsi
	echo "Function .onInit" >> install.nsi
	echo "  !insertmacro MUI_LANGDLL_DISPLAY" >> install.nsi
	echo "FunctionEnd" >> install.nsi
	echo "Section -Pre" >> install.nsi
	echo "  CreateDirectory \"\$$SMPROGRAMS\\\DataWorks\"" >> install.nsi
	echo "SectionEnd" >> install.nsi
	echo "SectionGroup /e \"\$$(APP_SET_DESC)\" APP_SET" >> install.nsi
	echo "  Section -PreApp" >> install.nsi
	echo "    CreateDirectory \"\$$INSTDIR\\\bin\"" >> install.nsi
	echo "  SectionEnd" >> install.nsi
	echo "  Section \"\$$(CLIENT_SET_DESC)\" CLIENT_SET" >> install.nsi
	echo "    SetOutPath \"\$$INSTDIR\\\bin\"" >> install.nsi
	echo "    File \"Client/dataworks.exe\"" >> install.nsi
	echo "    CreateShortcut \"\$$SMPROGRAMS\\\DataWorks\\\DataWorks Client.lnk\" \"\$$INSTDIR\\\bin\\\dataworks.exe\" \"\"" >> install.nsi
	echo "  SectionEnd" >> install.nsi
	echo "  Section \"\$$(SERVER_SET_DESC)\" SERVER_SET" >> install.nsi
	echo "    SetOutPath \"\$$INSTDIR\\\bin\"" >> install.nsi
	echo "    File \"Server/dataworks_server.exe\"" >> install.nsi
	echo "    CreateShortcut \"\$$SMPROGRAMS\\\DataWorks\\\DataWorks Server.lnk\" \"\$$INSTDIR\\\bin\\\dataworks_server.exe\" \"\"" >> install.nsi
	echo "  SectionEnd" >> install.nsi
	echo "  Section \"\$$(RCLI_SET_DESC)\" RCLI_SET" >> install.nsi
	echo "    SetOutPath \"\$$INSTDIR\\\bin\"" >> install.nsi
	echo "    File \"RemoteClient/dataworks_remote_client.exe\"" >> install.nsi
	echo "    CreateShortcut \"\$$SMPROGRAMS\\\DataWorks\\\DataWorks Remote Client.lnk\" \"\$$INSTDIR\\\bin\\\dataworks_remote_client.exe\" \"\"" >> install.nsi
	echo "  SectionEnd" >> install.nsi
	echo "SectionGroupEnd" >> install.nsi
	echo "SectionGroup /e \"\$$(DEV_SET_DESC)\" DEV_SET" >> install.nsi
	echo "  Section /o \"\$$(DLL_SET_DESC)\" DLL_SET" >> install.nsi
	echo "    CreateDirectory \"\$$INSTDIR\\\lib\"" >> install.nsi
	echo "    SetOutPath \"\$$INSTDIR\\\lib\"" >> install.nsi
	echo "    File \"Library/dataworks.dll\"" >> install.nsi
	echo "    File \"Library/dataworks.lib\"" >> install.nsi
	echo "  SectionEnd" >> install.nsi
	echo "  Section /o \"\$$(HEADERS_SET_DESC)\" HEADERS_SET" >> install.nsi
	echo "    CreateDirectory \"\$$INSTDIR\\\include\"" >> install.nsi
	echo "    SetOutPath \"\$$INSTDIR\\\include\"" >> install.nsi
	for i in `find Library -name "*.h"`; do \
		echo "    File \"$$i\"" >> install.nsi; \
        done
	echo "  SectionEnd" >> install.nsi
	echo "SectionGroupEnd" >> install.nsi
	echo "Section -Post" >> install.nsi
	echo "  WriteUninstaller \"\$$INSTDIR\\Uninstall.exe\"" >> install.nsi
	echo "  \$${If} \$${RunningX64}" >> install.nsi
	echo "    SetRegView 64" >> install.nsi
	echo "  \$${Else}" >> install.nsi
	echo "    SetRegView 32" >> install.nsi
	echo "  \$${EndIf}" >> install.nsi
	echo "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\" \"DisplayIcon\" \"\$$INSTDIR\\dataworks.exe\"" >> install.nsi
	echo "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\" \"DisplayName\" \"DataWorks\"" >> install.nsi
	echo "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\" \"UninstallString\" \"\$$INSTDIR\\Uninstall.exe\"" >> install.nsi
	echo "SectionEnd" >> install.nsi
	echo "Section Uninstall" >> install.nsi
	echo "  SetAutoClose false" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\bin\\\dataworks.exe\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\bin\\\dataworks_server.exe\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\bin\\\dataworks_remote_client.exe\"" >> install.nsi
	echo "  RMDir \"\$$INSTDIR\\\bin\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\lib\\\dataworks.dll\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\lib\\\dataworks.lib\"" >> install.nsi
	echo "  RMDir \"\$$INSTDIR\\\lib\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\include\\\*.h\"" >> install.nsi
	echo "  RMDir \"\$$INSTDIR\\\include\"" >> install.nsi
	echo "  Delete \"\$$INSTDIR\\\Uninstall.exe\"" >> install.nsi
	echo "  RMDir \"\$$INSTDIR\"" >> install.nsi
	echo "  DeleteRegKey HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DataWorks\"" >> install.nsi
	echo "SectionEnd" >> install.nsi
	echo "!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${APP_SET} \"\$$(APP_SET_DESC)\"" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${SERVER_SET} \"\$$(SERVER_SET_DESC)\"" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${CLIENT_SET} \"\$$(CLIENT_SET_DESC)\"" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${RCLI_SET} \"\$$(RCLI_SET_DESC)\"" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${DEV_SET} \"\$$(DEV_SET_DESC)\"" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${DLL_SET} \"\$$(DLL_SET_DESC)\"" >> install.nsi
	echo "  !insertmacro MUI_DESCRIPTION_TEXT \$${HEADERS_SET} \"\$$(HEADERS_SET_DESC)\"" >> install.nsi
	echo "!insertmacro MUI_FUNCTION_DESCRIPTION_END" >> install.nsi
	cat install.nsi
	makensis install.nsi
	rm install.nsi dataworks.bmp
