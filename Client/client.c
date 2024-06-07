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
#include <locale.h>
#endif

#ifdef __MINGW32__
#include <windows.h>
HANDLE winstdout;
#endif

void padleft(int leftpad, const char* str) {
	char* spaces = malloc(leftpad - strlen(str) + 1);
	memset(spaces, ' ', leftpad - strlen(str));
	spaces[leftpad - strlen(str)] = 0;
	printf("%s%s", str, spaces);
	free(spaces);
}

bool option(const char* str, const char* shortopt, const char* longopt) {
	char* dos_shortopt = __dw_strcat("/", shortopt);
	char* dos_longopt = __dw_strcat("/", longopt);
	char* nix_shortopt = __dw_strcat("-", shortopt);
	char* nix_longopt = __dw_strcat("--", longopt);
	if(__dw_strcaseequ(str, dos_longopt) || __dw_strcaseequ(str, nix_longopt) || strcmp(str, dos_shortopt) == 0 || strcmp(str, nix_shortopt) == 0) return true;
	return false;
}

int main(int argc, char** argv) {
#ifdef __MINGW32__
	setlocale(LC_ALL, "");
#endif
	int i;
	bool clear = true;
	bool create = false;
	bool banner = true;
	bool log = true;
	const char* fname = NULL;
	const char* fprog = NULL;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-' || argv[i][0] == '/') {
			if(option(argv[i], "V", "version")) {
				printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
				return 0;
			} else if(option(argv[i], "C", "create")) {
				create = true;
			} else if(option(argv[i], "NC", "noclear")) {
				clear = false;
			} else if(option(argv[i], "q", "quiet")) {
				banner = false;
				log = false;
				clear = false;
			} else if(option(argv[i], "NB", "nobanner")) {
				banner = false;
			} else if(option(argv[i], "NL", "nolog")) {
				log = false;
			} else if(option(argv[i], "f", "file")) {
				fprog = argv[i + 1];
				i++;
			} else if(option(argv[i], "h", "help")) {
				printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
				printf("\n");
				printf("Usage: %s [options] database\n", argv[0]);
				printf("You can use double-dash or slash for long-format flag, and single-dash or slash for short-foramt flag.\n");
				printf("Options:\n");
				printf("\t-C  --create        Create the database\n");
				printf("\t-V  --version       Show the version\n");
				printf("\t-NC --noclear       Do not clear the screen on the startup\n");
				printf("\t-NB --nobanner      Do not show the banner on the startup\n");
				printf("\t-NL --nolog         Do not show the log\n");
				printf("\t-q  --quiet         Same with -NC -NB -NL\n");
				printf("\t-f  --file [path]   Run the [path] as the script file\n");
				return 0;
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
	if(banner) {
		printf("DataWorks  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
		printf("Using Yacc: %s\n", dataworks_get_yacc_version());
		printf("       Lex: %s\n", dataworks_get_lex_version());
		if(dataworks_get_endian() == 'L') {
			printf("This system is little-endian.\n");
		} else {
			printf("This system is big-endian.\n");
		}
		printf("\n");
		if(!dataworks_get_if_supported()) {
			printf("!!! THIS PLATFORM IS UNSUPPORTED! !!!\n\n");
		}
		printf("%s\n", dataworks_get_copyright());
		printf("All rights reserved.\n");
	}
	if(create) {
		if(log) {
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
	if(log) {
		printf("\n");
		printf("Opening the database: %s\n", fname);
	}
	struct dataworks_db* db = dataworks_database_open(fname);
	if(db->error) {
		printf("%s.\n", dataworks_database_strerror(db->errnum));
		dataworks_database_close(db);
		return 1;
	}
	if(log) {
		time_t mtime = (time_t)dataworks_database_get_mtime(db);
		struct tm* tm = localtime(&mtime);
		char mtimestr[256];
		strftime(mtimestr, 255, "%a %b %d %H:%M:%S %Z %Y", tm);
		printf("Opened the database (Version %d, Modified at %s).\n", dataworks_database_get_version(db), mtimestr);
		printf("\n");
		if(fprog == NULL) {
			printf("Type a command (.help) for the help.\n");
			printf("\n");
		}
	}
	int len = 0;
	char* buf = malloc(1);
	buf[0] = 0;
	int ch;
	char prompt = '.';
	FILE* fp = stdin;
	if(fprog != NULL) {
		fp = fopen(fprog, "r");
		if(fp == NULL) {
			printf("Could not open the program file.\n");
			return 1;
		}
	}
	if(fprog == NULL) {
		printf("%c ", prompt);
		fflush(stdout);
	}
	char* linebuf = malloc(1);
	linebuf[0] = 0;
	while(1) {
		if((ch = fgetc(fp)) == EOF) break;
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
							printf("  %s: ", list[i]);
							fflush(stdout);
							char** fields = dataworks_database_get_table_fields(db, list[i]);
							if(fields != NULL) {
								int j;
								for(j = 0; fields[j] != NULL; j++) {
									printf("%s ", fields[j]);
									fflush(stdout);
									free(fields[j]);
								}
								free(fields);
							} else {
								printf("(failed to get)");
								fflush(stdout);
							}
							printf("\n");
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
				char* tmp = linebuf;
				linebuf = __dw_strcat(tmp, buf);
				free(tmp);
				int i;
				while(true) {
					bool breakit = true;
					for(i = 0; linebuf[i] != 0; i++) {
						if(linebuf[i] == ';') {
							char* line = malloc(i + 1);
							line[i] = 0;
							memcpy(line, linebuf, i);

							struct dataworks_db_result* r = dataworks_database_execute_code(db, line, log);
							if(r->error) {
								printf("%s.\n", dataworks_database_strerror(r->errnum));
							}

							free(line);
							if(strlen(linebuf) > 0) {
								char* newbuf = malloc(strlen(linebuf) - i);
								newbuf[strlen(linebuf) - i - 1] = 0;
								memcpy(newbuf, linebuf + i + 1, strlen(linebuf) - i - 1);
								free(linebuf);
								linebuf = newbuf;
							}
							breakit = false;
							break;
						}
					}
					if(breakit) break;
				}
			}
			if(fprog == NULL) {
				printf("%c ", prompt);
				fflush(stdout);
			}
			free(buf);
			buf = malloc(1);
			buf[0] = 0;
			len = 0;
		} else if(ch != '\r') {
			char* newbuf = malloc(len + 2);
			for(i = 0; i < len; i++) {
				newbuf[i] = buf[i];
			}
			newbuf[len] = ch;
			newbuf[len + 1] = 0;
			free(buf);
			buf = newbuf;
			len++;
		}
	}
	free(linebuf);
	free(buf);
	printf("\n");
}
