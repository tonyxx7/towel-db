/* toweldb - A human readable database system.
 * Copyright (C) 2008  Andrew <i80and@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOWELDB_DATABASE_H
#define TOWELDB_DATABASE_H

#include "toweldb.h"

/* Database */
static typedef struct
{
	DIR* db_dir;
	char* path;
	char mode;
	toweldb_err error;
	char max_key_len;
} toweldb_db;

/* Database functions */
toweldb_db* toweldb_open( const char* path, const char mode );
	/* Open a database located at path.  If mode is 'c', create it if needed */
toweldb_err toweldb_drop( toweldb_db* db );
	/* Delete the database.  WARNING: BROKEN */
void toweldb_close( toweldb_db* db );
	/* Close a database and free the memory storing it. */

#endif
