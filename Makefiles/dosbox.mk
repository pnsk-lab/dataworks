# $Id$

.PHONY: dosbox prepare-dosbox dosbox-x cleanup-dosbox

prepare-dosbox: no-doc
	echo 'create_table("test", "string:key", "floating:value");' > op.txt
	echo '.tables' >> op.txt
	echo "[serial]" > dosbox.conf
	echo "serial1=modem listenport:4096" >> dosbox.conf
	echo "[sdl]" >> dosbox.conf
	echo "output=openglpp" >> dosbox.conf
	echo "windowresolution=640x400" >> dosbox.conf
	echo "[render]" >> dosbox.conf
	echo "aspect=true" >> dosbox.conf
	echo "aspect_ratio=9:5" >> dosbox.conf
	echo "[cpu]" >> dosbox.conf
	echo "cycles=12000" >> dosbox.conf
	echo "[autoexec]" >> dosbox.conf
	echo "mount c: ." >> dosbox.conf
	echo "c:" >> dosbox.conf
	echo "copy Server\*$(EXEC_SUFFIX) dwserv$(EXEC_SUFFIX)" >> dosbox.conf
	echo "copy Client\*$(EXEC_SUFFIX) dw$(EXEC_SUFFIX)" >> dosbox.conf
	echo "dwserv -p COM1 db.dwf" >> dosbox.conf
	echo "dw /NC /f op.txt /create db.dwf" >> dosbox.conf
	echo "dw /NC /f op.txt db.dwf" >> dosbox.conf
	echo "pause" >> dosbox.conf
	echo "del db.dwf" >> dosbox.conf
	echo "del dw.exe" >> dosbox.conf
	echo "del dwserv.exe" >> dosbox.conf
	echo "exit" >> dosbox.conf
	if [ ! -e "dos4gw.exe" ]; then wget "https://github.com/yetmorecode/dos32a-ng/releases/download/9.1.2/DOS32ANG.EXE" -O dos4gw.exe ; fi

dosbox: prepare-dosbox
	dosbox
	$(MAKE) cleanup-dosbox

dosbox-x: prepare-dosbox
	dosbox-x -conf dosbox.conf
	$(MAKE) cleanup-dosbox

cleanup-dosbox:
	rm -f op.txt dosbox.conf DW.* DWSERV.* dw.* dwserv.* dosbox.core
