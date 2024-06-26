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

#include "dataworks.h"
#include "dw_parser.h"
#include "dw_util.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct Node* parser_process(struct dataworks_db* db, struct Node* node, bool dolog) {
	if(node->ident != NULL) {
		int i;
		struct Node* newnode = malloc(sizeof(*newnode));
		newnode->ident = NULL;
		newnode->string = NULL;
		newnode->nodes = NULL;
		newnode->type = 0;
		newnode->result = NULL;
		newnode->errnum = DW_ERR_SUCCESS;
		char** fields = malloc(sizeof(*fields));
		fields[0] = NULL;
		char* fieldtypes = malloc(1);
		fieldtypes[0] = 0;
		char* name = NULL;
		bool used = false;
		if(__dw_strcaseequ(node->ident, "version")) {
			newnode->string = __dw_strdup(dataworks_get_version());
			used = true;
		}
		if(node->nodes != NULL && !__dw_strcaseequ(node->ident, "insert")) {
			for(i = 0; node->nodes[i] != NULL; i++) {
				struct Node* r = parser_process(db, node->nodes[i], false);
				if(r->errnum != DW_ERR_SUCCESS) {
					newnode->errnum = r->errnum;
					__dw_free_node(r);
					int j;
					for(j = 0; fields[j] != NULL; j++) free(fields[j]);
					free(fields);
					free(fieldtypes);
					if(name != NULL) free(name);
					return newnode;
				}
				if(__dw_strcaseequ(node->ident, "print")) {
					if(r->type == 'N') {
						printf("%f", r->number);
					} else if(r->type == 'L') {
						printf("%c", r->logical ? 'T' : 'F');
					} else if(r->string != NULL) {
						printf("%s", r->string);
					}
					fflush(stdout);
					used = true;
				} else if(__dw_strcaseequ(node->ident, "concat")) {
					if(r->string != NULL) {
						if(newnode->string == NULL) {
							newnode->string = malloc(1);
							newnode->string[0] = 0;
						}
						char* tmp = newnode->string;
						newnode->string = __dw_strcat(tmp, r->string);
						free(tmp);
					}
					used = true;
				} else if(__dw_strcaseequ(node->ident, "use")) {
					if(name != NULL) {
						newnode->errnum = DW_ERR_EXEC_TOO_MANY_ARGUMENTS;
						__dw_free_node(r);
						int j;
						for(j = 0; fields[j] != NULL; j++) free(fields[j]);
						free(fields);
						free(fieldtypes);
						if(name != NULL) free(name);
						return newnode;
					}
					if(name == NULL) {
						name = __dw_strdup(r->string);
					}
					used = true;
				} else if(__dw_strcaseequ(node->ident, "create_table")) {
					if(name == NULL) {
						name = __dw_strdup(r->string);
					} else {
						char* val = __dw_strdup(r->string);
						int j;
						char cbuf[2];
						cbuf[1] = 0;
						for(j = 0; val[j] != 0; j++) {
							if(val[j] == ':') {
								val[j] = 0;
								cbuf[0] = 0;
								if(__dw_strcaseequ(val, "string")) {
									cbuf[0] = DW_RECORD_STRING;
								} else if(__dw_strcaseequ(val, "integer")) {
									cbuf[0] = DW_RECORD_INTEGER;
								} else if(__dw_strcaseequ(val, "floating")) {
									cbuf[0] = DW_RECORD_FLOATING;
								} else if(__dw_strcaseequ(val, "logical")) {
									cbuf[0] = DW_RECORD_LOGICAL;
								} else if(__dw_strcaseequ(val, "help")) {
									cbuf[0] = DW_RECORD_HELP;
								}
								char* tmp = fieldtypes;
								fieldtypes = __dw_strcat(tmp, cbuf);
								free(tmp);

								char** oldfields = fields;
								int k;
								for(k = 0; oldfields[k] != NULL; k++)
									;
								fields = malloc(sizeof(*fields) * (k + 2));
								for(k = 0; oldfields[k] != NULL; k++) {
									fields[k] = oldfields[k];
								}
								fields[k] = __dw_strdup(val + j + 1);
								fields[k + 1] = NULL;
								break;
							}
						}
						free(val);
					}
					used = true;
				} else if(__dw_strcaseequ(node->ident, "insert")) {
				} else {
					int j;
					for(i = 0; fields[i] != NULL; i++) free(fields[i]);
					free(fields);
					free(fieldtypes);
					if(name != NULL) free(name);
					newnode->errnum = DW_ERR_EXEC_UNKNOWN_METHOD;
					__dw_free_node(r);
					return newnode;
				}
				__dw_free_node(r);
			}
		}
		if(__dw_strcaseequ(node->ident, "create_table")) {
			if(name != NULL) {
				newnode->errnum = dataworks_database_create_table(db, name, fields, fieldtypes);
			} else {
				newnode->errnum = DW_ERR_EXEC_INSUFFICIENT_ARGUMENTS;
			}
		} else if(__dw_strcaseequ(node->ident, "print")) {
			printf("\n");
		} else if(__dw_strcaseequ(node->ident, "use")) {
			if(name != NULL) {
				newnode->errnum = dataworks_database_use_table(db, name);
				if(dolog) printf("Using table `%s`.\n", name);
			} else {
				newnode->errnum = DW_ERR_EXEC_INSUFFICIENT_ARGUMENTS;
			}
		} else if(__dw_strcaseequ(node->ident, "insert")) {
			for(i = 0; node->nodes[i] != NULL; i++)
				;

			char** list = dataworks_database_get_table_fields(db, dataworks_database_get_using_table(db));

			int count;
			for(count = 0; list[count] != NULL; count++) free(list[count]);
			free(list);

			if(i < count) {
				newnode->errnum = DW_ERR_EXEC_INSUFFICIENT_ARGUMENTS;
			} else if(i > count) {
				newnode->errnum = DW_ERR_EXEC_TOO_MANY_ARGUMENTS;
			} else {
				char* typ = dataworks_database_get_table_field_types(db, dataworks_database_get_using_table(db));
				struct Node** arr = malloc(sizeof(*arr) * (i + 1));
				for(i = 0; node->nodes[i] != NULL; i++) {
					struct Node* r = parser_process(db, node->nodes[i], false);
					char t = __dw_get_node_type(r);
					if(!(
						(typ[i] == 'F' || typ[i] == 'I') && t == 'N'
					||	typ[i] == t
					)){
						newnode->errnum = DW_ERR_EXEC_TYPE_MISMATCH;
					}
					arr[i] = r;
				}
				arr[i] = NULL;

				if(newnode->errnum == DW_ERR_SUCCESS){
					void** fields = malloc(sizeof(*fields) * (i + 1));
					fields[i] = NULL;
					int k;
					for(k = 0; k < i; k++){
						if(typ[k] == DW_RECORD_STRING){
							fields[k] = __dw_strdup(arr[k]->string);
						}else if(typ[k] == DW_RECORD_LOGICAL){
							fields[k] = malloc(1);
							*(char*)fields[k] = arr[k]->logical;
						}else if(typ[k] == DW_RECORD_FLOATING){
							fields[k] = malloc(sizeof(double));
							*(double*)fields[k] = arr[k]->number;
						}else if(typ[k] == DW_RECORD_INTEGER){
							fields[k] = malloc(8);
							*(uint64_t*)fields[k] = arr[k]->number;
						}
					}
					struct dataworks_db_result* insr = dataworks_database_insert_record(db, fields);

					free(fields);
				}

				for(i = 0; arr[i] != NULL; i++) {
					__dw_free_node(arr[i]);
				}
				free(typ);
				free(arr);
			}
		} else if(!used) {
			newnode->errnum = DW_ERR_EXEC_UNKNOWN_METHOD;
		}
		if(name != NULL) free(name);
		for(i = 0; fields[i] != NULL; i++) free(fields[i]);
		free(fields);
		free(fieldtypes);
		return newnode;
	} else {
		struct Node* n = __dw_duplicate_node(node);
		n->errnum = DW_ERR_SUCCESS;
		n->result = NULL;
		return n;
	}
}

struct dataworks_db_result* dataworks_database_execute_code(struct dataworks_db* db, const char* code, bool dolog) {
	struct dataworks_db_result* r = malloc(sizeof(*r));
	r->error = false;
	r->value = NULL;

	struct Node* node;
	if((node = __dw_parser_parse(code, dolog)) == NULL) {
		r->error = true;
		r->errnum = DW_ERR_PARSER_FAIL;
	} else {
		if(dolog) {
			bool pr = __dw_print_node(node, true);
			if(pr) printf("\n");
		}
		struct Node* ret = parser_process(db, node, dolog);
		if(ret->errnum == DW_ERR_SUCCESS) {
			bool pr = __dw_print_node(ret, true);
			if(pr) printf("\n");
		} else {
			r->error = true;
			r->errnum = ret->errnum;
		}
		__dw_free_node(ret);
		__dw_free_node(node);
	}

	return r;
}

void dataworks_database_free_result(struct dataworks_db_result* result) {
	if(result->value != NULL) free(result->value);
	free(result);
}
