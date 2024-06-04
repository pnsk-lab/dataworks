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
#include <string.h>

void print_node(struct Node* node, bool top) {
	printf("%s(", node->ident);
	if(node->nodes != NULL) {
		int i;
		for(i = 0; node->nodes[i] != NULL; i++) {
			if(i > 0) printf(", ");
			if(node->nodes[i]->ident != NULL) {
				print_node(node->nodes[i], false);
			} else {
				printf("\"%s\"", node->nodes[i]->string);
			}
		}
	}
	printf(")");
	if(top) printf("\n");
}

void parser_process(struct Node* node, bool dolog) {
	if(node->ident != NULL) {
		if(dolog) print_node(node, true);
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
	}else{
		parser_process(node, dolog);
	}

	return r;
}

void dataworks_database_free_result(struct dataworks_db_result* result) {
	if(result->value != NULL) free(result->value);
	free(result);
}
