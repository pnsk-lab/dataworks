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

#ifdef __WATCOMC__
#define PACKED _Packed
#else
#define PACKED __attribute__((__packed__))
#endif

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
 * @brief indexentry for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
PACKED
struct dataworks_db_v1_indexentry {
	uint8_t flag;
	uint64_t count;
	uint8_t dbname_len;
	char dbname[256];
	char fields[4096];
};

/**
 * @~english
 * @brief "Used" bitmask for indexentry for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
#define DATAWORKS_V1_INDEXENTRY_USED (1 << 7)

/**
 * @~english
 * @brief infoentry for database.
 * @note See \ref FORMATv1 for more info.
 *
 */
PACKED
struct dataworks_db_infoentry {
	uint16_t version;
	uint64_t mtime;
};

/**
 * @~english
 * @brief Creates the database.
 * @param fname Filename
 * @return 0 if successful
 *
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

/**
 * @~english
 * @brief Get the table list of the database.
 * @param db Database
 * @return Table list of the databas
 *
 */
char** dataworks_database_get_table_list(struct dataworks_db* db);

#ifdef __cplusplus
}
#endif

#endif
