# Introduction #
The API is extremely simple, as expected from such a simple database library.  Describes the latest revision of the pure-Python prototype of toweldb.
This document shouldn't change for a long time, as the pure-python version of toweldb is deprecated and will never be worked on ever again.

# Objects #
## Global ##
### Functions ###
```
is_key_valid( key )
```
Check if a record key _key_ is valid.

```
db_exists( path )
```
Check if the database at _path_ exists and is a valid database.  Returns a boolean.

```
open( path, mode=DB_READ )
```
Open a database at _path_ with _mode_ permissions.

### Constants ###
  * DB\_VERSION - Version of the database implemented.

The mode given to open() can be:
  * DB\_READ - Read-only
  * DB\_WRITE - Read/Write
  * DB\_CREATE - Read/Write, create if needed

Record keys can be checked for validity with these constants:
  * KEY\_MAX\_LEN - The maximum length of a record key
  * KEY\_DANGEROUS - A list of prohibited characters
  * DB\_RESERVED - Reserved record names

### Exceptions ###
  * DBError - Base class for towel-db errors
    * DBVersionMismatch - Version of the database being opened is different from that of the library.
    * DBWrongMode - A requested operation isn't compatible with the requested database open mode.
    * DBNotFound - The requested database couldn't be found.
    * DBInvalidKey - A key that was too long, reserved, or contained illegal characters was used.
    * DVNoIntegerRecords - The database has no records with an integer key.

## Database ##
```
class Db()
```
### Functions ###
```
Db.append()
```
Create and return a new integer record with an index that is one larger than the current largest integer record.

```
Db.create_key( key )
```
Create and return a new record with a key of _key_.  _key_ may be a string or an integer.

```
Db.drop()
```
Delete the database.

```
Db.keys( force_sort=False )
```
Return the list of record keys in the database.  By default, it will only sort the list if there is at least one integer key in it, but you can force
sorting by setting _force\_sort_ to True.

```
Db.last()
```
Return the key of the last integer record in the database.  Return _-1_ if there are no integer records.

```
Db.values()
```
Return a list of the records in the database.

### Usage ###
```
# Record creation.  db.create_key() is preferred over assignment of None
db.create_key( 'foo' )
db.create_key( 42 )
db['bar'] = None

# Record copying
db['bah'] = db['foo']

# Record deletion
del db['foo']

# Database iteration
for key in db:
    print key
    print db[key]['test']

# Number of records
print len( db )
```

## Record ##
```
class _Record()
```
Please do not create record objects manually.  They are returned as needed by database functions.
### Functions ###
```
change_key( key, clear=True )
```
Mutator function to change the key of a record object.  May be made private before 1.0.

```
get_key()
```
Accessor function to get the key of a record.

```
clear()
```
Clear all the data in the record.

```
sync()
```
Flush all changes to disk.

```
keys()
```
Return a list of field keys within the record.

### Usage ###
```
# Assignment
record['name'] = 'foo'
record['summary'] = 'bar'
record[3] = 'foobar'

# Field copying
record['bah'] = record['name']

# Field deletion
del record['name']

# Writing
record.sync()

# Iteration
for key in record:
    print key
    print record[key]

# Number of fields
print len( record )

# List of keys
print db.keys
```