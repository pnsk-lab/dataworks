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
#include <dw_parser.h>
#include <dw_util.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
	bool clear = true;
	bool create = false;
	bool banner = true;
	bool log = true;
	const char* fname = NULL;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
				printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
				return 0;
			} else if(strcmp(argv[i], "--create") == 0 || strcmp(argv[i], "-C") == 0) {
				create = true;
			} else if(strcmp(argv[i], "--noclear") == 0 || strcmp(argv[i], "-NC") == 0) {
				clear = false;
			} else if(strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
				banner = false;
				log = false;
			} else if(strcmp(argv[i], "--nobanner") == 0 || strcmp(argv[i], "-NB") == 0) {
				banner = false;
			} else if(strcmp(argv[i], "--nolog") == 0 || strcmp(argv[i], "-NL") == 0) {
				log = false;
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
	if(clear) {
		printf("\x1b[2J\x1b[1;1H");
		fflush(stdout);
	}
	if(banner){
		printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
		if(dataworks_get_endian() == 'L') {
			printf("This system is little-endian.\n");
		} else {
			printf("This system is big-endian.\n");
		}
		printf("\n");
		printf("%s\n", dataworks_get_copyright());
		printf("All rights reserved.\n");
	}
	if(create) {
		if(log){
			printf("\n");
			printf("Creating the database: %s\n", fname);
		}
		int n = dataworks_database_create(fname);
		if(n != 0) {
			printf("Failed to create.\n");
			return 1;
		} else {
			if(log) printf("Created successfully.\n");
		}
	}
	if(log){
		printf("\n");
		printf("Opening the database: %s\n", fname);
	}
	struct dataworks_db* db = dataworks_database_open(fname);
	if(db->error) {
		printf("%s.\n", dataworks_database_strerror(db->errnum));
		dataworks_database_close(db);
		return 1;
	}
	if(log){
		time_t mtime = (time_t)dataworks_database_get_mtime(db);
		struct tm* tm = localtime(&mtime);
		char mtimestr[256];
		strftime(mtimestr, 255, "%a %b %d %H:%M:%S %Z %Y", tm);
		printf("Opened the database (Version %d, Modified at %s).\n", dataworks_database_get_version(db), mtimestr);
		printf("\n");
		printf("Type a command (.help) for the help.\n");
		printf("\n");
	}
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
					padleft(16, ".tables");
					printf("Shows the table list.\n");
					padleft(16, ".clear");
					printf("Clears the console.\n");
				} else if(__dw_strcaseequ(buf, ".tables")) {
					char** list = dataworks_database_get_table_list(db);
					if(list != NULL) {
						int i;
						for(i = 0; list[i] != NULL; i++)
							;
						printf("%d table(s) found.\n", i);
						for(i = 0; list[i] != NULL; i++) {
							printf("  %s\n", list[i]);
							free(list[i]);
						}
						free(list);
					} else {
						printf("Failed to get the list.\n");
					}
				} else if(__dw_strcaseequ(buf, ".clear")) {
					printf("\x1b[2J\x1b[1;1H");
					fflush(stdout);
				} else {
					printf("Unknown dot-command.\n");
				}
			} else if(strlen(buf) > 0) {
				struct __dw_token* token = __dw_parser_parse(NULL, buf);
				if(token != NULL) {
					if(token->error) {
						printf("%s\n", dataworks_database_strerror(token->errnum));
					} else {
					}
				} else {
					printf("Parser returned NULL. Help!\n");
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
