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

#include <dataworks.h>

#include <dw_database.h>
#include <dw_util.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int argc;
char** argv;
bool auth = false;
char* authfile = NULL;
bool usr1sig = false;
FILE* authdb;
struct auth_entry {
	char* user;
	char* pass;
};
struct auth_entry** entries = NULL;
#ifdef __DOS__
sig_atomic_t signals = 0;
#endif

struct dataworks_db* db = NULL;

void protocol_init(int sock);

bool option(const char* str, const char* shortopt, const char* longopt) {
	char* dos_shortopt = __dw_strcat("/", shortopt);
	char* dos_longopt = __dw_strcat("/", longopt);
	char* nix_shortopt = __dw_strcat("-", shortopt);
	char* nix_longopt = __dw_strcat("--", longopt);
	if(__dw_strcaseequ(str, dos_longopt) || __dw_strcaseequ(str, nix_longopt) || strcmp(str, dos_shortopt) == 0 || strcmp(str, nix_shortopt) == 0) return true;
	return false;
}

int server_init(void);
void server_loop(void);
void writeline(int sock, const char* str);
char* readline_sock(int sock);
void disconnect(int sock);

FILE* flog;

void protocol_init(int sock) {
	/* sock is ignored on hayes */
	char* tmp;
	char* verstr;
	verstr = __dw_strcat("ATTR", auth ? ":AUTH" : "");

	tmp = verstr;
	verstr = __dw_strcat(tmp, ":VER=");
	free(tmp);

	tmp = verstr;
	verstr = __dw_strcat(tmp, dataworks_get_version());
	free(tmp);

	tmp = verstr;
	verstr = __dw_strcat(tmp, ":PLATFORM=");
	free(tmp);

	tmp = verstr;
	verstr = __dw_strcat(tmp, dataworks_get_platform());
	free(tmp);

	writeline(sock, verstr);
	free(verstr);

	writeline(sock, "READY");
}

void protocol_loop(int sock) {
	bool login = false;
	char* user = NULL;
	while(1) {
		char* buf = readline_sock(sock);
		if(buf == NULL) {
			break;
		}
		int i;
		bool has_arg = false;
		for(i = 0; buf[i] != 0; i++) {
			if(buf[i] == ':') {
				buf[i] = 0;
				has_arg = true;
				break;
			}
		}
		if(__dw_strcaseequ(buf, "ECHO")) {
			int start;
			if(has_arg) {
				i++;
				start = i;
				for(;; i++) {
					if(buf[i] == ':' || buf[i] == 0) {
						char oldc = buf[i];
						buf[i] = 0;
						char* echo = __dw_strcat("ECHO:", buf + start);
						start = i + 1;

						writeline(sock, echo);
						free(echo);
						if(oldc == 0) break;
					}
				}
			} else {
				writeline(sock, "ECHO");
			}
		} else if(__dw_strcaseequ(buf, "BYE") || __dw_strcaseequ(buf, "QUIT")) {
			writeline(sock, "QUIT:Bye");
			disconnect(sock);
			break;
		} else if(__dw_strcaseequ(buf, "USER")) {
			if(auth) {
				if(login) {
					writeline(sock, "ERROR:ALREADY_IN");
				} else {
					if(has_arg) {
						int j;
						bool found_user = false;
						for(j = 0; entries[j] != NULL; j++) {
							if(strcmp(entries[j]->user, buf + i + 1) == 0) {
								writeline(sock, "REQPASS");
								if(user != NULL) {
									free(user);
								}
								user = __dw_strdup(buf + i + 1);
								found_user = true;
								break;
							}
						}
						if(!found_user) {
							writeline(sock, "ERROR:NO_SUCH_USER");
						}
					} else {
						writeline(sock, "ERROR:NEED_MORE_ARGS");
					}
				}
			} else {
				writeline(sock, "ERROR:NO_AUTH");
			}
		} else if(__dw_strcaseequ(buf, "LOGOUT")) {
			if(auth) {
				if(login) {
					login = false;
					free(user);
					user = NULL;
					writeline(sock, "LOGOUT");
				} else {
					writeline(sock, "ERROR:NOT_IN");
				}
			} else {
				writeline(sock, "ERROR:NO_AUTH");
			}
		} else if(__dw_strcaseequ(buf, "PASS")) {
			if(auth) {
				if(login) {
					writeline(sock, "ERROR:ALREADY_IN");
				} else if(user == NULL) {
					writeline(sock, "ERROR:NEED_USERNAME");
				} else {
					if(has_arg) {
						int j;
						bool found_user = false;
						for(j = 0; entries[j] != NULL; j++) {
							if(strcmp(entries[j]->user, user) == 0) {
								found_user = true;
								if(strcmp(entries[j]->pass, buf + i + 1) == 0) {
									writeline(sock, "LOGIN");
									login = true;
									fprintf(flog, "User %s is in\n", user);
									break;
								} else {
									writeline(sock, "ERROR:INVALID_PASSWORD");
									free(user);
									user = NULL;
									break;
								}
							}
						}
						if(!found_user) {
							writeline(sock, "ERROR:NO_SUCH_USER");
						}
					} else {
						writeline(sock, "ERROR:NEED_MORE_ARGS");
					}
				}
			} else {
				writeline(sock, "ERROR:NO_AUTH");
			}
		} else if(__dw_strcaseequ(buf, "EXEC")) {
			if(auth ? login : 1) {
				if(has_arg) {
					dataworks_database_execute_code(db, buf + i + 1, false);
				} else {
					writeline(sock, "ERROR:NEED_MORE_ARGS");
				}
			} else {
				writeline(sock, "ERROR:NOT_IN");
			}
		} else {
			writeline(sock, "ERROR:UNKNOWN_CMD");
		}
		free(buf);
	}
	if(user != NULL) fprintf(flog, "User %s disconnected\n", user);
}

void exitnow(int sig) {
#ifdef __DOS__
	if(sig != -1) signals++;
#else
	if(sig != -1) exit(0);
#endif
}

int rescan(void) {
	if(entries != NULL) {
		fprintf(flog, "Rescan...\n");
		int i;
		for(i = 0; entries[i] != NULL; i++) {
			free(entries[i]->user);
			free(entries[i]->pass);
			free(entries[i]);
		}
		free(entries);
	}
	entries = malloc(sizeof(*entries));
	entries[0] = NULL;
	if(auth) {
		authdb = fopen(authfile, "r");
		if(authdb == NULL) {
			fprintf(stderr, "Failed to open authfile\n");
			return 1;
		}
		char cbuf[2];
		cbuf[1] = 0;
		char* str = malloc(1);
		str[0] = 0;
		while(true) {
			if(fread(cbuf, 1, 1, authdb) <= 0) break;
			if(cbuf[0] == '\n') {
				int i;
				bool has_pass = false;
				for(i = 0; str[i] != 0; i++) {
					if(str[i] == ':') {
						str[i] = 0;
						has_pass = true;
						break;
					}
				}
				if(has_pass) {
					int shift = i;
					struct auth_entry** old_entries = entries;
					for(i = 0; old_entries[i] != NULL; i++)
						;
					entries = malloc(sizeof(*entries) * (i + 2));
					for(i = 0; old_entries[i] != NULL; i++) entries[i] = old_entries[i];
					free(old_entries);
					entries[i] = malloc(sizeof(**entries));
					entries[i]->user = __dw_strdup(str);
					entries[i]->pass = __dw_strdup(str + shift + 1);
					entries[i + 1] = NULL;
					fprintf(flog, "User %s is allowed to access the database now\n", str);
				}
				free(str);
				str = malloc(1);
				str[0] = 0;
			} else if(cbuf[0] != '\r') {
				char* tmp = str;
				str = __dw_strcat(tmp, cbuf);
				free(tmp);
			}
		}
		free(str);
	}
	return 0;
}

void rescan_sig(int sign) {
	int st = rescan();
	if(st != 0) exit(st);
}

int main(int _argc, char** _argv) {
	argc = _argc;
	argv = _argv;
	flog = stdout;
	signal(SIGINT, exitnow);
	signal(SIGTERM, exitnow);
	signal(SIGUSR1, rescan_sig);
	printf("DataWorks Server  version %s  %s %s\n", dataworks_get_version(), dataworks_get_compile_date(), dataworks_get_platform());
	printf("\n");
	printf("%s\n", dataworks_get_copyright());
	printf("\n");
	int st;
	if((st = server_init()) != 0) return st;
	st = rescan();
	if(st != 0) return st;
	if(db != NULL) {
		if(db->error) {
			dataworks_database_close(db);
			db = NULL;
		}
	}
	if(db == NULL) {
		fprintf(stderr, "Failed to open databse\n");
		return 1;
	}
#if !defined(__WATCOMC__) && !defined(__MINGW32__)
	if(usr1sig) {
		/* Server is ready, raise SIGUSR1 */
		kill(getppid(), SIGUSR1);
	}
#endif
	server_loop();
}
