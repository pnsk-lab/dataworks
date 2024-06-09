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
#include "dw.tab.h"
#include "grammersym.h"
#include <string.h>
extern YYSTYPE yylval;

double __dw_atof(const char* str);
%}

%%

["][^"]*["]			{
	yylval.node.string = strdup(yytext + 1);
	yylval.node.string[strlen(yylval.node.string) - 1] = 0;
	yylval.node.nodes = NULL;
	yylval.node.ident = NULL;
	yylval.node.number = 123123;
	yylval.node.type = 'S';
	return (STRING);
}
['][^']*[']			{
	yylval.node.string = strdup(yytext + 1);
	yylval.node.string[strlen(yylval.node.string) - 1] = 0;
	yylval.node.nodes = NULL;
	yylval.node.ident = NULL;
	yylval.node.type = 'S';
	return (STRING);
}
[ \t]*#.*			{ return (COMMENT); }
[a-zA-Z_][a-zA-Z0-9_\-]*	{
	yylval.node.ident = strdup(yytext);
	yylval.node.nodes = NULL;
	yylval.node.string = NULL;
	yylval.node.type = 'I';
	return (IDENTIFIER);
}
-?[0-9]+(\.[0-9]+)?		{
	yylval.node.ident = NULL;
	yylval.node.nodes = NULL;
	yylval.node.string = NULL;
	yylval.node.number = __dw_atof(yytext);
	yylval.node.type = 'N';
	return (NUMBER);
}
[\(\),]				{
	return (yytext[0]);
}
[ \t\v]*		{
	return (SPACE);
}

%%

#if defined(FLEX_SCANNER)
#define XSTR(x) #x
#define STR(x) XSTR(x)
const char* lexver = "Flex " STR(YY_FLEX_MAJOR_VERSION) "." STR(YY_FLEX_MINOR_VERSION) "." STR(YY_FLEX_SUBMINOR_VERSION);
#else
const char* lexver = "Unknown";
#endif
