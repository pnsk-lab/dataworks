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
#include <string.h>

struct __dw_token* __dw_parser_parse(const char* str) {
	struct __dw_token* token = malloc(sizeof(*token));
	token->error = false;
	token->errnum = DW_ERR_SUCCESS;
	token->token = NULL;
	token->name = NULL;
	token->type = __DW_METHOD;
	char* buf = malloc(1);
	buf[0] = 0;
	int i;
	bool dq = false;
	char cbuf[2];
	cbuf[1] = 0;
	int brace = 0;
	char* br = malloc(1);
	bool has_brace = false;
	br[0] = 0;
	for(i = 0; str[i] != 0; i++) {
		cbuf[0] = str[i];
		if(brace > 0) {
			if(str[i] == '(') brace++;
			if(str[i] == ')') brace--;
			if(brace > 0) {
				if(str[i] == ',') {
					i++;
					for(; str[i] != 0 && (str[i] == '\t' || str[i] == ' '); i++)
						;
					i--;
				}
				char* tmp = br;
				br = __dw_strcat(tmp, cbuf);
				free(tmp);
			} else {
				int j;
				for(j = 0; buf[j] != 0; j++) {
					if(buf[j] != ' ' && buf[j] != '\t') {
						char* newbuf = __dw_strdup(buf + j);
						free(buf);
						buf = newbuf;
						break;
					}
				}
				for(j = 0; br[j] != 0; j++) {
					if(br[j] != ' ' && br[j] != '\t') {
						char* newbr = __dw_strdup(br + j);
						free(br);
						br = newbr;
						break;
					}
				}
				for(j = strlen(buf) - 1; j >= 0; j--) {
					if(buf[j] != ' ' && buf[j] != '\t') {
						buf[j + 1] = 0;
						break;
					}
				}
				for(j = strlen(br) - 1; j >= 0; j--) {
					if(br[j] != ' ' && br[j] != '\t') {
						br[j + 1] = 0;
						break;
					}
				}
				if(buf[0] == ',') {
					char* newbuf = __dw_strdup(buf + 1);
					free(buf);
					buf = newbuf;
				}
				token->name = __dw_strdup(buf);
				int k;
				char* comma = malloc(1);
				comma[0] = 0;
				int intbrace = 0;
				for(k = 0;; k++) {
					char c = br[k];
					if(c == '(' || c == ')' || intbrace > 0) {
						if(c == '(') intbrace++;
						if(c == ')') intbrace--;
						cbuf[0] = c;
						char* tmp = comma;
						comma = __dw_strcat(tmp, cbuf);
						free(tmp);
					} else if(c == 0 || c == ',') {
						if(strlen(comma) > 0){
							j = 0;
							if(token->token != NULL) {
								for(j = 0; token->token[j] != NULL; j++)
									;
							}
							struct __dw_token** newtokens = malloc(sizeof(*newtokens) * (j + 2));
							if(token->token != NULL) {
								for(j = 0; token->token[j] != NULL; j++) {
									newtokens[j] = token->token[j];
								}
								free(token->token);
							}
							token->token = newtokens;
							token->token[j] = __dw_parser_parse(comma);
							token->token[j + 1] = NULL;
							free(comma);
							comma = malloc(1);
							comma[0] = 0;
						}
						if(c == 0) break;
					} else {
						cbuf[0] = c;
						char* tmp = comma;
						comma = __dw_strcat(tmp, cbuf);
						free(tmp);
					}
				}
				free(comma);
				free(br);
				br = malloc(1);
				br[0] = 0;
				free(buf);
				buf = malloc(1);
				buf[0] = 0;
			}
		} else if(dq) {
			if(str[i] == '"') {
				dq = !dq;
			} else {
				char* tmp = buf;
				buf = __dw_strcat(tmp, cbuf);
				free(tmp);
			}
		} else if(str[i] == '(') {
			has_brace = true;
			brace++;
		} else if(str[i] == ')') {
			brace--;
		} else if(str[i] == '"') {
			dq = !dq;
		} else if(str[i] == ',') {
			free(buf);
			buf = malloc(1);
			buf[0] = 0;
		} else {
			char* tmp = buf;
			buf = __dw_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	if(!has_brace) {
		token->type = __DW_VALUE;
		token->name = __dw_strdup(buf);
	}
	free(br);
	free(buf);
	return token;
}

void __dw_parser_free(struct __dw_token* token) {
	free(token->name);
	if(token->type == __DW_METHOD) {
		if(token->token != NULL) {
			int i;
			for(i = 0; token->token[i] != NULL; i++) __dw_parser_free(token->token[i]);
			free(token->token);
		}
	}
	free(token);
}
