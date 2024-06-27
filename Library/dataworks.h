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

#ifndef __DATAWORKS_DATAWORKS_H__
#define __DATAWORKS_DATAWORKS_H__

/**
 * @file dataworks.h
 * @~english
 * @brief DataWorks info
 *
 * @~japanese
 * @brief DataWorksの情報
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @~english
 * @brief Get the version of DataWorks
 * @return Version of DataWorks
 *
 * @~japanese
 * @brief DataWorksのバージョンを取得します
 * @return DataWorksのバージョン
 *
 */
const char* dataworks_get_version(void);

/**
 * @~english
 * @brief Get the version of Yacc used by DataWorks
 * @return Yacc version of DataWorks
 *
 * @~japanese
 * @brief DataWorksのコンパイル時に使われたYaccのバージョンを取得します
 * @return DataWorksのコンパイル時に使われたYaccのバージョン
 *
 */
const char* dataworks_get_yacc_version(void);

/**
 * @~english
 * @brief Get the version of Lex used by DataWorks
 * @return Lex version of DataWorks
 *
 * @~japanese
 * @brief DataWorksのコンパイル時に使われたLexのバージョンを取得します
 * @return DataWorksのコンパイル時に使われたLexのバージョン
 *
 */
const char* dataworks_get_lex_version(void);

/**
 * @~english
 * @brief Get the compiler which was used to compile DataWorks
 * @return Compiler which was used to compile DataWorks
 *
 * @~japanese
 * @brief DataWorksのコンパイル時に使われたコンパイラのバージョンを取得します
 * @return DataWorksのコンパイル時に使われたコンパイラのバージョン
 *
 */
char* dataworks_get_compiler(void);

/**
 * @~english
 * @brief Get the compilation date of DataWorks
 * @return Compilation date of DataWorks
 *
 * @~japanese
 * @brief DataWorksのコンパイル日時を取得します
 * @return DataWorksのコンパイル日時
 *
 */
const char* dataworks_get_compile_date(void);

/**
 * @~english
 * @brief Get the compilation platform of DataWorks
 * @return Compilation platform of DataWorks
 *
 * @~japanese
 * @brief DataWorksのコンパイル先環境を取得します
 * @return DataWorksのコンパイル先環境
 *
 */
const char* dataworks_get_platform(void);

/**
 * @~english
 * @brief Get the Endian of the platform
 * @return Endian of the platform
 *
 * @~japanese
 * @brief 実行環境のエンディアンを取得します
 * @return 実行環境のエンディアン
 *
 */
char dataworks_get_endian(void);

/**
 * @~english
 * @brief Get the copyright of DataWorks
 * @return Copyright of DataWorks
 *
 * @~japanese
 * @brief DataWorksのコピーライトを取得します
 * @return DataWorksのコピーライト
 *
 */
const char* dataworks_get_copyright(void);

/**
 * @~english
 * @brief Get if your platform is supported or not
 * @return Supported or not
 *
 * @~japanese
 * @brief あなたの環境がサポートされているか否かを取得します
 * @return サポートされているか否か
 *
 */
bool dataworks_get_if_supported(void);

#ifdef __cplusplus
}
#endif

#endif
