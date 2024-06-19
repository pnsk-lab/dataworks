/* $Id$ */
/* --- START LICENSE --- */
/* -------------------------------------------------------------------------- */
/*                                                   DataWorks - Simple DBMS  */
/* -------------------------------------------------------------------------- */
/* Copyright (c) 2024 Crabware.                                               */
/* Copyright (c) 2024 pnsk-lab.                                               */
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

char* __util_strdup(const char* a) {
	char* str = malloc(strlen(a) + 1);
	memcpy(str, a, strlen(a));
	str[strlen(a)] = 0;
	return str;
}

char* __util_strcat(const char* a, const char* b) {
	char* str = malloc(strlen(a) + strlen(b) + 1);
	memcpy(str, a, strlen(a));
	memcpy(str + strlen(a), b, strlen(b));
	str[strlen(a) + strlen(b)] = 0;
	return str;
}

FILE* in;
FILE* out;

#define MAX 128

int process_doc(void) {
	char cbuf[2];
	cbuf[1] = 0;
	char* full = malloc(1);
	full[0] = 0;
	char* str;
	int indent = 0;

	str = malloc(1);
	str[0] = 0;
	fseek(in, 0, SEEK_SET);
	int cincr = 0;
	while(1) {
		int len = fread(cbuf, 1, 1, in);
		if(len <= 0) break;
		{
			char* tmp = full;
			full = __util_strcat(tmp, cbuf);
			free(tmp);
		}
		if(cbuf[0] == '\n') {
			if(str[0] == '\\') {
				char* cmd = str;
				char* val = NULL;
				int i;
				for(i = 0; str[i] != 0; i++) {
					if(str[i] == ' ') {
						str[i] = 0;
						val = str + i + 1;
						break;
					}
				}
				if(strcmp(cmd, "\\hline") == 0) {
				} else if(strcmp(cmd, "\\indent") == 0) {
				} else if(strcmp(cmd, "\\list") == 0) {
				} else if(strcmp(cmd, "\\list+") == 0) {
					cincr++;
					if(cincr >= MAX){
						free(str);
						free(full);
						fprintf(stderr, "List index stack overflow. Aborted\n");
						return 1;
					}
				} else if(strcmp(cmd, "\\list-") == 0) {
					cincr--;
					if(cincr < 0){
						free(str);
						free(full);
						fprintf(stderr, "List index stack underflow. Aborted\n");
						return 1;
					}
				} else if(str[1] == '"') {
				} else {
					fprintf(stderr, "Invalid command. Aborted\n");
					free(str);
					free(full);
					return 1;
				}
			}
			free(str);
			str = malloc(1);
			str[0] = 0;
		} else if(cbuf[0] != '\r') {
			char* tmp = str;
			str = __util_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	free(str);

	int* list = malloc(MAX * sizeof(*list));
	int listi = 0;
	for(listi = 0; listi < MAX; listi++) list[listi] = 1;

	str = malloc(1);
	str[0] = 0;
	int incr = 0;
	while(1) {
		cbuf[0] = full[incr++];
		if(cbuf[0] == 0) break;
		if(cbuf[0] == '\n') {
			if(str[0] == '\\') {
				char* cmd = str;
				char* val = NULL;
				int i;
				for(i = 0; str[i] != 0; i++) {
					if(str[i] == ' ') {
						str[i] = 0;
						val = str + i + 1;
						break;
					}
				}
				if(strcmp(cmd, "\\hline") == 0) {
					int i;
					for(i = 0; i < 79; i++) fprintf(out, "=");
					fprintf(out, "\n");
				} else if(strcmp(cmd, "\\indent") == 0) {
					if(val == NULL) {
						indent = 0;
					} else {
						if(val[0] == '+') {
							indent += atoi(val + 1);
						} else if(val[0] == '-') {
							indent -= atoi(val + 1);
						} else {
							indent = atoi(val);
						}
					}
				} else if(strcmp(cmd, "\\list+") == 0) {
					list++;
				} else if(strcmp(cmd, "\\list-") == 0) {
					list--;
				} else if(strcmp(cmd, "\\list") == 0) {
					if(val == NULL) {
						*list = 1;
					} else {
						for(i = 0; i < indent; i++) fprintf(out, " ");
						fprintf(out, "%d. %s\n", (*list)++, val);
					}
				} else if(str[1] == '"') {
				}
			} else {
				int i;
				for(i = 0; i < indent; i++) fprintf(out, " ");
				fprintf(out, "%s\n", str);
			}
			free(str);
			str = malloc(1);
			str[0] = 0;
		} else if(cbuf[0] != '\r') {
			char* tmp = str;
			str = __util_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	free(str);
	free(full);
	return 0;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Insufficient arguments\n");
		return 1;
	}
	in = fopen(argv[1], "r");
	out = stdout;
	if(in == NULL) {
		fprintf(stderr, "Failed to open the input\n");
	}
	return process_doc();
}
