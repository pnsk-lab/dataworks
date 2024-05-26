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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __dw_buffer_to_db_v1_indexentry(buf, index) \
	memcpy(&index.flag, buf, 1); \
	uint64_t be_count; \
	memcpy(&be_count, buf + 1, 8); \
	__dw_native_endian(be_count, uint64_t, index.count = __converted); \
	memcpy(&index.dbname_len, buf + 1 + 8, 1); \
	memcpy(index.dbname, buf + 1 + 8 + 1, 256); \
	memcpy(index.fields, buf + 1 + 8 + 1 + 256, 4096);

#define __dw_buffer_to_db_v1_dbentry(buf, dbent) \
	memcpy(&dbent.flag, buf, 1); \
	uint64_t be; \
	memcpy(&be, buf + 1, 8); \
	__dw_native_endian(be, uint64_t, index.length = __converted); \
	memcpy(&be, buf + 1 + 8, 8); \
	__dw_native_endian(be, uint64_t, index.size = __converted); \
	memcpy(&dbent.field_index, 1 + 8 + 8, 1); \
	memcpy(&dbent.db_index, 1 + 8 + 8 + 1, 1); \
	memcpy(&be, buf + 1 + 8 + 8 + 1 + 1, 8); \
	__dw_native_endian(be, uint64_t, index.count = __converted); \
	memcpy(&be, buf + 1 + 8 + 8 + 1 + 1 + 8, 8); \
	__dw_native_endian(be, uint64_t, index.fragnum = __converted);

/**
 * @~english
 * @brief Error enum
 *
 */
enum DW_ERRORS {
	/**
	 * @~english
	 * @brief Success
	 *
	 */
	DW_ERR_SUCCESS = 0,

	/**
	 * @~english
	 * @brief Used already
	 *
	 */
	DW_ERR_USED,

	/**
	 * @~english
	 * @brief File open fail
	 *
	 */
	DW_ERR_FAIL_FOPEN,

	/**
	 * @~english
	 * @brief Invalid signature
	 *
	 */
	DW_ERR_INVALID_SIGNATURE,

	/**
	 * @~english
	 * @brief Invalid version
	 *
	 */
	DW_ERR_INVALID_VERSION,

	/**
	 * @~english
	 * @brief Parser returned NULL
	 *
	 */
	DW_ERR_PARSER_NULL,

	/**
	 * @~english
	 * @brief Cannot call non-method
	 *
	 */
	DW_ERR_EXEC_NON_METHOD,

	/**
	 * @~english
	 * @brief Unknown method
	 *
	 */
	DW_ERR_EXEC_UNKNOWN_METHOD,

	/**
	 * @~english
	 * @brief Insufficient arguments
	 *
	 */
	DW_ERR_EXEC_INSUFFICIENT_ARGUMENTS,

	/**
	 * @~english
	 * @brief Too many arguments
	 *
	 */
	DW_ERR_EXEC_TOO_MANY_ARGUMENTS,

	/**
	 * @~english
	 * @brief Not used
	 *
	 */
	DW_ERR_NOT_USED,

	/**
	 * @~english
	 * @brief Too many tables
	 *
	 */
	DW_ERR_TOO_MANY_TABLES,

	/**
	 * @~english
	 * @brief Database not selected
	 *
	 */
	DW_ERR_DATABASE_NOT_SELECTED,
};

/**
 * @~english
 * @brief Type enum
 *
 */
enum DW_RECORD_TYPES {
	/**
	 * @~english
	 * @brief String
	 *
	 */
	DW_RECORD_STRING = 'C',

	/**
	 * @~english
	 * @brief Integer
	 *
	 */
	DW_RECORD_INTEGER = 'I',

	/**
	 * @~english
	 * @brief Double
	 *
	 */
	DW_RECORD_DOUBLE = 'D',

	/**
	 * @~english
	 * @brief Logical
	 *
	 */
	DW_RECORD_LOGICAL = 'L',

	/**
	 * @~english
	 * @brief HELP
	 *
	 */
	DW_RECORD_HELP = '?',
};

/*
| String | `S` | ASCII | |
| Integer | `I` | uint64\_t | |
| Double | `D` | double | |
| Logical | `L` | uint8\_t | 0 for false, other value for true |
| Help | `?` | ASCII | Should be just ignored |
*/

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

	/**
	 * @~english
	 * @brief True if this is an error.
	 *
	 */
	bool error;

	/**
	 * @~english
	 * @brief Error number.
	 *
	 */
	int errnum;

	/**
	 * @~english
	 * @brief Database being used.
	 *
	 */
	char* name;
};

/**
 * @~english
 * @brief Database record struct

 */
struct dataworks_db_record {
	/**
	 * @~english
	 * @brief Type. See enum DW_RECORD_TYPES.
	 *
	 */
	char type;
};

/**
 * @~english
 * @brief Database result struct
 *
 */
struct dataworks_db_result {
	/**
	 * @~english
	 * @brief True if this is an error.
	 *
	 */
	bool error;

	/**
	 * @~english
	 * @brief Error number.
	 *
	 */
	int errnum;

	/**
	 * @~english
	 * @brief Value.
	 *
	 */
	char* value;
};

/**
 * @~english
 * @brief `indexentry` for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
struct dataworks_db_v1_indexentry {
	uint8_t flag;
	uint64_t count;
	uint8_t dbname_len;
	char dbname[256];
	char fields[4096];
};

/**
 * @~english
 * @brief `dbentry` for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
struct dataworks_db_v1_dbentry {
	uint8_t flag;
	uint64_t length;
	uint64_t size;
	uint8_t field_index;
	uint8_t db_index;
	uint64_t count;
	uint64_t fragnum;
	uint8_t* data;
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
 * @brief "Used" bitmask for dbentry for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
#define DATAWORKS_V1_DBENTRY_USED (1 << 7)

/**
 * @~english
 * @brief "Fragment" bitmask for dbentry for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
#define DATAWORKS_V1_DBENTRY_FRAGMENT (1 << 6)

/**
 * @~english
 * @brief "Unset" bitmask for dbentry for v1 database.
 * @note See \ref FORMATv1 for more info.
 *
 */
#define DATAWORKS_V1_DBENTRY_UNSET (1 << 5)

/**
 * @~english
 * @brief `infoentry` for database.
 * @note See \ref FORMATv1 for more info.
 *
 */
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
 * @brief Closes the database.
 * @param db Database
 *
 */
void dataworks_database_close(struct dataworks_db* db);

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
 * @brief Get the error flag of the database.
 * @param db Database
 * @return Error flag of the database
 *
 */
bool dataworks_database_get_error(struct dataworks_db* db);

/**
 * @~english
 * @brief Get the error number of the database.
 * @param db Database
 * @return Error number of the database
 *
 */
int dataworks_database_get_error_number(struct dataworks_db* db);

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
 * @return Table list of the database
 *
 */
char** dataworks_database_get_table_list(struct dataworks_db* db);

/**
 * @~english
 * @brief Get the fields of the table of the database.
 * @param db Database
 * @param table Table name
 * @return Fields of the table
 *
 */
char** dataworks_database_get_table_fields(struct dataworks_db* db, const char* table);

/**
 * @~english
 * @brief Creates a table.
 * @param db Database
 * @param name Table name
 * @param fields Fields
 * @param fieldtypes Types
 * @return
 * - `DW_ERR_SUCCESS` if success
 * - `DW_ERR_USED` if the name is already used
 *
 */
int dataworks_database_create_table(struct dataworks_db* db, const char* name, char** fields, const char* fieldtypes);

/**
 * @~english
 * @brief Deletes a table.
 * @param db Database
 * @param name Table name
 * @return
 * - `DW_ERR_SUCCESS` if success
 * - `DW_ERR_NOT_USED` if the name is not used
 *
 */
int dataworks_database_delete_table(struct dataworks_db* db, const char* name);

/**
 * @~english
 * @brief Converts error number to a string.
 * @param n Error number
 * @return Error string
 *
 */
const char* dataworks_database_strerror(int n);

/**
 * @~english
 * @brief Update mtime.
 * @param db Database
 *
 */
void dataworks_database_update_mtime(struct dataworks_db* db);

/**
 * @~english
 * @brief Executes the code.
 * @param db Database
 * @param code Code
 * @param dolog Output the log if true
 * @return Result
 *
 */
struct dataworks_db_result* dataworks_database_execute_code(struct dataworks_db* db, const char* code, bool dolog);

/**
 * @~english
 * @brief Frees the result.
 * @param result Result
 *
 */
void dataworks_database_free_result(struct dataworks_db_result* result);

/**
 * @~english
 * @brief Inserts the record.
 * @param db Database
 * @param records Records
 * @param prop List which contains character `U` or `S`
 * `U` indicates the field is unset
 * @return Result
 *
 */
struct dataworks_db_result* dataworks_database_insert_record(struct dataworks_db* db, char** fields, const char* prop);

#ifdef __cplusplus
}
#endif

#endif
