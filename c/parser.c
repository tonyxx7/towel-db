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

#define TOWELDB_COMPONENT_NONE 0
#define TOWELDB_COMPONENT_KEY 1
#define TOWELDB_COMPONENT_DATA 2

#define TOWELDB_PHASE_NULL 0
#define TOWELDB_PHASE_START 1
#define TOWELDB_PHASE_FINISH 3 

toweldb_rec*
toweldb_read_rec( toweldb_db* db, const char* key )
{	
	char* path = NULL;
	FILE* rec_file = NULL;
	toweldb_rec* rec = NULL;
	struct stat rec_info;
	char* rec_contents = NULL;
	toweldb_field_node* cur_node = NULL;
	
	off_t rec_len = 0;
	off_t current_pos = 0;
	off_t current_pos_final = 0;
	char token_phase = TOWELDB_PHASE_NULL;
	char rec_component = TOWELDB_COMPONENT_NONE;
	
	/* Allocate the record */
	rec = malloc( sizeof( toweldb_rec ));
	rec->parent = db;
	rec->key = malloc( sizeof( char ) * ( strlen( key ) + 1 ));
	strcpy( rec->key, key );
	
	/* Allocate the first field node */
	rec->contents_start = malloc( sizeof( toweldb_field_node ));
	cur_node = rec->contents_start;
	cur_node->key_len = 0;
	cur_node->value_len = 0;
	cur_node->key = NULL;
	cur_node->value = NULL;
	cur_node->next = NULL;
	
	/* Open the record */
	path = toweldb_get_path( db, key );
	if(( rec_file = fopen( path, "r" )) == NULL )
	{
		/* Something's wrong.  Let's skip town, shall we? */
		return NULL;
	}
	
	/* Figure out how large the file is so we can load it into memory */
	if( stat( path, &rec_info ))
	{
		return NULL;
	}
	else
	{
		rec_len = rec_info.st_size;
	}
	
	/* Copy the file into memory */
	rec_contents = malloc( sizeof( char ) * rec_len );
	for( current_pos = 0; current_pos < rec_len; current_pos++ )
	{
		rec_contents[current_pos] = fgetc( rec_file );
		
		/* If we've got a complete token, then mark our location */
		if( token_phase == TOWELDB_PHASE_FINISH )
		{
			/* If we haven't hit a key yet, then we don't want to allocate a
			 * new data structure */
			if( rec_component != TOWELDB_COMPONENT_NONE )
			{
				/* Our length calculations for the previous data includes the
				 * header token.  Let's ditch that... */
				cur_node->value_len -= TOWELDB_PHASE_FINISH;
				
				/* We need a new field node */
				cur_node->next = malloc( sizeof( toweldb_field_node ));
				cur_node = cur_node->next;
				cur_node->key_len = 0;
				cur_node->value_len = 0;
				cur_node->key = NULL;
				cur_node->value = NULL;
				cur_node->next = NULL;
			}
			
			/* Mark the location of the end of the token */
			cur_node->key_loc = current_pos;
			
			/* It's official: we are in a key */
			rec_component = TOWELDB_COMPONENT_KEY;
		}
		
		/* Parsing works kind of crazily currently.  A header token has three
		 * characters: \n%%.  token_phase holds the number of characters that
		 * have been matched so far.  So here we look at that */
		if( rec_contents[current_pos] == '\n' )
		{
			/* If we're already in a key, then this should end it */
			if( rec_component == TOWELDB_COMPONENT_KEY )
			{
				rec_component = TOWELDB_COMPONENT_DATA;
			}
						
			if( !token_phase )
				token_phase = TOWELDB_PHASE_START;
		}
		else if( rec_contents[current_pos] == '%' )
		{
			if( token_phase < TOWELDB_PHASE_FINISH )
				token_phase++;
		}
		else
		{
			token_phase = TOWELDB_PHASE_NULL;
		}

		/* Increment the length variables as needed */
		if( rec_component == TOWELDB_COMPONENT_KEY )
		{
			cur_node->key_len++;
		}
		else if( rec_component == TOWELDB_COMPONENT_DATA )
		{
			/* We aren't in a key, so we need to increment the data length */
			cur_node->value_len++;
		}
	}

	/* Now we have to copy the data into our final structures */
	cur_node = rec->contents_start;
	while( cur_node != NULL )
	{	
		/* First we copy the field key */
		current_pos_final = 0;
		/* We need to allocate the key array, but add one to allow for the nul
		 * character at the end. */
		cur_node->key = malloc( sizeof( char ) * cur_node->key_len+1 );
		for( current_pos = cur_node->key_loc; \
			current_pos < (cur_node->key_loc + cur_node->key_len); \
			current_pos++ )
		{
			cur_node->key[current_pos_final] = rec_contents[current_pos];
			current_pos_final++;
		}
		/* Add the nul character to indicate the end of the key */
		cur_node->key[current_pos_final] = '\0';

		/* Go onto the next node */
		printf( "%s\n", cur_node->key );
		/*printf( "%s\n", cur_node->value );*/
		cur_node = cur_node->next;
	}

	/* Wrap up */
	fclose( rec_file );
	free( path );
	free( rec_contents );
	
	return rec;
}

void
toweldb_free_rec( toweldb_rec* rec )
{
	toweldb_field_node* cur_node = NULL;
	toweldb_field_node* next_node = NULL;
	
	/* First we need to free the key/value sets */
	cur_node = rec->contents_start;
	do
	{
		if( cur_node->key != NULL )
			free( cur_node->key );
		if( cur_node->value != NULL )
			free( cur_node->value );
		
		next_node = cur_node->next;
		free( cur_node );
	} while(( cur_node = next_node ) != NULL );
	
	free( rec->key );
	free( rec );
}
