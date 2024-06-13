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

#include <dataworks.h>

#include <dw_database.h>
#include <dw_util.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int argc;
extern char** argv;
extern bool auth;
extern char* authfile;

#if defined(__MINGW32__)
#define USE_WINSOCK
#endif

#ifdef USE_WINSOCK
#include <process.h>
#include <windows.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#endif
#include <unistd.h>

void protocol_init(int sock);
void protocol_loop(int sock);

bool option(const char* str, const char* shortopt, const char* longopt);

int port = 4096;
int server_socket;
#ifdef USE_WINSOCK
struct sockaddr_in server_address;
#else
struct sockaddr_in6 server_address;
#endif

extern struct dataworks_db* db;

int server_init(void) {
	printf("Using BSD TCP/IP\n");
	int i;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '/' || argv[i][0] == '-') {
			if(option(argv[i], "p", "port")) {
				i++;
				port = atoi(argv[i]);
			} else if(option(argv[i], "l", "login")) {
				i++;
				authfile = argv[i];
				auth = true;
			} else if(option(argv[i], "h", "help")) {
				printf("\n");
				printf("Usage: %s [options] database\n", argv[0]);
				printf("You can use double-dash or slash for long-format flag, and single-dash or slash for short-foramt flag.\n");
				printf("Options:\n");
				printf("\t-p --port [port]   Specify the port to be listened on\n");
				printf("\t-l --login [path]  Specify the authentication file\n");
				exit(0);
			} else {
				fprintf(stderr, "Invalid option: %s\n", argv[i]);
				return 1;
			}
		} else {
			db = dataworks_database_open(argv[i]);
		}
	}
#ifdef USE_WINSOCK
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);
#endif
#ifdef USE_WINSOCK
	if((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
#else
	if((server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
#endif
		fprintf(stderr, "BSD TCP/IP initialization fail (socket)\n");
		return 1;
	}
	int yes = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail (setsockopt)\n");
		close(server_socket);
		return 1;
	}
	if(setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (void*)&yes, sizeof(yes)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail (setsockopt)\n");
		close(server_socket);
		return 1;
	}
#ifdef USE_WINSOCK
#else
	int no = 0;
	if(setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail (setsockopt)\n");
		close(server_socket);
		return 1;
	}
#endif
	memset(&server_address, 0, sizeof(server_address));
#ifdef USE_WINSOCK
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = INADDR_ANY;
	server_address.sin_port = htons(port);
#else
	server_address.sin6_family = AF_INET6;
	server_address.sin6_addr = in6addr_any;
	server_address.sin6_port = htons(port);
#endif
	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail (bind)\n");
		close(server_socket);
		return 1;
	}
	if(listen(server_socket, 128) < 0) {
		fprintf(stderr, "BSD TCP/IP initialization fail (listen)\n");
		close(server_socket);
		return 1;
	}
#ifdef USE_WINSOCK
#else
	signal(SIGCHLD, SIG_IGN);
#endif
	fprintf(stderr, "BSD TCP/IP initialization successful\n");
	return 0;
}

#ifdef USE_WINSOCK
unsigned int WINAPI pass_sock(LPVOID sockptr) {
	int sock = *(int*)sockptr;
#else
void pass_sock(int sock) {
#endif
	protocol_init(sock);
	protocol_loop(sock);
#ifdef USE_WINSOCK
	closesocket(sock);
#else
	close(sock);
#endif
}

void server_loop(void) {
	while(1) {
		struct sockaddr_in claddr;
		int clen = sizeof(claddr);
		int sock = accept(server_socket, (struct sockaddr*)&claddr, &clen);
#ifdef USE_WINSOCK
		HANDLE thread;
		thread = (HANDLE)_beginthreadex(NULL, 0, pass_sock, &sock, 0, NULL);
#else
		pid_t p = fork();
		if(p == 0) {
			pass_sock(sock);
			_exit(0);
		} else {
			close(sock);
		}
#endif
	}
}

char* readline_sock(int sock) {
	char cbuf[2];
	cbuf[1] = 0;
	char* buf = malloc(1);
	buf[0] = 0;
	while(1) {
		int l = recv(sock, cbuf, 1, 0);
		if(l <= 0) {
			free(buf);
			return NULL;
		} else if(cbuf[0] == '\n') {
			break;
		} else if(cbuf[0] != '\r') {
			char* tmp = buf;
			buf = __dw_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	return buf;
}

void disconnect(int sock) { close(sock); }

void writeline(int sock, const char* str) {
	char* snd = __dw_strcat(str, "\r\n");
	send(sock, snd, strlen(snd), 0);
	free(snd);
}
