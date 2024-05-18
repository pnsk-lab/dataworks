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

#include "dw_database.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const char sig[3] = {0x7f, 'D', 'W'};

int dataworks_database_create(const char* fname) {
	FILE* f = fopen(fname, "wb");
	if(f == NULL) {
		return 1;
	}
	uint8_t nul[4096];
	int i;
	fwrite(sig, 1, 3, f);
	nul[0] = 0;
	nul[1] = 1;
	fwrite(nul, 1, 2, f);
	for(i = 0; i < 4096; i++) nul[i] = 0;
	for(i = 0; i < 256; i++) {
		fwrite(nul, 1, 1, f);
		fwrite(nul, 1, 8, f);
		fwrite(nul, 1, 1, f);
		fwrite(nul, 1, 256, f);
		fwrite(nul, 1, 4096, f);
	}
	fclose(f);
	return 0;
}

struct dataworks_db* dataworks_database_open(const char* fname) {
	FILE* fp = fopen(fname, "rb+");
	if(fp == NULL) {
		return NULL;
	}
	struct dataworks_db* db = malloc(sizeof(*db));
	db->fp = fp;
	return db;
}
