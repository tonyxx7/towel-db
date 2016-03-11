# Introduction #
The API is extremely simple, as expected from such a simple database library.  Describes version 0.8.1.

# Objects #
## Global ##
### Functions ###
```
def open( path, mode=DB_READ )
```
Open a database at _path_ with _mode_ permissions.

### Constants ###
  * DB\_VERSION - Version of the database implemented.
  * DB\_RESERVED - Reserved record names

The mode given to open() can be:
  * DB\_READ - Read-only
  * DB\_WRITE - Read/Write
  * DB\_CREATE - Read/Write, create if needed

### Errors ###
  * DBError - Base class for towel-db errors
    * DBVersionMismatch - Version of the database being opened is different from that of the library.
    * DBWrongMode - A requested operation isn't compatible with the requested database open mode.
    * DBNotFound - The requested database couldn't be found.

## Database ##
### Functions ###
```
create( key )
```
Create a new record with a key of _key_.

### Usage ###
```
# Record creation
db.create( 'foo' )
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

### Usage ###
```
# Assignment
record['name'] = 'foo'
record['summary'] = 'bar'

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
```