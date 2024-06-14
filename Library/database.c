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

#include "dw_database.h"

#include "dw_util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char sig[3] = {0x7f, 'D', 'W'};

const char* dw_errors[] = {"Success", "Used already", "File open fail", "Invalid signature", "Invalid version", "Parser returned NULL", "Cannot call non-method", "Unknown method", "Insufficient arguments", "Too many arguments", "Not used", "Too many tables", "Database not selected", "Parser fail"};

#ifdef M_I86
#define BUFSIZE 128
#endif

int dataworks_database_create(const char* fname) {
	FILE* f = fopen(fname, "wb");
	if(f == NULL) {
		return 1;
	}
#ifdef BUFSIZE
	uint8_t nul[BUFSIZE];
#else
	uint8_t nul[4096];
#endif
	int i;
	fwrite(sig, 1, 3, f);
	nul[0] = 0;
	nul[1] = 1;
	fwrite(nul, 1, 2, f);
	int64_t t = time(NULL);
	__dw_big_endian(t, int64_t, fwrite(__converted_ptr, 1, 8, f));
	for(i = 0; i < sizeof(nul); i++) nul[i] = 0;
	for(i = 0; i < 256; i++) {
		fwrite(nul, 1, 1, f);
		fwrite(nul, 1, 8, f);
		fwrite(nul, 1, 1, f);
		int j;
#ifdef BUFSIZE
		for(j = 0; j < 256 / BUFSIZE; j++) {
			fwrite(nul, 1, BUFSIZE, f);
		}
		for(j = 0; j < 4096 / BUFSIZE; j++) {
			fwrite(nul, 1, BUFSIZE, f);
		}
#else
		fwrite(nul, 1, 256, f);
		fwrite(nul, 1, 4096, f);
#endif
	}
	fclose(f);
	return 0;
}

void dataworks_database_close(struct dataworks_db* db) {
	if(db->fp != NULL) fclose(db->fp);
	free(db);
}

struct dataworks_db* dataworks_database_open(const char* fname) {
	struct dataworks_db* db = malloc(sizeof(*db));
	db->error = false;
	db->fp = NULL;
	db->name = NULL;
	db->locked = false;
	db->fp = fopen(fname, "rb+");
	if(db->fp == NULL) {
		db->error = true;
		db->errnum = DW_ERR_FAIL_FOPEN;
		return db;
	}
	fseek(db->fp, 0, SEEK_SET);
	char readsig[sizeof(sig)];
	fread(readsig, 1, sizeof(sig), db->fp);
	if(memcmp(readsig, sig, sizeof(sig)) != 0) {
		db->error = true;
		db->errnum = DW_ERR_INVALID_SIGNATURE;
		return db;
	}
	__dw_lockfile(db);
	char ptrver[8];
	fread(ptrver, 1, 2, db->fp);
	uint16_t be_ver = *(uint16_t*)(char*)ptrver;
	uint16_t ver;
	__dw_native_endian(be_ver, uint16_t, ver = __converted);
	fread(ptrver, 1, 8, db->fp);
	uint64_t be_mtime = *(uint64_t*)(char*)ptrver;
	uint64_t mtime;
	__dw_native_endian(be_mtime, uint64_t, mtime = __converted);
	__dw_unlockfile(db);
	if(ver == 1) {
		db->version = ver;
		db->mtime = mtime;
		return db;
	} else {
		fclose(db->fp);
		db->fp = NULL;
		db->error = true;
		db->errnum = DW_ERR_INVALID_VERSION;
		return db;
	}
}

int dataworks_database_get_version(struct dataworks_db* db) { return db->version; }

uint64_t dataworks_database_get_mtime(struct dataworks_db* db) { return db->mtime; }

int dataworks_database_get_error_number(struct dataworks_db* db) { return db->errnum; }

const char* dataworks_database_strerror(int n) { return dw_errors[n]; }

void dataworks_database_update_mtime(struct dataworks_db* db) {
	__dw_lockfile(db);
	fseek(db->fp, 3 + 2, SEEK_SET);
	int64_t t = time(NULL);
	__dw_big_endian(t, int64_t, fwrite(__converted_ptr, 1, 8, db->fp));
	__dw_unlockfile(db);
}

void dataworks_database_free(struct dataworks_db* db) {
	if(db->fp != NULL) {
		fclose(db->fp);
	}
	if(db->name != NULL) {
		free(db->name);
	}
	free(db);
}

int dataworks_database_use_table(struct dataworks_db* db, const char* table) {
	int errnum = DW_ERR_SUCCESS;
	if(db->name != NULL) free(db->name);
	bool has = false;
	char** names = dataworks_database_get_table_list(db);
	if(names != NULL) {
		int k;
		for(k = 0; names[k] != NULL; k++) {
			if(strcmp(names[k], table) == 0) {
				has = true;
			}
			free(names[k]);
		}
		free(names);
		if(has) {
			db->name = __dw_strdup(table);
		} else {
			errnum = DW_ERR_NOT_USED;
		}
	} else {
		errnum = DW_ERR_PARSER_NULL;
	}
	return errnum;
}
