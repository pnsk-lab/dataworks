/* $Id: dw_database.h 7 2024-05-16 15:56:44Z nishi $ */
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

#ifndef __DATAWORKS_DW_DATABASE_H__
#define __DATAWORKS_DW_DATABASE_H__

/**
 * @file dw_database.h
 * @~english
 * @brief DataWorks database
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

/**
 * @~english
 * @brief Database struct
 *
 */
struct dataworks_db {
	/**
	 * @~english
	 * @brief File pointer to the database file.
	 *
	 */
	FILE* fp;

	/**
	 * @~english
	 * @brief Version of the database.
	 *
	 */
	uint16_t version;

	/**
	 * @~english
	 * @brief Last modified time of the database.
	 *
	 */
	uint64_t mtime;
};

/**
 * @~english
 * @brief Creates the database.
 * @param fname Filename
 * @return 0 if successful
 *
 */
int dataworks_database_create(const char* fname);

/**
 * @~english
 * @brief Opens the database.
 * @param fname Filename
 * @return
 * - `NULL` if it failed to open the database.
 * - non-`NULL` if it could open the database.
 *
 */
struct dataworks_db* dataworks_database_open(const char* fname);

/**
 * @~english
 * @brief Get the version of the database.
 * @param db Database
 * @return Version of the database
 *
 */
int dataworks_database_get_version(struct dataworks_db* db);

/**
 * @~english
 * @brief Get the last modified time of the database.
 * @param db Database
 * @return Last modified time of the database
 *
 */
uint64_t dataworks_database_get_mtime(struct dataworks_db* db);

#ifdef __cplusplus
}
#endif

#endif
