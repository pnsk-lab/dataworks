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

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

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

char* suffix;
pid_t pid;
pid_t pid2;

int exist_exec(const char* og) {
	char* path;
	path = __util_strcat(og, suffix);
	printf("Checking if %s exists... ", path);
	fflush(stdout);
	if(access(path, F_OK) == 0) {
		printf("yes\n");
		free(path);
		return 0;
	} else {
		printf("no\n");
		free(path);
		return 1;
	}
}

int create_db(void) {
	printf("Creating the database... ");
	fflush(stdout);
	char* path = __util_strcat("./Client/dataworks", suffix);
	char* sh = __util_strcat(path, " --create db.dwf -f /dev/null >/dev/null 2>/dev/null");
	int st = system(sh);
	free(sh);
	free(path);
	if(st == 0) {
		printf("ok\n");
		return 0;
	} else {
		printf("fail\n");
		return 1;
	}
}

int check_db(void) {
	printf("Checking the database... ");
	fflush(stdout);
	char* path = __util_strcat("./Client/dataworks", suffix);
	char* sh = __util_strcat(path, " db.dwf -f /dev/null >/dev/null 2>/dev/null");
	int st = system(sh);
	free(sh);
	free(path);
	if(st == 0) {
		printf("ok\n");
		return 0;
	} else {
		printf("fail\n");
		return 1;
	}
}

bool await = true;

void usr1hand(int sig) { await = false; }

int server(void) {
	await = true;
	printf("Starting the server... ");
	fflush(stdout);
	char* path = __util_strcat("./Server/dataworks_server", suffix);
	pid = fork();
	if(pid == 0) {
		int null = open("/dev/null", O_RDWR);
		dup2(null, 1);
		dup2(null, 2);
		execl(path, path, "-s", "db.dwf", NULL);
		free(path);
		printf("fail\n");
		return 1;
	} else if(pid == -1) {
		free(path);
		printf("fail\n");
		return 1;
	}
	free(path);
	int at = 1;
	while(await) {
		printf("%d... ", at++);
		fflush(stdout);
		sleep(1);
		if(at > 3) {
			int status;
			waitpid(pid, &status, WNOHANG);
			if(WIFEXITED(status)) {
				printf("fail\n");
				return 1;
			}
		}
	}
	printf("ok\n");
	return 0;
}

int rcli(void) {
	await = true;
	printf("Starting the remote client... ");
	fflush(stdout);
	char* path = __util_strcat("./RemoteClient/dataworks_remote_client", suffix);
	pid2 = fork();
	if(pid2 == 0) {
		int null = open("/dev/null", O_RDWR);
		dup2(null, 1);
		dup2(null, 2);
		execl(path, path, "-s", "127.0.0.1", NULL);
		free(path);
		printf("fail\n");
		return 1;
	} else if(pid2 == -1) {
		free(path);
		printf("fail\n");
		return 1;
	}
	free(path);
	int at = 1;
	while(await) {
		printf("%d... ", at++);
		fflush(stdout);
		sleep(1);
		if(at > 3) {
			int status;
			waitpid(pid2, &status, WNOHANG);
			if(WIFEXITED(status) && WEXITSTATUS(status) != 0) {
				printf("fail\n");
				return 1;
			}
		}
	}
	printf("ok\n");
	return 0;
}

int main(int argc, char** argv) {
	int i;
	suffix = __util_strdup("");
	signal(SIGUSR1, usr1hand);
	for(i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-s") == 0) {
			i++;
			free(suffix);
			suffix = __util_strdup(argv[i]);
		}
	}
	printf("Testing DataWorks\n");
	int st;
	if((st = exist_exec("./Client/dataworks")) != 0) return st;
	if((st = exist_exec("./Server/dataworks_server")) != 0) return st;
	if((st = exist_exec("./RemoteClient/dataworks_remote_client")) != 0) return st;
	printf("All executables present\n");
	if((st = create_db()) != 0) return st;
	if((st = check_db()) != 0) return st;
	if((st = server()) != 0) return st;
	if((st = rcli()) != 0) {
		kill(pid, SIGINT);
		int status;
		waitpid(pid, &status, 0);
		return st;
	}
	kill(pid, SIGINT);
	kill(pid2, SIGINT);
	int status;
	waitpid(pid, &status, 0);
	waitpid(pid2, &status, 0);
	remove("db.dwf");
	return 0;
}
