# toweldb #
## About ##
toweldb is an extremely simple two-dimensional database library currently only available for Python.  It has been designed for data integrity over performance by using a simple textual data storage format so that it can be easily repaired by hand if needed.  It is primarily aimed at usages where a search delay is preferable to a broken database (i.e. package managers).

## Status ##
Currently, toweldb 0.8.1 is available.  It's a proof-of-concept written in Python, and has more than a couple bugs.  The pure-python approach has since been discontinued in favor of a library written in C, which is under heavy development and will be released as 0.9.

## Rationale ##
Raw speed is, in most cases, not a concern for "real world" programs.  As processor and disk performance rises, the usage of a binary database format begins to be of questionable value for most use cases.  A binary format is also harder to debug, and cannot be easily repaired in the case of a system crash or just a bug in the database library itself.  A textual format, however, sacrifices performance for data stability.  Thus, the goal of toweldb is to provide a database library with a data storage engine more powerful than standard configuration file formats, while still offering the integrity possible in such formats.