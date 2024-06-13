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

#ifdef __MINGW32__
#include <windows.h>
#include <winsock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#endif

#include <unistd.h>

#ifndef __MINGW32__
#define ADDRINFO struct addrinfo
#endif

#ifdef __FreeBSD__
#include <netinet/in.h>
#endif

#include <dw_util.h>

extern int argc;
extern char** argv;

int port = 4096;
int sock;

bool option(const char* str, const char* shortopt, const char* longopt);

int rcli_init(void) {
	printf("Using BSD TCP/IP\n");
#ifdef __MINGW32__
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);
#endif
	int i;
	char* host = NULL;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '/' || argv[i][0] == '-') {
			if(option(argv[i], "p", "port")) {
				i++;
				port = atoi(argv[i]);
			} else if(option(argv[i], "h", "help")) {
				printf("\n");
				printf("Usage: %s [options] host\n", argv[0]);
				printf("You can use double-dash or slash for long-format flag, and single-dash or slash for short-foramt flag.\n");
				printf("Options:\n");
				printf("\t-p --port [port]   Specify the port to be listened on\n");
				exit(0);
			} else {
				fprintf(stderr, "Invalid option: %s\n", argv[i]);
				return 1;
			}
		} else {
			if(host != NULL) free(host);
			host = __dw_strdup(argv[i]);
		}
	}
	if(host == NULL) {
		fprintf(stderr, "Specify host\n");
		return 1;
	}
	ADDRINFO hints;
	ADDRINFO* result;
	ADDRINFO* rp;
	int s;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;

	char* strport = malloc(6);
	memset(strport, 0, 6);
	sprintf(strport, "%d", port);
	s = getaddrinfo(host, strport, &hints, &result);
	if(s != 0) {
		free(strport);
		fprintf(stderr, "Failed to resolve\n");
		return 1;
	}

	for(rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sock == -1) continue;
		int nbyt = 65535;
		setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&nbyt, sizeof(nbyt));
		nbyt = 65535;
		setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&nbyt, sizeof(nbyt));
		int yes = 1;
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes));
		if(connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) break;
#ifdef __MINGW32__
		closesocket(sock);
#else
		close(sock);
#endif
	}
	freeaddrinfo(result);
	free(strport);
	if(rp == NULL) {
		fprintf(stderr, "Failed to connect\n");
		return 1;
	}

	return 0;
}

void disconnect(void) {
#ifdef __MINGW32__
	closesocket(sock);
#else
	close(sock);
#endif
}

char* readline_sock(void) {
	char* str = malloc(1);
	str[0] = 0;
	char cbuf[2];
	cbuf[1] = 0;
	while(1) {
		int s = recv(sock, cbuf, 1, 0);
		if(s <= 0) {
			free(str);
			return NULL;
		}
		if(cbuf[0] == '\n') {
			break;
		} else if(cbuf[0] != '\r') {
			char* tmp = str;
			str = __dw_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	return str;
}

void writeline(const char* str) {
	char* w = __dw_strcat(str, "\r\n");
	send(sock, w, strlen(str) + 2, 0);
	free(w);
}
