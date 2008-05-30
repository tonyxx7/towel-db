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

const char* TOWELDB_HEADER_TOKEN = "\n%%";

void toweldb_push_stack( char* stack, unsigned short stack_len, char new_char )
{
	unsigned short i = 0;
	unsigned short stack_max = (stack_len - 2);
	
	/* Push back the current stack */
	for( i = 0; i < stack_max; i++ )
	{
		stack[i] = stack[i+1];
	}
	
	/* Add the new char */
	stack[stack_max] = new_char;
	stack[stack_max+1] = '\0';
}

toweldb_rec*
toweldb_read_rec( toweldb_db* db, const char* key )
{
	char* path = NULL;
	toweldb_rec* rec = NULL;
	FILE* rec_f = NULL;
	const short stack_len = 4;
	
	/* Array allocation information */
	unsigned int n_fields = 0;
	unsigned int header_len = 0;
	unsigned long field_len = 0;
	
	/* Parsing data */
	toweldb_field_node* cur_node = NULL;
	bool header = true;
	bool in_field = false;
	unsigned int i = 0;
	char current_char = 0;
	
	/* A small cache stack to store recently read characters */
	char* read_cache = malloc( sizeof( char ) * stack_len );
	for( i = 0; i < (stack_len-1); i++ )
	{
		read_cache[i] = 0;
	}
	read_cache[stack_len-1] = '\0';
	
	/* Allocate the data structure */
	rec = malloc( sizeof( toweldb_rec ));
	rec->parent = db;
	rec->key = malloc( sizeof( char ) * ( strlen( key ) + 1 ));
	strcpy( rec->key, key );
	
	rec->contents_start = malloc( sizeof( toweldb_field_node ));
	cur_node = rec->contents_start;
	cur_node->key = NULL;
	cur_node->value = NULL;
	cur_node->next = NULL;
	
	/* Open the record */
	path = toweldb_get_path( db, key );
	if(( rec_f = fopen( path, "r" )) == NULL )
	{
		/* Something's wrong.  Let's skip town, shall we? */
		return NULL;
	}
	
	/* Parse the record.  I admit it: this is the most bizarre code that has no
	 * business actually working.  Basically, the main loop only exists to
	 * stop the parsing at the end of the file and to start the various
	 * sub-loops. */
	/* TODO: Allow escaping */
	
	/* Prepare for the loop */
	/* Push back the current character stack */
	toweldb_push_stack( read_cache, stack_len, fgetc( rec_f ));
	
	while( read_cache[2] != EOF )
	{	
		/* Figure out if we've got a header on our hands */
		if( !strcmp( read_cache, TOWELDB_HEADER_TOKEN ))
		{
			header = true;
			in_field = true;
		}
		else
		{
			header = false;
		}
		
		if( header )
		{
			/* Initialize the current_char variable for this iteration */
			current_char = read_cache[2];
			
			/* Figure out how long the header is */
			while( current_char != '\n' )
			{
				current_char = fgetc( rec_f );
				header_len++;
			};
			
			/* Go back to the start of the header */
			fseek( rec_f, (-header_len), SEEK_CUR );
			
			/* Allocate the header string */
			cur_node->key = malloc( sizeof( char ) * ( header_len + 1 ));
			
			/* Copy the header string into the allocated memory block.  However,
			 * header_len needs to be one less since our array counts from 0 */
			header_len--;
			for( i = 0; i < header_len; i++ )
			{
				cur_node->key[i] = fgetc( rec_f );
			}
			cur_node->key[i] = '\0';
			
			/* Skip to the next line */
			fseek( rec_f, 1, SEEK_CUR );
			
			/* Allocate the next field node */
			cur_node->next = malloc( sizeof( toweldb_field_node ));
			cur_node->next->key = NULL;
			cur_node->next->value = NULL;
			cur_node->next->next = NULL;
			cur_node = cur_node->next;
			
			/* Reset our variables */
			header_len = 0;
		}
		else if( in_field )
		{
			/* We need to read the contents of the field.  First let's figure
			 * out how long it is */
			printf( "%c", read_cache[2] );
		}
		
		/* Go to the next character in the file */
		toweldb_push_stack( read_cache, stack_len, fgetc( rec_f ));
	}
	
	/* Wrap up */
	fclose( rec_f );
	free( path );
	free( read_cache );
	
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
