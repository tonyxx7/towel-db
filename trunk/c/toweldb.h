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
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

/* Version of the database implemented */
#define TOWELDB_DB_VERSION 0

/* Various utility macros */
#define TOWELDB_MAX_KEY_LEN 255
#define TOWELDB_META "_db"

/* Errors */
typedef char toweldb_err;
enum _toweldb_err
{
	toweldb_err_noerror = 0,		/* Everything's just dandy */
	toweldb_err_fs_limits,			/* The filesystem is limiting us somehow */
	toweldb_err_versionmismatch,	/* The database is of a different version */
};

/* Record write cache entry */
typedef struct
{
	char* key;
	char* value;
} toweldb_tuple;

/* Database */
typedef struct
{
	DIR* db_dir;
	toweldb_tuple* cached_writes;
	char* path;
	char mode;
	toweldb_err error;
	char max_key_len;
} toweldb_db;

/* Record */
typedef struct
{
	toweldb_db* parent;
	char* key;
	char** data_keys;
	char** data_values;
} toweldb_rec;

/* Database stuff */
toweldb_db* toweldb_open( char* path, const char mode );
	/* Open a database located at path.  If mode is 'c', create it if needed */
toweldb_err toweldb_drop( toweldb_db* db );
	/* Delete the database.  WARNING: BROKEN */
void toweldb_close( toweldb_db* db );
	/* Close a database and free the memory storing it. */
	
/* Record functions */
unsigned int toweldb_get_num_recs( toweldb_db* db );
	/* Get the number of records in the database.  FIXME: This will rewind
	 * to the start of the database, and mess up toweldb_get_next_key in some
	 * cases */
char* toweldb_get_next_key( toweldb_db* db );
	/* Get the next key in the database.  This is a wrapper around the POSIX
	 * readdir that skips entries that the programmer doesn't need.  It will
	 * return NULL and rewind if it hits the last item in the directory. */
toweldb_err toweldb_create_rec( toweldb_db* db, const char* key );
	/* Create a new record within the database with the given key. */

#endif
