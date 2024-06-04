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

#include "dw_parser.h"

#include "dw_database.h"
#include "dw_util.h"

#include "../Grammar/dw.tab.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

extern int yydebug;
extern int yyparse(void);
extern void* yy_scan_string(const char* str);
extern void yy_delete_buffer(void* buffer);

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

void parser_process(struct Node* node) {
	if(node->ident != NULL) {
		int i;
		print_node(node, true);
	}
}

int __dw_parser_parse(const char* str, bool top) {
#ifdef PARSER_DEBUG
	yydebug = 1;
#endif

	void* buf = yy_scan_string(str);
	if(yyparse() != 0) {
		yy_delete_buffer(buf);
		return DW_ERR_PARSER_FAIL;
	} else {
		extern YYSTYPE yyval;
		parser_process(&yyval.node);
	}
	yy_delete_buffer(buf);

	return DW_ERR_SUCCESS;
}
