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

#include <dw_util.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int argc;
char** argv;

#ifdef __DOS__
#include <bios.h>

/* Why would you run the database server on DOS, tho ... */
#define HAYES
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#define TCPIP
#endif

void protocol_init(int sock);

#ifdef HAYES
int port = -1;

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

char* modem_response(void) {
	char* buf = malloc(513);
	buf[0] = 0;
	int count = 0;
	while(1) {
		if(_bios_serialcom(_COM_STATUS, port, 0) & 0x0100) {
			unsigned short b = _bios_serialcom(_COM_RECEIVE, port, 0);
			char ch = b & 0xff;
			if(ch != '\r' && ch != '\n' && ch != 0) {
				buf[count] = ch;
				buf[count + 1] = 0;
				count++;
			} else if(ch == '\r') {
				if(strlen(buf) != 0) break;
			}
		}
	}
	return buf;
}

int server_init(void) {
	printf("Using Hayes Modem\n");
	int i;
	for(i = 1; i < argc; i++) {
		if(__dw_strcaseequ(argv[i], "COM1")) {
			port = 0;
		} else if(__dw_strcaseequ(argv[i], "COM2")) {
			port = 1;
		} else if(__dw_strcaseequ(argv[i], "COM3")) {
			port = 2;
		} else if(__dw_strcaseequ(argv[i], "COM4")) {
			port = 3;
		} else {
			fprintf(stderr, "Invalid port: %s\n", argv[i]);
			return 1;
		}
	}
	if(port == -1) {
		fprintf(stderr, "Specify serial port\n");
		return 1;
	}
	_bios_serialcom(_COM_INIT, port, _COM_9600 | _COM_NOPARITY | _COM_CHR8 | _COM_STOP1);
	write_serial("AT&FE0F1\r");
	free(modem_response()); /* Kill echo */
	char* resp = modem_response();
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
	bool connected = false;
	while(1) {
		char* resp = modem_response();
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
			}
		}
		free(resp);
	}
}

void writeline(int sock, const char* str) {
	char* snd = __dw_strcat(str, "\r\n");
	write_serial(snd);
	free(snd);
}
#endif

#ifdef TCPIP
int port = 4096;
int server_socket;
struct sockaddr_in6 server_address;

int server_init(void) {
	printf("Using BSD TCP/IP\n");
	int i;
	for(i = 1; i < argc; i++) {
		port = atoi(argv[i]);
	}
	if((server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail\n");
		return 1;
	}
	int yes = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail\n");
		close(server_socket);
		return 1;
	}
	if(setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail\n");
		close(server_socket);
		return 1;
	}
	int no = 0;
	if(setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail\n");
		close(server_socket);
		return 1;
	}
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin6_family = AF_INET6;
	server_address.sin6_addr = in6addr_any;
	server_address.sin6_port = htons(port);
	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail\n");
		close(server_socket);
		return 1;
	}
	if(listen(server_socket, 128) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail\n");
		close(server_socket);
		return 1;
	}
	signal(SIGCHLD, SIG_IGN);
	fprintf(stderr, "BSD TCP/IP initialization successful\n");
	return 0;
}

void server_loop(void) {
	while(1) {
		struct sockaddr_in claddr;
		int clen = sizeof(claddr);
		int sock = accept(server_socket, (struct sockaddr*)&claddr, &clen);
		pid_t p = fork();
		if(p == 0) {
			protocol_init(sock);
			_exit(0);
		} else {
			close(sock);
		}
	}
}

void writeline(int sock, const char* str) {
	char* snd = __dw_strcat(str, "\r\n");
	send(sock, snd, strlen(snd), 0);
	free(snd);
}
#endif

void protocol_init(int sock) {
	/* sock is ignored on hayes */
	char* verstr = __dw_strcat("READY:VER=", dataworks_get_version());
	writeline(sock, verstr);
	writeline(sock, "ATTR:AUTH=YES");
}

int main(int _argc, char** _argv) {
	argc = _argc;
	argv = _argv;
	printf("DataWorks Server  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
	int st;
	if((st = server_init()) != 0) return st;
	server_loop();
}
