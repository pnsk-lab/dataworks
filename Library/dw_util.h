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

#ifndef __DATAWORKS_DW_UTIL_H__
#define __DATAWORKS_DW_UTIL_H__

/**
 * @file dw_util.h
 * @~english
 * @brief DataWorks utils
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>

#include "dataworks.h"
#include "dw_database.h"

char* __dw_strdup(const char* a);
char* __dw_strcat(const char* a, const char* b);
bool __dw_strcaseequ(const char* a, const char* b);
bool __dw_lockfile(struct dataworks_db* db);
bool __dw_unlockfile(struct dataworks_db* db);

#define __dw_xstr(x) #x
#define __dw_str(x) __dw_xstr(x)

/* Converts NE to BE */
#define __dw_big_endian(arg, type, exec) \
	{ \
		type __original = arg; \
		signed char* __ptr = (signed char*)&__original; \
		type __converted; \
		signed char* __converted_ptr = (signed char*)&__converted; \
		int __i; \
		int __endian_check = 1; \
		char __endian = (1 == *(volatile char*)&__endian_check) ? 'L' : 'B'; \
		for(__i = 0; __i < sizeof(type); __i++) { \
			if(__endian == 'L') { \
				__converted_ptr[sizeof(type) - __i - 1] = __ptr[__i]; \
			} else { \
				__converted_ptr[__i] = __ptr[__i]; \
			} \
		} \
		exec; \
	}

/* Converts BE to NE */
#define __dw_native_endian __dw_big_endian

#ifdef __cplusplus
}
#endif

#endif
