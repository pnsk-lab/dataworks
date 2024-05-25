# $Id$

CC = owcc
SHCC = /bin/true
AR = wlib
AR_ARGS = -b -n -fo \$$@
RANLIB = /bin/true
LIB_PREFIX =
LIB_SUFFIX = .dll
EXEC_SUFFIX = .$(WATCOM_PLATFORM)
STATICLIB_PREFIX =
STATICLIB_SUFFIX = .a
PLATFORM_M = $(WATCOM_PLATFORM)
PLATFORM_P = $(WATCOM_PLATFORM)
CFLAGS += -DDOS -b $(WATCOM_PLATFORM)
LDFLAGS += -b $(WATCOM_PLATFORM)

dosbox: no-doc
	echo 'create_table("test", "string:key", "double:value");' > op.txt
	echo '.tables' >> op.txt
	echo "[cpu]" > dosbox.conf
	echo "cycles=12000" >> dosbox.conf
	echo "[autoexec]" >> dosbox.conf
	echo "mount c: ." >> dosbox.conf
	echo "c:" >> dosbox.conf
	echo "copy Client\*$(EXEC_SUFFIX) dw$(EXEC_SUFFIX)" >> dosbox.conf
	echo "dw -NC -f op.txt --create db.dwf" >> dosbox.conf
	echo "pause" >> dosbox.conf
	echo "del db.dwf" >> dosbox.conf
	echo "del dw.exe" >> dosbox.conf
	echo "exit" >> dosbox.conf
	if [ ! -e "dos4gw.exe" ]; then wget "https://github.com/yetmorecode/dos32a-ng/releases/download/9.1.2/DOS32ANG.EXE" -O dos4gw.exe ; fi
	dosbox
	rm op.txt dos4gw.exe dosbox.conf