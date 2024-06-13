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

extern int argc;
extern char** argv;

int port;

bool option(const char* str, const char* shortopt, const char* longopt);

int rcli_init(void) {
	printf("Using BSD TCP/IP\n");
	int i;
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
		}
	}
	return 0;
}

void disconnect(void) {}

char* readline_sock(void) { return NULL; }

void writeline(const char* str) {}
