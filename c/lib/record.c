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

#include "record.h"
#include "toweldb.h"

bool toweldb_is_record_real( toweldb_db* db, const char* key );
	/* Check to see if a key is worth returning as a record key or not */

char*
toweldb_get_path( toweldb_db* db, const char* key )
{
	char* path = NULL;
	
	path = malloc( sizeof( char ) * \
		( strlen( db->path ) + strlen( key ) + 1 ));
	strcpy( path, db->path );
	strcat( path, key );
	
	return path;
}

bool
toweldb_is_record_real( toweldb_db* db, const char* key )
{
	struct stat file_info;
	char* path = NULL;
	
	path = toweldb_get_path( db, key );
	
	/* First let's try getting the file information.  I can't imagine why it
	 * would fail, but if it does, we don't want it to be considered valid */
	if( stat( path, &file_info ))
	{
		free( path );
		return false;
	}
	
	/* We don't need the path anymore */
	free( path );
	
	/* Now we check if it's a real file or not, and if it's an internally
	 * reserved record */
	if( S_ISREG( file_info.st_mode )	\
		&& strcmp( key, TOWELDB_META ))
	{
		return true;
	}
	else
	{
		return false;
	}
}

unsigned int
toweldb_get_num_recs( toweldb_db* db )
{
	DIR* db_dir = NULL;
	struct dirent* dir_entry = NULL;
	unsigned int n_recs = 0;

	/* Open the database directory stream */
	db_dir = opendir( db->path );
	
	/* Now we just loop through the database keys until we hit NULL */
	while(( dir_entry = readdir( db_dir )) != NULL )
	{
		if( toweldb_is_record_real( db, dir_entry->d_name ))
		{
			n_recs++;
		}
	}
	
	/* Wrap it up */
	closedir( db_dir );
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
		if( toweldb_is_record_real( db, dir_entry->d_name ))
		{
			found = 1;
		}
	};
	
	/* Close the directory list and wrap up */
	return dir_entry->d_name;
}

toweldb_err
toweldb_create_rec( toweldb_db* db, const char* key )
{
	FILE* rec = NULL;
	
	/* Let's first check if the key is too long */
	if( strlen( key ) > db->max_key_len )
	{
		return toweldb_err_key_too_long;
	}
	
	if(( rec = fopen( key, "w" )) != NULL )
	{
		fclose( rec );
	}
	else
	{
		return toweldb_err_write_error;
	}
	
	return toweldb_err_noerror;
}

toweldb_err
toweldb_remove_rec( toweldb_db* db, const char* key )
{
	if( remove( key ))
	{
		return toweldb_err_delete_error;
	}
	else
	{
		return toweldb_err_noerror;
	}
}

time_t toweldb_record_get_time( toweldb_rec rec )
{
	struct stat rec_info;
	stat( toweldb_get_path( rec->parent, rec->key ), &rec_info );
	
	return rec_info.st_mtime;
}
