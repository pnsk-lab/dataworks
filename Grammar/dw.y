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
		struct Node* nodes;
	} node;
}

%%

argument
	: STRING {
		printf("%s\n", $<node>1.string);
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
	: single_argument
	| arguments ',' single_argument
	| SPACE
	| ;
	;

command
	: IDENTIFIER SPACE '(' arguments ')' {
		printf("%s\n", $<node>1.ident);
	}
	| IDENTIFIER '(' arguments ')' {
		printf("%s\n", $<node>1.ident);
	}
	;

%%

#if defined(YYBISON)
const char* yaccver = "GNU Bison " YYBISON_VERSION;
#elif defined(YYBYACC)
#define XSTR(x) #x
#define STR(x) XSTR(x)
const char* yaccver = "Berkeley Yacc " STR(YYMAJOR) "." STR(YYMINOR);
#else
const char* yaccver = "Unknown";
#endif