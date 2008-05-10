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

#include "toweldb.h"

unsigned int
toweldb_get_num_recs( toweldb_db* db )
{
	unsigned int n_recs = 0;

	/* We first need to rewind the database directory stream to the start */
	rewinddir( db->db_dir );
	
	/* Now we just loop through the database keys until we hit NULL */
	while( toweldb_get_next_key( db ) != NULL )
	{
		n_recs++;
	}
	
	return n_recs;
}

char*
toweldb_get_next_key( toweldb_db* db )
{
	struct dirent* dir_entry = NULL;
	char found = 0;
	
	while( !found )
	{
		/* Read the next file/directory on the stack */
		dir_entry = readdir( db->db_dir );
		
		/* First things first; is there anything there?  If not, we need to
		 * rewind */
		if( dir_entry == NULL )
		{
			rewinddir( db->db_dir );
			return NULL;
		}
		
		/* Now, if the item in question is reserved internally or pointless,
		 * go to the next item in the directory stream */
		if( strcmp( dir_entry->d_name, "." )		\
			&& strcmp( dir_entry->d_name, ".." )	\
			&& strcmp( dir_entry->d_name, TOWELDB_META ))
		{
			found = 1;
		}
	};
	
	/* Close the directory list and wrap up */
	return dir_entry->d_name;
}

toweldb_err toweldb_create_rec( toweldb_db* db, const char* key )
{
	
	
	return toweldb_err_noerror;	
}
