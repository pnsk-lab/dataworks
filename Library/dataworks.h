/* $Id$ */
/* --- START LICENSE --- */
/* -------------------------------------------------------------------------- */
/* Copyright (c) 2024 Nishi.                                                  */
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

#ifndef __DATAWORKS_DATAWORKS_H__
#define __DATAWORKS_DATAWORKS_H__

/**
 * @file dataworks.h
 * @~english
 * @brief DataWorks info
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @~english
 * @brief Get the version of DataWorks
 * @return Version of DataWorks
 *
 */
const char* dataworks_get_version(void);

/**
 * @~english
 * @brief Get the compilation date of DataWorks
 * @return Compilation date of DataWorks
 *
 */
const char* dataworks_get_compile_date(void);

/**
 * @~english
 * @brief Get the compilation platform of DataWorks
 * @return Compilation platform of DataWorks
 *
 */
const char* dataworks_get_platform(void);

/**
 * @~english
 * @brief Get the Endian of the platform
 * @return Endian of the platform
 *
 */
char dataworks_get_endian(void);

/**
 * @~english
 * @brief Get the copyright of DataWorks
 * @return Copyright of DataWorks
 *
 */
const char* dataworks_get_copyright(void);

#ifdef __cplusplus
}
#endif

#endif
