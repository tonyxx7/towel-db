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

/* Linked list of record fields */
typedef struct _toweldb_tuple
{
	off_t key_loc;
	off_t value_end;
	unsigned int key_len;
	unsigned long value_len;
	char* key;
	char* value;
	struct _toweldb_tuple* next;
} toweldb_field_node;

/* Record */
typedef struct
{
	time_t read_time;
	toweldb_db* parent;
	char* key;
	toweldb_field_node* contents_start;
} toweldb_rec;

/* Record functions */
char* toweldb_get_path( toweldb_db* db, const char* key );
	/* Get the path to the record specified jointly by the database and the
	 * key */
unsigned int toweldb_get_num_recs( toweldb_db* db );
	/* Get the number of records in the database. */

#endif
