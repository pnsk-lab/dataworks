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

#include <dataworks.h>
#include <dw_util.h>

int argc;
char** argv;

int rcli_init(void);
char* readline_sock(void);
void writeline(const char*);
void disconnect(void);

bool option(const char* str, const char* shortopt, const char* longopt) {
	char* dos_shortopt = __dw_strcat("/", shortopt);
	char* dos_longopt = __dw_strcat("/", longopt);
	char* nix_shortopt = __dw_strcat("-", shortopt);
	char* nix_longopt = __dw_strcat("--", longopt);
	if(__dw_strcaseequ(str, dos_longopt) || __dw_strcaseequ(str, nix_longopt) || strcmp(str, dos_shortopt) == 0 || strcmp(str, nix_shortopt) == 0) return true;
	return false;
}

int main(int _argc, char** _argv) {
	printf("DataWorks Remote Client  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
	argc = _argc;
	argv = _argv;
	int st = rcli_init();
	bool ready = false;
	if(st != 0) return st;
	while(1) {
		char* resp = readline_sock();
		if(resp == NULL) {
			break;
		}
		int i;
		bool has_arg = false;
		for(i = 0; resp[i] != 0; i++) {
			if(resp[i] == ':') {
				resp[i] = 0;
				has_arg = true;
				break;
			}
		}
		char* arg = has_arg ? resp + i + 1 : NULL;
		if(__dw_strcaseequ(resp, "READY")) {
			printf("Connection is ready\n");
			ready = true;
			break;
		} else if(__dw_strcaseequ(resp, "ATTR") && has_arg) {
			int start = 0;
			for(i = 0;; i++) {
				if(arg[i] == ':' || arg[i] == 0) {
					bool brk = arg[i] == 0;
					arg[i] = 0;
					int j;
					char* curarg = arg + start;
					for(j = 0; curarg[j] != 0; j++) {
						if(curarg[j] == '=') {
							curarg[j] = 0;
							if(__dw_strcaseequ(curarg, "VER")) {
								printf("Server version : ");
							} else if(__dw_strcaseequ(curarg, "PLATFORM")) {
								printf("Server Platform: ");
							} else {
								printf("%s: ", curarg);
							}
							fflush(stdout);
							printf("%s\n", curarg + j + 1);
							break;
						}
					}
					start = i + 1;
					if(brk) break;
				}
			}
		}
		free(resp);
	}
	if(ready) {
		char cbuf[2];
		cbuf[1] = 0;
		char* str = malloc(1);
		str[0] = 0;
		printf(". ");
		fflush(stdout);
		while(true) {
			int len = fread(cbuf, 1, 1, stdin);
			if(len <= 0) break;
			if(cbuf[0] == '\n') {
				if(__dw_strcaseequ(str, ".quit")) {
					break;
				}
				free(str);
				str = malloc(1);
				str[0] = 0;
				printf(". ");
				fflush(stdout);
			} else if(cbuf[0] != '\r') {
				char* tmp = str;
				str = __dw_strcat(tmp, cbuf);
				free(tmp);
			}
		}
		free(str);
		writeline("QUIT");
		while(true) {
			char* resp = readline_sock();
			if(resp == NULL) break;
			free(resp);
		}
	}
	return 0;
}
