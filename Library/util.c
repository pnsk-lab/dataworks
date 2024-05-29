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

#include "dw_util.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MINGW32__
#include <fileapi.h>
#else
#include <unistd.h>
#endif

char* __dw_strdup(const char* a) {
	char* str = malloc(strlen(a) + 1);
	memcpy(str, a, strlen(a));
	str[strlen(a)] = 0;
	return str;
}

char* __dw_strcat(const char* a, const char* b) {
	char* str = malloc(strlen(a) + strlen(b) + 1);
	memcpy(str, a, strlen(a));
	memcpy(str + strlen(a), b, strlen(b));
	str[strlen(a) + strlen(b)] = 0;
	return str;
}

bool __dw_strcaseequ(const char* a, const char* b) {
	if(strlen(a) != strlen(b)) return false;
	int i;
	for(i = 0; a[i] != 0; i++) {
		if(tolower(a[i]) != tolower(b[i])) return false;
	}
	return true;
}

bool __dw_lockfile(struct dataworks_db* db) {
	if(db->locked) {
		return true;
	}
	off_t off = ftell(db->fp);
	fseek(db->fp, 0, SEEK_SET);
#if defined(__WATCOMC__)

#elif defined(__MINGW32__)
	OVERLAPPED overlap = {0};
	LockFileEx(db->fp, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlap);
#else
	lockf(fileno(db->fp), F_LOCK, 0);
#endif
	fseek(db->fp, off, SEEK_SET);
	db->locked = true;
	return true;
}

bool __dw_unlockfile(struct dataworks_db* db) {
	if(!db->locked) {
		return true;
	}
	off_t off = ftell(db->fp);
	fseek(db->fp, 0, SEEK_SET);
	fflush(db->fp);
#if defined(__WATCOMC__)

#elif defined(__MINGW32__)
	OVERLAPPED overlap = {0};
	UnlockFileEx(db->fp, 0, MAXDWORD, MAXDWORD, &overlap);
#else
	lockf(fileno(db->fp), F_ULOCK, 0);
#endif
	fseek(db->fp, off, SEEK_SET);
	db->locked = false;
	return true;
}

double __dw_atof(const char* str) {
	double num = 0;
	bool pr = false;
	int i;
	double mul = 10;
	for(i = 0; str[i] != 0; i++) {
		if(str[i] >= '0' && str[i] <= '9') {
			if(!pr) {
				num *= 10;
				num += str[i] - '0';
			} else {
				num += (str[i] - '0') / mul;
				mul *= 10;
			}
		} else if(str[i] == '.') {
			pr = true;
		}
	}
	return str[0] == '-' ? -num : num;
}
