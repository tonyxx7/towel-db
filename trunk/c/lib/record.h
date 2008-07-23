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

#ifndef TOWELDB_RECORD_H
#define TOWELDB_RECORD_H

#include "toweldb.h"

struct _toweldb_rec
{
	toweldb_db parent;
	char* key;
	toweldb_field_node* contents_start;
};

/* Record functions */
char* toweldb_get_record_path( toweldb_db db, const char* key );
	/* Get the path to the record specified jointly by the database and the
	 * key */

#endif
