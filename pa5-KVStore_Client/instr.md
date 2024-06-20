# Overview

For this assignment you will write a server program that allows other programs to store and retrieve data. Your server program will act as a [key-value store](https://en.wikipedia.org/wiki/Key%E2%80%93value_database) and will implement two types of requests: set and get. In addition, your program must maintain an [index](https://en.wikipedia.org/wiki/Database_index) to allow efficient lookups by key for "get" requests.


### Setup

- Accept the [Github Classroom Assignment 5 invitation](https://github.com/amigler-cp/357-assignment-5-jerhuan205).
- Clone the repository.



# Functional Overview

Your program will maintain a database of key/value pairs. Keys will be strings up to 32 characters in length (following the rules of C variable names, ie: no spaces or special characters), values will be strings up to 1024 characters in length.



# Requirement 1

Your primary server program should be named `kvstore` and must accept two command-line arguments, in this order:

1. The name of the file (discussed below, in R4) containing the database records in plain text format.
2. The file name of the UNIX domain socket that will accept requests from other programs. Your program should create this socket file if it does not already exist.

For example, your program should run as follows:

`./kvstore test.db db_socket`



# Requirement 2

Implement a separate client program to access your database. Your client should be minimal; it should be constructed to pass key/value strings to a running `kvstore` process via the UNIX domain socket provided as its first command line argument. This client program should be named `kvclient`, and should accept commands as follows:

`./kvclient <socket file> <get or set> <key> [<value>]`

For example, to store the string `Systems Programming` under key `cpe357` (note the double quotes around the value to treat this as a single command line argument; the quotes _should not_ be stored as part of the value):

`$ ./kvclient db_socket set cpe357 "Systems Programming"`

Example retrieval of the value for key `cpe357`:

```
$ ./kvclient db_socket get cpe357
Systems Programming
```

After issuing each get request, your client should read and print the value. If the requested key is not found in the database, your client program should print a message that reads: `Key <name of key> does not exist.`

Your `kvclient` program should terminate after handling a single set/get request, and it should print a notification message if an error occurs.



# Requirement 3

Your `kvstore` program should be constructed to run continuously, accepting requests until it receives a `SIGQUIT` signal. While running, your program should accept request strings sent over the server's UNIX domain socket. Commands will be structured as follows:

`get <key>`

or

`set <key> <value>`

Each request will be terminated with a newline character. Values will never contain commas.

A `set` request for an existing key should cause that new value to be returned for all future get requests for that key.



# Requirement 4

Your database file should be maintained in plan text form, with a comma-separated key/value pair stored on each line. For example, this sequence of commands:

```
set a value for a
set b b's value
set a new a value
set my_c another value
```

Should yield a database file with this content:

```
b,b's value
a,new a value
my_c,another value
```

Note that lines may be stored in any order, they do not need to be sorted by key. You are also permitted to retain "stale" values in your database file, as long as your get requests return the most up-to-date value. This might be the case if you treat your database file as an append-only file.  In the above example, the following database file would also be valid, on the assumption that the last entry for a given key holds the current value:

```
a,value for a
b,b's value
a,new a value
my_c,another value
```



# Requirement 5

To support efficient lookups by key, your `kvstore` program must maintain an in-memory [index](https://en.wikipedia.org/wiki/Database_index). It is up to you to define an indexing strategy; the fundamental requirement is that the indexing strategy you implement must support lookups that are more efficient than an O(n) "full scan." Possible approaches include the following data structures:

- [Hashtable](https://en.wikipedia.org/wiki/Hash_table)
- [B-Tree](https://en.wikipedia.org/wiki/B-tree)
- [Skip list](https://en.wikipedia.org/wiki/Skip_list)

Your structure must be designed to support lookups by key that do not involve (in the worst case) comparing the requested key against all keys in your data store, or reading through all lines in your database file. A lookup by key should be performed in less than O(n) time.



# Requirement 6

The data within your database must be stored on disk and must be available for use by subsequent executions of your `kvstore` program. For example, if a user stores values using keys `a`, `b`, and `c` and your `kvstore` program is then terminated and re-run at a later time, the user should be able to retrieve the values most recently stored for keys `a`, `b`, and `c`. Your index should be maintained entirely in-memory by the `kvstore` program, and should be rebuilt (based on any existing entries) every time your `kvstore` program starts up.

Your server program may assume that all keys can be retained in memory. However, the full set of values **should not** be retained in main memory, on the assumption that this would exceed the amount of main memory available to your program. Aside from temporary handling during a set/get request, values should be stored only in the on-disk database file; any get request will require the server to read from the database file. Your client program should not permanently store keys or values (either in memory or on disk.)



# Requirement 7

Your program must properly manage resources to avoid running out of processes/memory. Your program must pass a `valgrind` check with zero errors.



# Forward-looking Note

The next (and last) programming assignment will re-use this implementation as part of its functionality. Assignment 6 will involve the construction of a network server that handles HTTP requests, using your `kvstore` program for data storage.



# Deliverables

- Source Code (and `Makefile`) - Push all relevant source code and an appropriate `Makefile` to your repository.
- During testing, the autograder creates files/directories with the prefix: `ag_`. Please make sure your repository does not include files or directories named with this prefix with these names.
- Please do not commit compiled/executable files to git (ie. do not add: `a.out`, `kvstore` (the executable file, without an extension), `main.o`, etc.)
- For full credit, your git commit history must be sufficiently detailed to illustrate how you approached the assignment, addressed bugs, etc. Commit often, with descriptive commit labels. For example: "initial data structures and function prototypes", "implemented the -a switch to display hidden files", "solved resource leak in function xyz()"  There is no fixed minimum number of commits. As one example guideline, you will not be eligible for full credit if your submitted git repository includes just 2-3 commits where large blocks of code are added in each commit.



# Rubric
![kvstore_rubric](https://github.com/jerhuan205/CSC357-SystemsProgramming/assets/122332863/e7bdfc43-1874-4737-bb05-626114902a86)



