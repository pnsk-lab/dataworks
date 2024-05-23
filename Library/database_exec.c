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

#include "dw_parser.h"
#include "dw_util.h"

#include <stdbool.h>
#include <stdlib.h>

struct dataworks_db_result* __dataworks_database_execute_code(struct dataworks_db* db, struct __dw_token* token) {
	struct dataworks_db_result* r = malloc(sizeof(*r));
	r->value = NULL;
	r->error = false;
	if(token->type == __DW_METHOD) {
		int i;
		struct dataworks_db_result** results = malloc(sizeof(*results));
		results[0] = NULL;
		int argc = 0;
		if(token->token != NULL) {
			for(i = 0; token->token[i] != NULL; i++) {
				argc++;
				if(token->token[i]->type == __DW_METHOD) {
					struct dataworks_db_result* r2 = __dataworks_database_execute_code(db, token->token[i]);
					if(r2->error) {
						int j;
						for(j = 0; results[j] != NULL; j++) {
							dataworks_database_free_result(results[j]);
						}
						free(results);
						dataworks_database_free_result(r);
						r = r2;
						break;
					}
					int j;
					for(j = 0; results[j] != NULL; j++)
						;
					struct dataworks_db_result** newresults = malloc(sizeof(*newresults) * (j + 2));
					for(j = 0; results[j] != NULL; j++) {
						newresults[j] = results[j];
					}
					free(results);
					results = newresults;
					results[j] = r2;
					results[j + 1] = NULL;
				} else {
					struct dataworks_db_result* r2 = malloc(sizeof(*r2));
					r2->error = false;
					r2->value = __dw_strdup(token->token[i]->name);
					int j;
					for(j = 0; results[j] != NULL; j++)
						;
					struct dataworks_db_result** newresults = malloc(sizeof(*newresults) * (j + 2));
					for(j = 0; results[j] != NULL; j++) {
						newresults[j] = results[j];
					}
					free(results);
					results = newresults;
					results[j] = r2;
					results[j + 1] = NULL;
				}
			}
		}
		if(r->error) return r;
		if(__dw_strcaseequ(token->name, "create_table")) {
			argc = 0;
			int first = -1;
			int j;
			for(j = 0; results[j] != NULL; j++) {
				if(results[j]->value != NULL) {
					argc++;
					if(first == -1) first = j;
				}
			}
			if(argc < 2) {
				r->error = true;
				r->errnum = DW_ERR_EXEC_INSUFFICIENT_ARGUMENTS;
			} else {
				int j;
				char* fieldtypes = malloc(argc);
				fieldtypes[argc - 1] = 0;
				char** fields = malloc(sizeof(*fields) * (argc));
				fields[argc - 1] = NULL;
				argc = 0;
				for(j = first + 1; results[j] != NULL; j++) {
					if(results[j]->value != NULL) {
						char* val = __dw_strdup(results[j]->value);
						int k;
						for(k = 0; val[k] != 0; k++) {
							if(val[k] == ':') {
								val[k] = 0;
								if(__dw_strcaseequ(val, "string")) {
									fieldtypes[argc] = 'S';
								} else if(__dw_strcaseequ(val, "integer")) {
									fieldtypes[argc] = 'I';
								} else if(__dw_strcaseequ(val, "double")) {
									fieldtypes[argc] = 'D';
								} else if(__dw_strcaseequ(val, "logical")) {
									fieldtypes[argc] = 'L';
								} else if(__dw_strcaseequ(val, "help")) {
									fieldtypes[argc] = '?';
								}
								fields[argc] = __dw_strdup(val + k + 1);
								argc++;
								break;
							}
						}
						free(val);
					}
				}
				int er;
				if((er = dataworks_database_create_table(db, results[first]->value, fields, fieldtypes)) != DW_ERR_SUCCESS) {
					r->error = true;
					r->errnum = er;
				}
				for(j = 0; fields[j] != NULL; j++) free(fields[j]);
				free(fields);
				free(fieldtypes);
			}
		} else if(__dw_strcaseequ(token->name, "concat")) {
			r->value = malloc(1);
			r->value[0] = 0;
			int j;
			for(j = 0; results[j] != NULL; j++) {
				if(results[j]->value != NULL) {
					char* tmp = r->value;
					r->value = __dw_strcat(tmp, results[j]->value);
					free(tmp);
				}
			}
		} else if(__dw_strcaseequ(token->name, "print")) {
			int j;
			for(j = 0; results[j] != NULL; j++) {
				if(results[j]->value != NULL) {
					printf("%s\n", results[j]->value);
				}
			}
		} else {
			r->error = true;
			r->errnum = DW_ERR_EXEC_UNKNOWN_METHOD;
		}
		int j;
		for(j = 0; results[j] != NULL; j++) {
			dataworks_database_free_result(results[j]);
		}
		free(results);
	} else {
		r->error = true;
		r->errnum = DW_ERR_EXEC_NON_METHOD;
	}
	return r;
}

struct dataworks_db_result* dataworks_database_execute_code(struct dataworks_db* db, const char* code) {
	struct dataworks_db_result* r = malloc(sizeof(*r));
	r->error = false;
	r->value = NULL;
	struct __dw_token* token = __dw_parser_parse(code, true);
	if(token != NULL) {
		if(token->error) {
			r->error = true;
			r->errnum = token->errnum;
		} else {
			dataworks_database_free_result(r);
			r = __dataworks_database_execute_code(db, token);
		}
		__dw_parser_free(token);
	} else {
		r->error = true;
		r->errnum = DW_ERR_PARSER_NULL;
	}
	return r;
}

void dataworks_database_free_result(struct dataworks_db_result* result) {
	if(result->value != NULL) free(result->value);
	free(result);
}
