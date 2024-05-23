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

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

struct __dw_token* __dw_parser_parse(const char* name, const char* str) {
	struct __dw_token* token = malloc(sizeof(*token));
	token->error = false;
	token->errnum = DW_ERR_SUCCESS;
	token->token = NULL;
	char* buf = malloc(1);
	buf[0] = 0;
	int i;
	bool dq = false;
	char cbuf[2];
	cbuf[1] = 0;
	int brace = 0;
	char* br = malloc(1);
	br[0] = 0;
	for(i = 0; str[i] != 0; i++) {
		cbuf[0] = str[i];
		if(brace > 0) {
			if(str[i] == '(') brace++;
			if(str[i] == ')') brace--;
			if(brace > 0) {
				char* tmp = br;
				br = __dw_strcat(tmp, cbuf);
				free(tmp);
			} else {
				printf("%s:%s\n", buf, br);
				__dw_parser_parse(buf, br);
				free(br);
				br = malloc(1);
				br[0] = 0;
				free(buf);
				buf = malloc(1);
				buf[0] = 0;
			}
		} else if(dq) {
			char* tmp = buf;
			buf = __dw_strcat(tmp, cbuf);
			free(tmp);
		} else if(str[i] == '(') {
			brace++;
		} else if(str[i] == ')') {
			brace--;
		} else if(str[i] == '"') {
			dq = !dq;
		} else {
			char* tmp = buf;
			buf = __dw_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	free(br);
	free(buf);
	return token;
}
