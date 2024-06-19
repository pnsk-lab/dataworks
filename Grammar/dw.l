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

%{
#include "dw.tab.h"
#include "grammersym.h"
#include <string.h>
extern YYSTYPE yylval;

double __dw_atof(const char* str);
char* __dw_strcat(const char* a, const char* b);
char* str = NULL;
%}

%x DQ SQ

%%
\"				{
	str = malloc(1);
	str[0] = 0;
	BEGIN(DQ);
}
<DQ>[^"]			{
	char* tmp = str;
	str = __dw_strcat(tmp, yytext);
	free(tmp);
}
<DQ>\\.				{
	char* tmp = str;
	str = __dw_strcat(tmp, yytext + 1);
	free(tmp);
}
<DQ>\"				{
	yylval.node.string = strdup(str);
	yylval.node.nodes = NULL;
	yylval.node.ident = NULL;
	yylval.node.type = 'S';
	BEGIN(INITIAL);
	free(str);
	return (STRING);
}
\'				{
	str = malloc(1);
	str[0] = 0;
	BEGIN(SQ);
}
<SQ>[^']			{
	char* tmp = str;
	str = __dw_strcat(tmp, yytext);
	free(tmp);
}
<SQ>\\.				{
	char* tmp = str;
	str = __dw_strcat(tmp, yytext + 1);
	free(tmp);
}
<SQ>\'				{
	yylval.node.string = strdup(str);
	yylval.node.nodes = NULL;
	yylval.node.ident = NULL;
	yylval.node.type = 'S';
	BEGIN(INITIAL);
	free(str);
	return (STRING);
}
[ \t]*#.*			{ return (COMMENT); }
[a-zA-Z_][a-zA-Z0-9_\-]+	{
	yylval.node.ident = strdup(yytext);
	yylval.node.nodes = NULL;
	yylval.node.string = NULL;
	yylval.node.type = 'I';
	return (IDENTIFIER);
}
[YTNF]				{
	yylval.node.ident = NULL;
	yylval.node.nodes = NULL;
	yylval.node.string = NULL;
	yylval.node.logical = (yytext[0] == 'Y' || yytext[0] == 'T') ? 0xff : 0;
	yylval.node.type = 'L';
	return (LOGICAL);
}
[\+-]?[0-9]+(\.[0-9]+)?		{
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
