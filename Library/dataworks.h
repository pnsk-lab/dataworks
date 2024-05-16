/* $Id$ */
/* --- START LICENSE --- */
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
 * @brief Version of DataWorks
 * @note Use dataworks_get_version.
 *
 */
#define DATAWORKS_VERSION "0.0.0"

/**
 * @~english
 * @brief Get the version of DataWorks
 * @return Version of DataWorks
 *
 */
const char* dataworks_get_version(void);

#ifdef __cplusplus
}
#endif

#endif
