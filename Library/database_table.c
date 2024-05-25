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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dataworks_database_delete_table(struct dataworks_db* db, const char* name) {
	bool rm = false;
	if(db->version == 1) {
		__dw_lockfile(db->fp);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				char* dbname = malloc(index.dbname_len + 1);
				memcpy(dbname, index.dbname, index.dbname_len);
				dbname[index.dbname_len] = 0;
				if(strcmp(dbname, name) == 0) {
					fseek(db->fp, -(1 + 8 + 1 + 256 + 4096), SEEK_CUR);
					buf[0] &= ~DATAWORKS_V1_INDEXENTRY_USED;
					fwrite(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
					rm = true;
				}
				free(dbname);
			}
		}
		free(buf);
		__dw_unlockfile(db->fp);
	}
	if(rm) return DW_ERR_SUCCESS;
	return DW_ERR_NOT_USED;
}

int dataworks_database_create_table(struct dataworks_db* db, const char* name, char** fields, const char* fieldtypes) {
	if(db->version == 1) {
		__dw_lockfile(db->fp);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				char* dbname = malloc(index.dbname_len + 1);
				memcpy(dbname, index.dbname, index.dbname_len);
				dbname[index.dbname_len] = 0;
				if(strcmp(name, dbname) == 0) {
					free(dbname);
					free(buf);
					__dw_unlockfile(db->fp);
					return DW_ERR_USED;
				}
			}
		}
		fseek(db->fp, 3 + 10, SEEK_SET);
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(!(index.flag & DATAWORKS_V1_INDEXENTRY_USED)) {
				fseek(db->fp, -(1 + 8 + 1 + 256 + 4096), SEEK_CUR);
				buf[0] |= DATAWORKS_V1_INDEXENTRY_USED;
				buf[8 + 1] = strlen(name);
				memcpy(buf + 1 + 8 + 1, name, strlen(name));
				fwrite(buf, 1, 1 + 8 + 1 + 256, db->fp);
				int j;
				for(j = 0; fieldtypes[j] != 0; j++) {
					fwrite(fieldtypes + j, 1, 1, db->fp);
					fwrite(fields[j], 1, strlen(fields[j]), db->fp);
					fwrite("\0", 1, 1, db->fp);
				}
				fwrite("\0", 1, 1, db->fp);
				break;
			}
		}
		free(buf);
		__dw_unlockfile(db->fp);
		dataworks_database_update_mtime(db);
	}
	return DW_ERR_SUCCESS;
}

char** dataworks_database_get_table_list(struct dataworks_db* db) {
	if(db->version == 1) {
		__dw_lockfile(db->fp);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		int c = 0;
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				c++;
			}
		}
		char** list = malloc(sizeof(*list) * (c + 1));
		fseek(db->fp, 3 + 10, SEEK_SET);
		c = 0;
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				list[c] = malloc(index.dbname_len + 1);
				memcpy(list[c], index.dbname, index.dbname_len);
				list[c][index.dbname_len] = 0;
				c++;
			}
		}
		list[c] = NULL;
		free(buf);
		__dw_unlockfile(db->fp);
		return list;
	}
}
