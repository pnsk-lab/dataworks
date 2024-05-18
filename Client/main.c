/* $Id$ */
/* --- START LICENSE --- */
/* -------------------------------------------------------------------------- */
/* Copyright (c) 2024 Nishi.                                                  */
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

#ifdef __MINGW32__
#include <windows.h>
HANDLE winstdout;
#endif

void padleft(int leftpad, const char* str) {
	int i;
	char* spaces = malloc(leftpad - strlen(str) + 1);
	memset(spaces, ' ', leftpad - strlen(str));
	spaces[leftpad - strlen(str)] = 0;
	printf("%s%s", str, spaces);
	free(spaces);
}

int main(int argc, char** argv) {
	int i;
	bool noclear = false;
	bool create = false;
	const char* fname = NULL;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
				printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
				return 0;
			} else if(strcmp(argv[i], "--create") == 0 || strcmp(argv[i], "-C") == 0) {
				create = true;
			} else if(strcmp(argv[i], "--noclear") == 0) {
				noclear = true;
			} else {
				fprintf(stderr, "%s: %s: invalid option\n", argv[0], argv[i]);
				return 1;
			}
		} else {
			if(fname != NULL) {
				fprintf(stderr, "%s: %s: cannot use more than 1 database at same time.\n", argv[0], argv[i]);
				return 1;
			}
			fname = argv[i];
		}
	}
	if(fname == NULL) {
		fprintf(stderr, "%s: filename needed\n", argv[0]);
		return 1;
	}
#ifdef __MINGW32__
	winstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(winstdout, &mode);
	const DWORD origmode = mode;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(winstdout, mode);
#endif
	if(!noclear) {
		printf("\x1b[2J\x1b[1;1H");
		fflush(stdout);
	}
	printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
	if(dataworks_get_endian() == 'L') {
		printf("This system is little-endian.\n");
	} else {
		printf("This system is big-endian.\n");
	}
	printf("\n");
	printf("Copyright (c) Nishi 2024\n");
	printf("All rights reserved.\n");
	if(create) {
		printf("\n");
		printf("Creating the database: %s\n", fname);
		int n = dataworks_database_create(fname);
		if(n != 0) {
			printf("Failed to create.\n");
			return 1;
		} else {
			printf("Created successfully.\n");
		}
	}
	printf("\n");
	printf("Opening the database: %s\n", fname);
	struct dataworks_db* db = dataworks_database_open(fname);
	if(db == NULL) {
		printf("Bad database file or non-existent.\n");
		return 1;
	}
	printf("Opened the database (Version %d).\n", db->version);
	printf("\n");
	printf("Type a command (.help) for the help\n");
	printf("\n");
	int len = 0;
	char* buf = malloc(1);
	buf[0] = 0;
	char ch;
	char prompt = '.';
	printf("%c ", prompt);
	fflush(stdout);
	while(1) {
		if(fread(&ch, 1, 1, stdin) <= 0) break;
		if(ch == '\n') {
			if(buf[0] == '.') {
				if(__dw_strcaseequ(buf, ".bye") || __dw_strcaseequ(buf, ".quit")) {
					printf("Bye.\n");
					break;
				} else if(__dw_strcaseequ(buf, ".version")) {
					printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
				} else if(__dw_strcaseequ(buf, ".help")) {
					padleft(16, ".help");
					printf("Shows this help.\n");
					padleft(16, ".bye  .quit");
					printf("Quits from the CLI.\n");
					padleft(16, ".version");
					printf("Shows the version of DataWorks.\n");
				} else {
					printf("Unknown dot-command.\n");
				}
			}
			printf("%c ", prompt);
			fflush(stdout);
			free(buf);
			buf = malloc(1);
			buf[0] = 0;
			len = 0;
		} else if(ch != '\r') {
			char* newbuf = malloc(len + 2);
			for(i = 0; i < len; i++) {
				newbuf[i] = buf[i];
			}
			newbuf[i] = ch;
			newbuf[i + 1] = 0;
			free(buf);
			buf = newbuf;
			len++;
		}
	}
	free(buf);
	printf("\n");
}
