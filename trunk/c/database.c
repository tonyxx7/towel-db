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

int
main()
{
	toweldb_db* db = NULL;
	db = toweldb_open( "/home/andrew/bah", 'c' );
	printf( "%u\n", toweldb_get_num_recs( db ));
	toweldb_close( db );
	
	return 0;
}

toweldb_db*
toweldb_open( char* path, const char mode )
{	
	/* Information about the filesystem */
	struct stat path_info;
	struct statvfs fs_info;
	
	/* The actual database pointer */
	toweldb_db* db = NULL;
	
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
	db->path = path;
	db->db_dir = opendir( path );
	db->error = toweldb_err_noerror;
	
	/* Now we need to warn the developer if the filesystem used is limiting */
	statvfs( path, &fs_info );
	if( fs_info.f_namemax < TOWELDB_MAX_KEY_LEN )
	{
		db->max_key_len = fs_info.f_namemax;
	}
	
	return db;
}

toweldb_err
toweldb_drop( toweldb_db* db )
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
toweldb_close( toweldb_db* db )
{
	closedir( db->db_dir );
	free( db );
}
