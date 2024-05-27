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

#include "dw_database.h"

#include "dw_util.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __dw_add_record(struct dataworks_db* db, uint64_t dbindex, uint64_t index, void* _data, uint64_t size, bool set) {
	uint8_t* data = _data;
	int fragnum = 0;
	__dw_lockfile(db);
	if(db->version == 1) {
		char* buf = malloc(1 + 8 + 8 + 1 + 1 + 8 + 8);
		fseek(db->fp, 3 + 2 + 8 + (1 + 8 + 1 + 256 + 4096) * 256, SEEK_SET);
		struct dataworks_db_v1_dbentry dbentry;
		while(1) {
			fread(buf, 1, 1 + 8 + 8 + 1 + 1 + 8 + 8, db->fp);
			__dw_buffer_to_db_v1_dbentry(buf, dbentry);
			if(feof(db->fp)) break;
			if(!(dbentry.flag & DATAWORKS_V1_DBENTRY_USED)) {
				fseek(db->fp, -(1 + 8 + 8 + 1 + 1 + 8 + 8), SEEK_CUR);
				buf[0] |= DATAWORKS_V1_DBENTRY_USED;
				if(dbentry.size <= size) {
					if(dbentry.size < size) buf[0] |= DATAWORKS_V1_DBENTRY_FRAGMENT;
					__dw_big_endian(dbentry.size, uint64_t, memcpy(buf + 1, __converted_ptr, 8));
					buf[1 + 8 + 8] = index;
					buf[1 + 8 + 8 + 1] = dbindex;
					__dw_big_endian(fragnum, uint64_t, memcpy(buf + 1 + 8 + 8 + 1 + 1 + 8, __converted_ptr, 8));
					fragnum++;
					size -= dbentry.size;
				}
				fwrite(buf, 1, 1 + 8 + 8 + 1 + 1 + 8 + 8, db->fp);
			}
			fseek(db->fp, dbentry.size, SEEK_CUR);
		}
		free(buf);
	}
	__dw_unlockfile(db);
}

struct dataworks_db_result* dataworks_database_insert_record(struct dataworks_db* db, void** fields, const char* prop) {
	struct dataworks_db_result* r = malloc(sizeof(*r));
	r->error = false;
	int i;
	char** dbi = dataworks_database_get_table_list(db);
	uint64_t dbindex;
	bool set = false;
	for(i = 0; dbi[i] != NULL; i++) {
		if(!set && strcmp(dbi[i], db->name) == 0) {
			dbindex = i;
			set = true;
		}
		free(dbi[i]);
	}
	free(dbi);
	char* types = dataworks_database_get_table_field_types(db, db->name);
	for(i = 0; prop[i] != 0; i++)
		;
	if(strlen(types) != i) {
		r->error = true;
		r->errnum = DW_ERR_EXEC_INSUFFICIENT_ARGUMENTS;
		return r;
	}
	for(i = 0; prop[i] != 0; i++) {
		uint64_t entsize = 0;
		if(types[i] == DW_RECORD_STRING) {
			entsize = strlen(fields[i]);
		} else if(types[i] == DW_RECORD_INTEGER) {
			entsize = 8;
		} else if(types[i] == DW_RECORD_DOUBLE) {
			entsize = sizeof(double);
		} else if(types[i] == DW_RECORD_LOGICAL) {
			entsize = 1;
		} else if(types[i] == DW_RECORD_HELP) {
			entsize = strlen(fields[i]);
		}
		__dw_add_record(db, dbindex, i, fields[i], entsize, prop[i] == 'S');
	}
	return r;
}
