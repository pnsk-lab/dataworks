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

extern int argc;
extern char** argv;

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

void protocol_init(int sock);

bool option(const char* str, const char* shortopt, const char* longopt);

int port = 4096;
int server_socket;
struct sockaddr_in6 server_address;

int server_init(void) {
	printf("Using BSD TCP/IP\n");
	int i;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '/' || argv[i][0] == '-'){
			if(option(argv[i], "p", "port")){
				i++;
				port = atoi(argv[i]);
			}
		}
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
