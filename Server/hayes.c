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
#include <time.h>

extern int argc;
extern char** argv;
extern bool auth;
extern char* authfile;

#include <bios.h>
#include <conio.h>
#include <i86.h>

void protocol_init(int sock);
void protocol_loop(int sock);
void disconnect(int sock);
void writeline(int sock, const char* str);

#define INIT_MODEM "AT&FE0F1"

bool option(const char* str, const char* shortopt, const char* longopt);

int port = -1;

int get_ioport() {
#ifdef PC98
	if(port == 0) return 0x30;
#else
	if(port == 0) {
		return 0x3f8;
	} else if(port == 1) {
		return 0x2f8;
	} else if(port == 2) {
		return 0x3e8;
	} else if(port == 3) {
		return 0x2e8;
	}
#endif
	return 0;
}

void write_serial(const char* str) {
	const char* ptr = str;
	while(1) {
		if(inp(get_ioport() + 5) & 0x20) {
			outp(get_ioport(), *ptr);
			ptr++;
			if(*ptr == 0) break;
		}
	}
}

bool connected;
bool fancy = false;

extern bool usr1sig;

extern sig_atomic_t signals;
extern struct dataworks_db* db;

char** commands = NULL;

void render_commands(void);

char* modem_response(void) {
	char* buf = malloc(1);
	buf[0] = 0;
	char cbuf[2];
	cbuf[1] = 0;
	while(1) {
		if(inp(get_ioport() + 5) & 1) {
			unsigned short b = inp(get_ioport());
			char ch = b & 0xff;
			if(ch != '\r' && ch != '\n' && ch != 0) {
				cbuf[0] = ch;
				char* tmp = buf;
				buf = __dw_strcat(tmp, cbuf);
				free(tmp);
			} else if(ch == '\r') {
				if(strlen(buf) != 0) break;
			}
		}
		printf("\r");
		if(signals > 0) {
			printf("\x1b[2J\x1b[1;1H");
			free(buf);
			if(connected) {
				writeline(0, "QUIT:Bye");
				disconnect(0);
			}
			return NULL;
		}
	}
	if(connected && fancy && commands != NULL) {
		char** oldcmds = commands;
		int i;
		for(i = 0; oldcmds[i] != NULL; i++)
			;
		commands = malloc(sizeof(*commands) * (i + 2));
		for(i = 0; oldcmds[i] != NULL; i++) commands[i] = oldcmds[i];

		time_t mtime = (time_t)time(NULL);
		struct tm* tm = localtime(&mtime);
		char mtimestr[256];
		strftime(mtimestr, 255, "%a %b %d %H:%M:%S %Z %Y", tm);

		char* tmp = __dw_strcat(mtimestr, ": ");
		commands[i] = __dw_strcat(tmp, buf);
		free(tmp);
		commands[i + 1] = NULL;
		free(oldcmds);

		oldcmds = commands;
		for(i = 0; oldcmds[i] != NULL; i++)
			;
		if(i > 24 - 10) {
			for(i = 0; oldcmds[i] != NULL; i++)
				;
			commands = malloc(sizeof(*commands) * (i + 1));
			free(oldcmds[0]);
			for(i = 1; oldcmds[i] != NULL; i++) commands[i - 1] = oldcmds[i];
			commands[i - 1] = NULL;
			free(oldcmds);
		}

		render_commands();
	}
	if(!connected && !fancy) printf("-> %s\n", buf);
	return buf;
}

void update_message(const char* str) {
	int i;
	printf("\x1b[5;2H\x1b[1m\x1b[34m");
	printf("%s", str);
	for(i = 0; i < 24 - strlen(str); i++) printf(" ");
	printf("\x1b[m");
	fflush(stdout);
}

void fancy_ready(void);

void render_commands(void) {
	int i;
	for(i = 0; commands[i] != NULL; i++)
		;
	for(; i >= 0; i--) {
		printf("\x1b[%d;2H\x1b[K%s", 24 - i, commands[i]);
	}
}

void fancy_init(void) {
	if(commands == NULL) {
		commands = malloc(sizeof(*commands));
		commands[0] = NULL;
	}
	int i;
	printf("\x1b[2J\x1b[1;1H\x1b[47m");
	for(i = 0; i < 80; i++) {
		printf(" ");
	}
	printf("\x1b[1;2H\x1b[1m\x1b[30mDataWorks Server %s   Copyr. 2024 Crabware\x1b[m", dataworks_get_version());
	printf("\x1b[2;1H\x1b[47m\x1b[1m\x1b[30m");
	for(i = 0; i < 80; i++) {
		printf("\xdc");
	}
	printf("\x1b[7;1H\x1b[40m\x1b[1m\x1b[37m");
	for(i = 0; i < 80; i++) {
		printf("\xdc");
	}
	printf("\x1b[8;1H\x1b[47m\x1b[1m\x1b[30m");
	for(i = 0; i < 80; i++) {
		printf(" ");
	}
	printf("\x1b[8;2H\x1b[1m\x1b[30mRecent commands\x1b[m");
	printf("\x1b[9;1H\x1b[47m\x1b[1m\x1b[30m");
	for(i = 0; i < 80; i++) {
		printf("\xdc");
	}
	printf("\x1b[m");
	printf("\x1b[4;2HStatus");
	fflush(stdout);
	fancy_ready();
	render_commands();
}

void fancy_ready(void) { update_message("Ready for call"); }

int server_init(void) {
	printf("Using Hayes Modem\n");
	connected = false;
	int i;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '/' || argv[i][0] == '-') {
			if(option(argv[i], "p", "port")) {
				i++;
				if(__dw_strcaseequ(argv[i], "COM1")) {
					port = 0;
#ifndef PC98
				} else if(__dw_strcaseequ(argv[i], "COM2")) {
					port = 1;
				} else if(__dw_strcaseequ(argv[i], "COM3")) {
					port = 2;
				} else if(__dw_strcaseequ(argv[i], "COM4")) {
					port = 3;
#endif
				} else {
					fprintf(stderr, "Invalid port: %s\n", argv[i]);
					return 1;
				}
			} else if(option(argv[i], "l", "login")) {
				i++;
				authfile = argv[i];
				auth = true;
			} else if(option(argv[i], "d", "db")) {
				i++;
				db = dataworks_database_open(argv[i]);
			} else if(option(argv[i], "s", "signal")) {
				usr1sig = true;
			} else if(option(argv[i], "h", "help")) {
				printf("\n");
				printf("Usage: %s [options] database\n", argv[0]);
				printf("You can use double-dash or slash for long-format flag, and single-dash or slash for short-foramt flag.\n");
				printf("Options:\n");
				printf("\t-p --port [comport]   Specify the modem port\n");
				printf("\t-f --fancy            Enable the fancy manager ;)\n");
				printf("\t-l --login [path]     Specify the authentication file\n");
				printf("\t-s --signal           Send SIGUSR1 when ready\n");
				printf("\t-d --db [path]        Force [path] as the database file\n");
				exit(0);
			} else if(option(argv[i], "f", "fancy")) {
				fancy = true;
			} else {
				fprintf(stderr, "Invalid option: %s\n", argv[i]);
				return 1;
			}
		} else {
			db = dataworks_database_open(argv[i]);
		}
	}
	if(port == -1) {
		fprintf(stderr, "Specify serial port\n");
		return 1;
	}
	if(fancy) {
		fancy_init();
		update_message("Modem init");
	} else {
		printf("Serial port is at I/O 0x%4.4x\n", get_ioport());
	}
	_bios_serialcom(_COM_INIT, port, _COM_9600 | _COM_NOPARITY | _COM_CHR8 | _COM_STOP1);
	write_serial(INIT_MODEM "\r");
	if(!fancy) printf("<- " INIT_MODEM "\n");
	char* resp = modem_response();
	bool echo = __dw_strcaseequ(resp, INIT_MODEM) || __dw_strcaseequ(resp, "NO CARRIER");
	if(resp != NULL && echo) free(resp); /* Kill echo */
	if(resp == NULL) return 0;
	if(echo) resp = modem_response();
	if(resp == NULL) return 0;
	if(__dw_strcaseequ(resp, "OK")) {
		if(fancy) {
			fancy_ready();
		} else {
			printf("Modem initialization successful\n");
			outp(get_ioport() + 4, 1);
		}
	} else {
		if(fancy) printf("\x1b[2J\x1b[1;1H");
		fprintf(stderr, "Modem initialization fail\n");
		free(resp);
		return 1;
	}
	free(resp);

	return 0;
}

void server_loop(void) {
	connected = false;
	while(1) {
		char* resp = modem_response();
		if(resp == NULL) break;
		if(__dw_strcaseequ(resp, "NO CARRIER")) {
			free(resp);
			if(fancy) {
				fancy_ready();
			} else {
				printf("Disconnected\n");
			}
			connected = false;
			outp(get_ioport() + 4, 1);
			continue;
		}
		int i;
		bool has_arg = false;
		for(i = 0; resp[i] != 0; i++) {
			if(resp[i] == ' ') {
				resp[i] = 0;
				has_arg = true;
				break;
			}
		}
		if(connected) {
		} else {
			if(__dw_strcaseequ(resp, "RING")) {
				if(fancy) {
					update_message("RING RING");
				} else {
					printf("Got a call, answering\n");
				}
				write_serial("ATA\r");
				if(fancy) {
				} else {
					printf("<- ATA\n");
				}
			} else if(__dw_strcaseequ(resp, "CONNECT")) {
				if(fancy) {
					char* bps = __dw_strdup("CONNECTED!");
					if(has_arg) {
						free(bps);
						char* tmp = __dw_strcat("CONNECTED @ ", resp + i + 1);
						bps = __dw_strcat(tmp, " BPS!");
						free(tmp);
					}
					update_message(bps);
					free(bps);
				} else {
					printf("Connection successful");
					if(has_arg) printf(", %s BPS", resp + i + 1);
					printf("\n");
					printf("Entering connected state\n");
				}
				connected = true;
				protocol_init(0);
				protocol_loop(0);
			}
		}
		free(resp);
	}
}

char* readline_sock(int sock) {
	char* resp = modem_response();
	if(__dw_strcaseequ(resp, "NO CARRIER")) {
		free(resp);
		if(fancy) {
			fancy_ready();
		} else {
			printf("Disconnected\n");
		}
		connected = false;
	}
	return connected ? resp : NULL;
}

void writeline(int sock, const char* str) {
	char* snd = __dw_strcat(str, "\r\n");
	write_serial(snd);
	free(snd);
}

void disconnect(int sock) {
	while((inp(get_ioport() + 5) & 0x20) == 0)
		;
	delay(100);
	while(inp(get_ioport() + 6) & (1 << 7)) outp(get_ioport() + 4, 0);
	connected = false;
}
