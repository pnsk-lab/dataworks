/* $Id$ */
/* --- START LICENSE --- */
/* -------------------------------------------------------------------------- */
/*                                                   DataWorks - Simple DBMS  */
/* -------------------------------------------------------------------------- */
/* Copyright (c) 2024 Crabware.                                               */
/* Redistribution and use in source and binary forms, with or without modific */
/* ation, are permitted provided that the following conditions are met:       */
/*     1. Redistributions of source code must retain the above copyright noti */
/* ce, this list of conditions and the following disclaimer.                  */
/*     2. Redistributions in binary form must reproduce the above copyright n */
/* otice, this list of conditions and the following disclaimer in the documen */
/* tation and/or other materials provided with the distribution.              */
/*     3. Neither the name of the copyright holder nor the names of its contr */
/* ibutors may be used to endorse or promote products derived from this softw */
/* are without specific prior written permission.                             */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS */
/* " AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, TH */
/* E IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPO */
/* SE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS  */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CON */
/* SEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITU */
/* TE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPT */
/* ION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, S */
/* TRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN AN */
/* Y WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY  */
/* OF SUCH DAMAGE.                                                            */
/* -------------------------------------------------------------------------- */
/* --- END LICENSE --- */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dw_util.h>

#include <bios.h>
#include <conio.h>
#include <i86.h>

extern int argc;
extern char** argv;

int port = -1;
bool connected;

bool option(const char* str, const char* shortopt, const char* longopt);

void disconnect(int sock);

int get_ioport() {
#ifdef PC98
	if(port == 0) return 0x30;
#else
	if(port == 0) {
		return 0x3f8;
	} else if(port == 1) {
		return 0x2f8;
	} else if(port == 2) {
		return 0x3e8;
	} else if(port == 3) {
		return 0x2e8;
	}
#endif
	return 0;
}

void write_serial(const char* str) {
	const char* ptr = str;
	while(1) {
		if(inp(get_ioport() + 5) & 0x20) {
			outp(get_ioport(), *ptr);
			ptr++;
			if(*ptr == 0) break;
		}
	}
}

char* modem_response(void) {
	char* buf = malloc(1);
	buf[0] = 0;
	char cbuf[2];
	cbuf[1] = 0;
	while(1) {
		if(inp(get_ioport() + 5) & 1) {
			unsigned short b = inp(get_ioport());
			char ch = b & 0xff;
			if(ch != '\r' && ch != '\n' && ch != 0) {
				cbuf[0] = ch;
				char* tmp = buf;
				buf = __dw_strcat(tmp, cbuf);
				free(tmp);
			} else if(ch == '\r') {
				if(strlen(buf) != 0) break;
			}
		}
	}
	return buf;
}

int rcli_init(void) {
	printf("Using Hayes Modem\n");
	connected = false;
	int i;
	char* dial = NULL;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '/' || argv[i][0] == '-') {
			if(option(argv[i], "p", "port")) {
				i++;
				if(__dw_strcaseequ(argv[i], "COM1")) {
					port = 0;
#ifndef PC98
				} else if(__dw_strcaseequ(argv[i], "COM2")) {
					port = 1;
				} else if(__dw_strcaseequ(argv[i], "COM3")) {
					port = 2;
				} else if(__dw_strcaseequ(argv[i], "COM4")) {
					port = 3;
#endif
				} else {
					fprintf(stderr, "Invalid port: %s\n", argv[i]);
					return 1;
				}
			} else if(option(argv[i], "h", "help")) {
				printf("\n");
				printf("Usage: %s [options] dial\n", argv[0]);
				printf("You can use double-dash or slash for long-format flag, and single-dash or slash for short-foramt flag.\n");
				printf("Options:\n");
				printf("\t-p --port [comport]   Specify the modem port\n");
				exit(0);
			} else {
				fprintf(stderr, "Invalid option: %s\n", argv[i]);
				return 1;
			}
		} else {
			if(dial != NULL) free(dial);
			dial = __dw_strdup(argv[i]);
		}
	}
	if(port == -1) {
		fprintf(stderr, "Specify serial port\n");
		return 1;
	}
	if(dial == NULL) {
		fprintf(stderr, "Specify where to dial\n");
		return 1;
	}
	_bios_serialcom(_COM_INIT, port, _COM_9600 | _COM_NOPARITY | _COM_CHR8 | _COM_STOP1);
	write_serial("AT&FE0F1\r");
	char* resp = modem_response();
	bool echo = __dw_strcaseequ(resp, "AT&FE0F1") || __dw_strcaseequ(resp, "NO CARRIER");
	if(resp != NULL && echo) free(resp); /* Kill echo */
	if(resp == NULL) return 1;
	if(echo) resp = modem_response();
	if(resp == NULL) return 1;
	if(__dw_strcaseequ(resp, "OK")) {
		printf("Modem ready\n");
		printf("Dialing\n");
		write_serial("ATDT");
		write_serial(dial);
		write_serial("\r");
		while(true) {
			resp = modem_response();
			int i;
			for(i = 0; resp[i] != 0; i++) {
				if(resp[i] == ' ') {
					resp[i] = 0;
					break;
				}
			}
			if(__dw_strcaseequ(resp, "CONNECT")) {
				printf("Connected\n");
				break;
			}
		}
		return 0;
	}
	disconnect(0);
	return 1;
}

void disconnect(int sock) {
	while((inp(get_ioport() + 5) & 0x20) == 0)
		;
	delay(100);
	while(inp(get_ioport() + 6) & (1 << 7)) outp(get_ioport() + 4, 0);
	connected = false;
}
