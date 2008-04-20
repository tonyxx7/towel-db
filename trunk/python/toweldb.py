# towel-db - A human readable database system.
# Copyright (C) 2008  Andrew <i80and@gmail.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import string
import __builtin__

_open = __builtin__.open

# Version of the database
DB_VERSION = 0

# Reserved record names
_DB_META = '_db'
DB_RESERVED = [_DB_META]

# 

# Database open flags
DB_READ = 'r'
DB_WRITE = 'w'
DB_CREATE = 'c'

class DBError( Exception ):
	"""Generic database error"""

class DBVersionMismatch( DBError ):
	"""Version of database is different from the version of the towel-db
	library being used"""

class DBWrongMode( DBError ):
	'Incorrect mode for the requested operation'

class DBNotFound( DBError ):
	'No valid database was found'

class _Record:
	'A record within a towel-db database'
	path = ""
	exists = False
	mode = ""
	key = ""
	changed = False
	data = {}

	def __init__( self, path, key, mode ):
		"""Create a record object
		Takes a path string and a key string as an argument"""
		self.path = path
		self.key = key
		self.mode = mode
		self.data = {}
		changed = False
		
		record_path = os.path.join( path, key )

		# Figure out if the record exists
		if os.path.exists( record_path ):
			self.exists = True

			# Open the record
			recfile = _open( record_path, "r" )
			self._read( recfile.read( ))
			recfile.close()
	
	def __len__( self ):
		return len( self.data )

	def __getitem__( self, key ):
		try:
			return self.data[key]
		except( KeyError ):
			raise KeyError, 'Field not found in record'

	def __iter__( self ):
		return iter( self.data )

	def __setitem__( self, key, value ):
		# Check mode
		if self.mode is DB_READ:
			raise DBWrongMode, 'Database opened read-only'
		
		# Make sure the type is right
		if not type( key ) == type( value ) == str:
			raise TypeError, 'Key and value must both be strings (for now)'
			
		if not self.changed:
			# Record has chanegd; OK to sync
			self.changed = True
		
		# Store the data
		self.data[key] = value
		
	def __delitem__( self, key ):
		if self.mode is DB_READ:
			raise DBWrongMode, 'Database opened read-only'
		
		if key in self.data:
			del self.data[key]
			self.changed = True
		else:
			raise KeyError, 'Given key is not in record'

	def __del__( self ):
		if self.changed:
			self.sync()

	def _read( self, data ):
		"""Read the database record.
		Takes a data string as an argument"""

		current_column = ""

		# Parse the data
		for line in data.splitlines( ):
			if line[0:2] == '%%':
				# Header line
				current_column = line[2:]
				self.data[current_column] = []
			else:
				# Data line
				self.data[current_column].append( line )

		# Concatenate data lines
		for field in self.data:
			# Iterate through all but the last line
			for line_index in xrange( len( self.data[field] ) - 1 ):
				self.data[field].insert( line_index+1, '\n' )
			# Clean out escaping
			if self.data[field][0][0] == '\\':
				self.data[field][0] = self.data[field][0][1:]
			# Finish
			self.data[field] = string.join( self.data[field], '' )

	def change_key( self, key, clear=True ):
		"""Mutator function to change the record key.
		change_key( key, clear=True )"""
		if not type( key ) == str:
			raise TypeError, 'Key must be a string'
		
		self.key = key
		if clear:
			self.data = {}
		
		if not self.changed:
			self.changed = True
		
	def get_key( self ):
		'Accessor function to get the key of the record'
		return self.key

	def clear( self ):
		'Clear the record of data'
		self.data = {}
		changed = True

	def sync( self ):
		data_t = []
		final_data = ""
		
		record_path = os.path.join( self.path, self.key )

		# Open the file
		datafile = _open( record_path, DB_WRITE )

		# Escape dangerous stuff
		for key in self.data:
			if self.data[key][0:2] == '%%':
				self.data[key] = string.join( ['\\', self.data[key]], '' )
			
			# Escape the escape char
			elif self.data[key][0] == '\\':
				self.data[key] = string.join( ['\\', self.data[key]], '' )
		
		if len( self.data ) > 0:
			# Construct the data
			for field in self.data.keys():
				data_t.append( '%%' )
				data_t.append( field )
				data_t.append( '\n' )
				data_t.append( self.data[field] )
				data_t.append( '\n' )

			# Join and clean the data
			data_t.pop()
			final_data = string.join( data_t, "" )
			
		# Write the data
		datafile.write( final_data )

		# Close the file
		datafile.close( )
		
		# No changes to be flushed now
		self.changed = False

class Db:
	'A human-readable and fairly simple database system'
	path = ''
	mode = ''
	
	def __init__( self, path, mode=DB_READ ):
		""" Create an towel-db database
		Takes a path to a database directory tree as an argument"""
		self.path = path
		self.mode = mode
		
		# Make sure that the path exists
		if not os.path.isdir( self.path ):
			if self.mode != DB_CREATE:
				raise DBNotFound, 'No database exists'
			else:
				self._create_db()
		
		# Make sure that this is a database, and check the version
		try:
			meta = self[_DB_META]
		except( KeyError ):
			if self.mode != DB_CREATE:
				raise DBNotFound, 'Not a valid database'
			else:
				self._create_db()
		
		if int( self[_DB_META]['version'] ) != DB_VERSION:
			raise DBVersionMismatch, 'Database version mismatch'

	def _create_db( self ):
		"""Create a database"""

		# Check the mode
		if self.mode != DB_CREATE:
			raise DBWrongMode, '"Create" mode not set'
		
		# Check if we need to create the directory
		if not os.path.isdir( self.path ):
			os.mkdir( self.path )
		
		# Create the database meta file
		meta = _Record( self.path, _DB_META, self.mode )
		meta['version'] = str( DB_VERSION )
		meta.sync()
		
	def __str__( self ):
		return string.join( ['towel-db database at', self.path] )

	def __len__( self ):
		return ( len( os.listdir( self.path )) - len( DB_RESERVED ))

	def __getitem__( self, key ):
		"""Get a record from the key given"""

		# Make sure the key exists
		if not( os.path.exists( os.path.join( self.path, key ))):
			raise KeyError, key

		# Get the data file
		record = _Record( self.path, key, self.mode )

		return record
		
	def __iter__( self ):
		contents = os.listdir( self.path )
		
		# Remove private records
		for rec_i in xrange( len( contents ) - 1 ):
			if contents[rec_i] in DB_RESERVED:
				del contents[rec_i]
		
		# Return an iterator through the list
		return iter( contents )
			
	
	def __setitem__( self, key, value ):
			if not type( key ) == str:
				raise TypeError, 'Key must be a string'
			
			if key in DB_RESERVED:
				raise DBError, 'Requested key is reserved'
			
			if self.mode == DB_READ:
				raise DBWrongMode, 'Database opened read-only'
			
			# Assign
			if isinstance( value, _Record ):
				record = value
				record.change_key( key, False )
				record.sync()
			elif value is None:
				record = _Record( self.path, key, self.mode )
				record.sync()
			else:
				raise TypeError, 'Value must be a record (for now)'

	def __delitem__( self, key ):
		if self.mode == DB_READ:
			raise DBWrongMode, 'Database opened read-only'
			
		os.remove( os.path.join( self.path, key ))
		
	def create_key( self, key ):
		'Create an empty new database record.  Takes a key string'
		if not type( key ) == str:
			raise TypeError, 'Key must be a string'
			
		record = _Record( self.path, key, self.mode )
		record.sync()
	
	def drop( self ):
		if self.mode != DB_CREATE:
			raise DBWrongMode, 'Database deletion requires "Create" open mode'
			
		# We need all the records, including the private ones
		records = os.listdir( self.path )
		
		# Delete the records
		for rec in records:
			os.remove( os.path.join( self.path, rec ))
		
		# Wrap everything up
		os.rmdir( self.path )
		del self

def open( path, mode=DB_READ ):
	return Db( path, mode )

def db_exists( path ):
	exists = False
	
	if os.path.isdir( path ):
		try:
			temp_db = open( path, mode=DB_READ )
			temp_db[_DB_META]
			exists = True
		except( KeyError, DBNotFound ):
			pass
	
	return exists
