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

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "indep.h"

const char copyr1[] = "Copyright (C) Crabware, pnsk-lab 2024";

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

int main() {
	printf("\x1b[2J\x1b[1;1H");
	fflush(stdout);
	printf(INSTALLER_TITLE);
	int i;
	for(i = 0; i < 80; i++) printf("-");
	fflush(stdout);
	printf("\x1b[2;%dH %s -\n", 80 - strlen(copyr1) - 2, copyr1);
	printf("%s\n", INSTALLER_WELCOME);
	printf("%s", INSTALLER_WHERE);
	char cbuf[2];
	cbuf[1] = 0;
	char* str = malloc(1);
	str[0] = 0;
do_so:
	while(1) {
		if(fread(cbuf, 1, 1, stdin) <= 0) {
			free(str);
			return 1;
		}
		if(cbuf[0] == '\n') {
			if(strlen(str) == 0) {
				goto do_so;
			}
			break;
		} else if(cbuf[0] != '\r') {
			char* tmp = str;
			str = __util_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	FILE* in = fopen("extract.exe", "rb");
	struct stat s;
	stat("extract.exe", &s);
	if(in == NULL) {
		printf("%s: ", INSTALLER_ERROR);
		printf("%s\n", strerror(errno));
		printf("%s\n", INSTALLER_ABORT);
		free(str);
		return 1;
	}
	if(chdir(str) != 0) {
		fclose(in);
		printf("%s: ", INSTALLER_ERROR);
		printf("%s\n", strerror(errno));
		printf("%s\n", INSTALLER_ABORT);
		free(str);
		return 1;
	} else {
		char* to = __util_strcat(str, "\\_extract.exe");
		FILE* out = fopen(to, "wb");
		if(out == NULL) {
			fclose(in);
			printf("%s: ", INSTALLER_ERROR);
			printf("%s\n", strerror(errno));
			printf("%s\n", INSTALLER_ABORT);
			free(str);
			return 1;
		}
		char buf[512];
		uint64_t readalr = 0;
		while(1) {
			int len = fread(buf, 1, 512, in);
			fwrite(buf, 1, len, out);
			readalr += len;
			printf("\r%s [", INSTALLER_COPYING);
			int i;
			for(i = 0; i < 20; i++) {
				if(20 * ((double)readalr / (uint64_t)s.st_size) > i) {
					printf("#");
				} else {
					printf(" ");
				}
			}
			printf("] %3.2f%%", 100 * ((double)readalr / (uint64_t)s.st_size));
			fflush(stdout);
			if(feof(in)) break;
		}
		fclose(in);
		fclose(out);
		printf("\n");
		int i;
		for(i = 0; to[i] != 0; i++) {
			if(to[i] == ':') {
				unsigned total;
				if(to[0] >= 'A' && to[0] <= 'Z') {
					_dos_setdrive(to[0] - 'A' + 1, &total);
				} else if(to[0] >= 'a' && to[0] <= 'z') {
					_dos_setdrive(to[0] - 'a' + 1, &total);
				}
				break;
			}
		}
		remove("dw.exe");
		remove("dwserv.exe");
		remove("dwrcli.exe");
		remove("readme.doc");
		system(to);
		remove(to);
		free(to);
		printf("%s\n", INSTALLER_SUCCESS);
	}
	free(str);
	return 0;
}
