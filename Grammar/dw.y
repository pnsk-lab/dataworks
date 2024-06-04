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

%{
#include "grammersym.h"
#include <stdio.h>
%}

%token IDENTIFIER STRING SPACE
%start command

%union {
	struct Node {
		char* string;
		char* ident;
		struct Node** nodes;
	} node;
}

%{
void parser_process(struct Node* node);
char* __dw_strdup(const char* a);
%}

%%

argument
	: STRING {
		$<node>$.string = $<node>1.string;
		$<node>$.nodes = $<node>1.nodes;
		$<node>$.ident = $<node>1.ident;
	}
	| command {
		$<node>$ = $<node>1;
	}
	;

single_argument
	: SPACE argument SPACE {
		$<node>$ = $<node>2;
	}
	| SPACE argument {
		$<node>$ = $<node>2;
	}
	| argument SPACE {
		$<node>$ = $<node>1;
	}
	| argument {
		$<node>$ = $<node>1;
	}
	;

arguments
	: single_argument {
		$<node>$.nodes = malloc(sizeof(*$<node>$.nodes) * 2);
		$<node>$.nodes[0] = malloc(sizeof($<node>$));
		$<node>$.nodes[0]->nodes = NULL;
		$<node>$.nodes[0]->ident = NULL;
		$<node>$.nodes[0]->string = NULL;
		if($<node>1.ident != NULL) $<node>$.nodes[0]->ident = __dw_strdup($<node>1.ident);
		if($<node>1.string != NULL) $<node>$.nodes[0]->string = __dw_strdup($<node>1.string);
		if($<node>1.nodes != NULL) $<node>$.nodes[0]->nodes = $<node>1.nodes;
		$<node>$.nodes[1] = NULL;
	}
	| arguments ',' single_argument {
		struct Node** old_nodes = $<node>$.nodes;
		int i;
		for(i = 0; old_nodes[i] != NULL; i++);
		$<node>$.nodes = malloc(sizeof(*$<node>$.nodes) * (i + 2));
		for(i = 0; old_nodes[i] != NULL; i++) $<node>$.nodes[i] = old_nodes[i];
		$<node>$.nodes[i] = malloc(sizeof($<node>$));
		$<node>$.nodes[i]->nodes = NULL;
		$<node>$.nodes[i]->ident = NULL;
		$<node>$.nodes[i]->string = NULL;
		if($<node>3.ident != NULL) $<node>$.nodes[i]->ident = __dw_strdup($<node>3.ident);
		if($<node>3.string != NULL) $<node>$.nodes[i]->string = __dw_strdup($<node>3.string);
		if($<node>3.nodes != NULL) $<node>$.nodes[i]->nodes = $<node>3.nodes;
		$<node>$.nodes[i + 1] = NULL;
		free(old_nodes);	
	}
	| SPACE
	| ;
	;

command
	: IDENTIFIER SPACE '(' arguments ')' {
		$<node>$.string = NULL;
		$<node>$.ident = $<node>1.ident;
		$<node>$.nodes = $<node>4.nodes;
	}
	| IDENTIFIER '(' arguments ')' {
		$<node>$.string = NULL;
		$<node>$.ident = $<node>1.ident;
		$<node>$.nodes = $<node>3.nodes;
	}
	;

%%

#if defined(YYBISON)
const char* yaccver = "GNU Bison " YYBISON_VERSION;
#error "GNU Bison does not work. Use Berkeley Yacc."
#elif defined(YYBYACC)
#define XSTR(x) #x
#define STR(x) XSTR(x)
const char* yaccver = "Berkeley Yacc " STR(YYMAJOR) "." STR(YYMINOR);
#else
const char* yaccver = "Unknown";
#endif
