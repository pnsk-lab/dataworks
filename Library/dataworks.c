/* $Id$ */
/* --- START LICENSE --- */
/* -------------------------------------------------------------------------- */
/*                                                   DataWorks - Simple DBMS  */
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

#include "dataworks.h"

#include <stdbool.h>
#include <stdio.h>

const char* dataworks_version = "0.1.0";
const char* dataworks_compile_date = __DATE__;
const char* dataworks_copyright = "Copyright (c) Crabware 2024\n"
				  "              pnsk-lab 2024\n"
				  "              Crabware is the software-development division of\n"
				  "              Hinode Gakuen PC-Club. Contact: <nishi@nishi.boats>";

#define SUPPORTED
#if defined(__MINGW32__)
const char* dataworks_platform = "Windows/"
#elif defined(__NetBSD__)
const char* dataworks_platform = "NetBSD/"
#elif defined(__FreeBSD__)
const char* dataworks_platform = "FreeBSD/"
#elif defined(__linux__)
const char* dataworks_platform = "Linux/"
#elif defined(__WATCOMC__)
const char* dataworks_platform = "WatcomC/"
#elif defined(__APPLE__)
const char* dataworks_platform = "MacOSX/"
#else
#undef SUPPORTED
const char* dataworks_platform = "Unknown/"
#endif
    PLATFORM_M " (" PLATFORM_P ")";

extern const char* yaccver;
extern const char* lexver;

const char* dataworks_get_version(void) { return dataworks_version; }
const char* dataworks_get_yacc_version(void) { return yaccver; }
const char* dataworks_get_lex_version(void) { return lexver; }
const char* dataworks_get_compile_date(void) { return dataworks_compile_date; }
const char* dataworks_get_platform(void) { return dataworks_platform; }
char dataworks_get_endian(void) {
	volatile unsigned short n = 1;
	return *(char*)&n == 1 ? 'L' : 'B';
}

const char* dataworks_get_copyright(void) { return dataworks_copyright; }

int yywrap() { return 1; }

int yyerror(const char* err) {
	fprintf(stderr, "Parser error: %s\n", err);
	return 0;
}

bool dataworks_get_if_supported(void) {
#ifdef SUPPORTED
	return true;
#else
	return false;
#endif
}
