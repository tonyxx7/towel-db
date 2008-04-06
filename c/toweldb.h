/* towel-db - A human readable database system.
 * Copyright (C) 2008  Andrew <i80and@gmail.com>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOWELDB_H
#define TOWELDB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

/* Version of the database implemented */
#define TOWELDB_DB_VERSION 0

/* Errors */
typedef enum
{
	toweldb_err_noerror = 0,
	toweldb_err_nodb,
	toweldb_err_versionmismatch,
	toweldb_err_wrongmode
} toweldb_err;

/* Database */
typedef struct
{
	char* path;
	char mode;
} toweldb_db;

/* Record */
typedef struct
{
	bool changed;
	bool exists;
	char mode;
	char* key;
	toweldb_db* parent;
	char** data_keys;
	char** data_values;
} toweldb_rec;

/* Database stuff */
toweldb_err toweldb_create_db ( char* path, toweldb_db* db );
/* Create a database */

toweldb_err toweldb_open ( char* path, toweldb_db* db );
/* Open an existing database */

toweldb_err toweldb_get_rec ( toweldb_db* db,
								toweldb_rec* rec,
								char* key );
/* Get a record within a toweldb database */

toweldb_err toweldb_create_rec ( toweldb_db* db, char* key );
/* Create a new record at key within the database db */

toweldb_err toweldb_del_rec( toweldb_db* db, char* key );
/* Delete a record within database db */

/* Record stuff */
toweldb_err toweldb_get_field ( toweldb_rec* rec,
								char* field,
								unsigned int len,
								char* data );
/* Get the value of rec[field] and put len bytes of it in data */

toweldb_err toweldb_sync( toweldb_rec* rec );
/* Write the data in the record rec to the disk */

#endif
