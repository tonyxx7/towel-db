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
#include "database.h"

toweldb_db
toweldb_open( const char* path, const char mode )
{	
	/* Information about the filesystem */
	struct stat path_info;
	struct statvfs fs_info;
	
	/* The actual database pointer */
	toweldb_db db = NULL;
	
	/* The path string */
	char* path_final = NULL;
	unsigned int i = 0;
	
	/* First let's make sure that the path given is a valid directory */
	stat( path, &path_info );
	if( !S_ISDIR( path_info.st_mode ))
	{
		if( mode == 'c' )
		{
			/* FIXME: Use default permissions mask here instead of 755 */
			mkdir( path, S_IRWXU|S_IRGRP|S_IROTH|S_IXGRP|S_IROTH|S_IXOTH );
		}
		else
		{
			return NULL;
		}
	}
	
	/* Let's allocate the database */
	db = malloc( sizeof( toweldb_db ));
	db->mode = mode;
	db->db_dir = opendir( path );
	db->error = toweldb_err_noerror;
	/* Give the database a pointer to the path string.  If the last character is
	 * the path join character, just copy it on over.  If not, then we need to
	 * append it ourselves */
	if( path[strlen( path )-1] != TOWELDB_PATH_JOIN )
	{
		/* Allocate the memory needed to hold the path.  Add two for the path
		 * join character and for the ending null character */
		path_final = malloc( sizeof( char ) * \
			( strlen( path ) + 2 ));
		
		/* Copy the string manually and add the path join character */
		while( path[i] != '\0' )
		{
			path_final[i] = path[i];
			i++;
		}
		path_final[i] = TOWELDB_PATH_JOIN;
		path_final[i+1] = '\0';
	}
	else
	{
		/* Just copy the path string right over */
		path_final = malloc( sizeof( char ) * strlen( path ));
		strcpy( path_final, path );
	}
	db->path = path_final;
	
	/* Now we need to set some flags if the filesystem is limiting */
	statvfs( path, &fs_info );
	if( fs_info.f_namemax < TOWELDB_MAX_KEY_LEN )
	{
		db->max_key_len = fs_info.f_namemax;
	}
	/* TODO: Check if the FS is read-only */
	
	return db;
}

DIR** toweldb_get_db_dir( toweldb_db db )
{
	return &db->db_dir;
}

char* toweldb_get_path( toweldb_db db )
{
	return db->path;
}

char toweldb_get_max_key_len( toweldb_db db )
{
	return db->max_key_len;
}

toweldb_err
toweldb_drop( toweldb_db db )
{
	if( rmdir( db->path ))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void
toweldb_close( toweldb_db db )
{
	closedir( db->db_dir );
	free( db->path );
	free( db );
}
