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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dataworks_database_delete_table(struct dataworks_db* db, const char* name) {
	bool rm = false;
	if(db->version == 1) {
		__dw_lockfile(db);
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
		__dw_unlockfile(db);
	}
	if(rm) return DW_ERR_SUCCESS;
	return DW_ERR_NOT_USED;
}

int dataworks_database_create_table(struct dataworks_db* db, const char* name, char** fields, const char* fieldtypes) {
	if(db->version == 1) {
		__dw_lockfile(db);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		int cnt = 0;
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
					__dw_unlockfile(db);
					return DW_ERR_USED;
				}
				cnt++;
			}
		}
		if(cnt == 256) {
			free(buf);
			__dw_unlockfile(db);
			return DW_ERR_TOO_MANY_TABLES;
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
		__dw_unlockfile(db);
		dataworks_database_update_mtime(db);
	}
	return DW_ERR_SUCCESS;
}

char** dataworks_database_get_table_list(struct dataworks_db* db) {
	if(db->version == 1) {
		__dw_lockfile(db);
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
		__dw_unlockfile(db);
		return list;
	}
	return NULL;
}

char** dataworks_database_get_table_fields(struct dataworks_db* db, const char* table) {
	if(db->version == 1) {
		__dw_lockfile(db);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		int c = 0;
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				char* dbname = malloc(index.dbname_len + 1);
				memcpy(dbname, index.dbname, index.dbname_len);
				dbname[index.dbname_len] = 0;
				if(strcmp(dbname, table) == 0) {
					char** list = malloc(sizeof(*list));
					list[0] = NULL;
					int j;
					int start = 1;
					for(j = 1;; j++) {
						if(index.fields[j] == 0) {
							char* nam = malloc(j - start + 1);
							nam[j - start] = 0;
							memcpy(nam, index.fields + start, j - start);
							int k;
							for(k = 0; list[k] != NULL; k++)
								;
							char** newlist = malloc(sizeof(*newlist) * (k + 2));
							for(k = 0; list[k] != NULL; k++) newlist[k] = list[k];
							newlist[k] = nam;
							newlist[k + 1] = NULL;
							free(list);
							list = newlist;
							j++;
							start = j + 1;
							if(index.fields[j] == 0) break;
						}
					}
					free(dbname);
					free(buf);
					__dw_unlockfile(db);
					return list;
				}
				free(dbname);
			}
		}
		free(buf);
		__dw_unlockfile(db);
	}
	return NULL;
}

void dataworks_database_set_table_count(struct dataworks_db* db, const char* table, uint64_t count) {
	if(db->version == 1) {
		__dw_lockfile(db);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		int c = 0;
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				char* dbname = malloc(index.dbname_len + 1);
				memcpy(dbname, index.dbname, index.dbname_len);
				dbname[index.dbname_len] = 0;
				if(strcmp(dbname, table) == 0) {
					__dw_big_endian(count, uint64_t, memcpy(buf + 1, __converted_ptr, 8));
					fseek(db->fp, -(1 + 8 + 1 + 256 + 4096), SEEK_CUR);
					fwrite(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
					free(dbname);
					free(buf);
					__dw_unlockfile(db);
					return;
				}
				free(dbname);
			}
		}
		free(buf);
		__dw_unlockfile(db);
	}
}

uint64_t dataworks_database_get_table_count(struct dataworks_db* db, const char* table) {
	if(db->version == 1) {
		__dw_lockfile(db);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		int c = 0;
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				char* dbname = malloc(index.dbname_len + 1);
				memcpy(dbname, index.dbname, index.dbname_len);
				dbname[index.dbname_len] = 0;
				if(strcmp(dbname, table) == 0) {
					free(dbname);
					free(buf);
					__dw_unlockfile(db);
					return index.count;
				}
				free(dbname);
			}
		}
		free(buf);
		__dw_unlockfile(db);
		return 0;
	} else {
		return 0;
	}
}

char* dataworks_database_get_table_field_types(struct dataworks_db* db, const char* table) {
	if(db->version == 1) {
		__dw_lockfile(db);
		fseek(db->fp, 3 + 10, SEEK_SET);
		int i;
		struct dataworks_db_v1_indexentry index;
		char* buf = malloc(1 + 8 + 1 + 256 + 4096);
		int c = 0;
		for(i = 0; i < 256; i++) {
			fread(buf, 1, 1 + 8 + 1 + 256 + 4096, db->fp);
			__dw_buffer_to_db_v1_indexentry(buf, index);
			if(index.flag & DATAWORKS_V1_INDEXENTRY_USED) {
				char* dbname = malloc(index.dbname_len + 1);
				memcpy(dbname, index.dbname, index.dbname_len);
				dbname[index.dbname_len] = 0;
				if(strcmp(dbname, table) == 0) {
					int j;
					int count = 0;
					for(j = 0;; j++) {
						if(index.fields[j] == 0) break;
						count++;
						for(; index.fields[j] != 0; j++)
							;
					}
					char* list = malloc(count + 1);
					count = 0;
					for(j = 0;; j++) {
						if(index.fields[j] == 0) break;
						list[count] = index.fields[j];
						count++;
						for(; index.fields[j] != 0; j++)
							;
					}
					list[count] = 0;
					free(dbname);
					free(buf);
					__dw_unlockfile(db);
					return list;
				}
				free(dbname);
			}
		}
		free(buf);
		__dw_unlockfile(db);
	}
	return NULL;
}
