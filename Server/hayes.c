/* $Id$ */
/* --- START LICENSE --- */
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

#include <dataworks.h>

#include <dw_database.h>
#include <dw_util.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

extern int argc;
extern char** argv;

#include <bios.h>
#include <i86.h>
#include <conio.h>

void protocol_init(int sock);
void protocol_loop(int sock);
void disconnect(int sock);

bool option(const char* str, const char* shortopt, const char* longopt);

int port = -1;

int get_ioport(){
#ifdef PC98
	if(port == 0) return 0x30;
#else
	if(port == 0){
		return 0x3f8;
	}else if(port == 1){
		return 0x2f8;
	}else if(port == 2){
		return 0x3e8;
	}else if(port == 3){
		return 0x2e8;
	}
#endif
	return 0;
}

void write_serial(const char* str) {
	const char* ptr = str;
	while(1) {
		if(_bios_serialcom(_COM_STATUS, port, 0) & 0x10) {
			_bios_serialcom(_COM_SEND, port, *ptr);
			ptr++;
			if(*ptr == 0) break;
		}
	}
}

bool connected;

extern sig_atomic_t signals;
extern struct dataworks_db* db;

char* modem_response(void) {
	char* buf = malloc(1);
	buf[0] = 0;
	char cbuf[2];
	cbuf[1] = 0;
	while(1) {
		if(_bios_serialcom(_COM_STATUS, port, 0) & 0x0100) {
			unsigned short b = _bios_serialcom(_COM_RECEIVE, port, 0);
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
		printf("\r");
		if(signals > 0){
			free(buf);
			if(connected){
				disconnect(0);
			}
			return NULL;
		}
	}
	return buf;
}

int server_init(void) {
	printf("Using Hayes Modem\n");
	connected = false;
	int i;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '/' || argv[i][0] == '-'){
			if(option(argv[i], "p", "port")){
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
			}else{
				fprintf(stderr, "Invalid option: %s\n", argv[i]);
				return 1;
			}
		}else{
			db = dataworks_database_open(argv[i]);
		}
	}
	if(port == -1) {
		fprintf(stderr, "Specify serial port\n");
		return 1;
	}
	printf("Serial port is at I/O 0x%4.4x\n", get_ioport());
	_bios_serialcom(_COM_INIT, port, _COM_9600 | _COM_NOPARITY | _COM_CHR8 | _COM_STOP1);
	write_serial("AT&FE0F1\r");
	char* resp = modem_response();
	bool echo = __dw_strcaseequ(resp, "AT&FE0F1");
	if(resp != NULL && echo) free(resp); /* Kill echo */
	if(resp == NULL) return 0;
	if(echo) resp = modem_response();
	if(resp == NULL) return 0;
	if(__dw_strcaseequ(resp, "OK")) {
		fprintf(stderr, "Modem initialization successful\n");
	} else {
		fprintf(stderr, "Modem initialization fail\n");
		free(resp);
		return 1;
	}
	free(resp);
	return 0;
}

void server_loop(void) {
	connected = false;
	while(1) {
		char* resp = modem_response();
		if(resp == NULL) break;
		if(__dw_strcaseequ(resp, "NO CARRIER")) {
			free(resp);
			printf("Disconnected\n");
			connected = false;
			continue;
		}
		int i;
		bool has_arg = false;
		for(i = 0; resp[i] != 0; i++) {
			if(resp[i] == ' ') {
				resp[i] = 0;
				has_arg = true;
				break;
			}
		}
		if(connected) {
		} else {
			if(__dw_strcaseequ(resp, "RING")) {
				printf("Got a call, answering\n");
				write_serial("ATA\r");
			} else if(__dw_strcaseequ(resp, "CONNECT")) {
				printf("Connection successful");
				if(has_arg) printf(", %s BPS", resp + i + 1);
				printf("\n");
				printf("Entering connected state\n");
				connected = true;
				protocol_init(0);
				protocol_loop(0);
			}
		}
		free(resp);
	}
}

char* readline_sock(int sock){
	return connected ? modem_response() : NULL;
}

void writeline(int sock, const char* str) {
	char* snd = __dw_strcat(str, "\r\n");
	write_serial(snd);
	free(snd);
}

void disconnect(int sock){
	while(inp(get_ioport() + 6) & (1 << 7)) outp(get_ioport() + 4, 0);
	connected = false;
}
