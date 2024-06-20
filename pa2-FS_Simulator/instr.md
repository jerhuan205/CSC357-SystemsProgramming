# Overview

Our regular use of computers (in various forms) includes interactions (often indirectly) with a file system or a file system-like structure. This course includes exploration of the systems programmer's interactions with the file system through a variety of system calls. For this assignment, you are asked to simulate a simplified file system using regular files to store filesystem details (to reduce complexity). Your program will support directories and files, navigation through the simulated file system (eg. change between directories), and modifications to the simulated file system (e.g., add directories and files).

This assignment should serve to further hone your C programming skills and to provide a new perspective on files and directories.



# Setup

- Accept the [GitHub Classroom Assignment 2 invitation](https://github.com/amigler-cp/357-assignment-2-jerhuan205).
- Clone the repository.

There are two provided example file systems found in sub-directories of the initial project repository. One (`empty/`) contains only a root directory. The other (`fs`) contains a few files and directories as demonstrated by the commands below. I recommend that you copy one of these and test with the copy while developing to avoid corrupting the provided files. You may also restore these directories from the git repository. Or, the sample file system directories may be found in this repository: https://github.com/amigler/357-a2-starter

```
> ls
0 .
0 ..
1 357
> cd 357
> ls
1 .
0 ..
2 assignment2
3 final
6 assignment3.tex
> cd final
> ls
3 .
1 ..
4 final.tex
5 code.c
> 
```



# Requirements

The following outlines the requirements for your program.


## Functional Overview

One approach to a file system is to store information (metadata) that pertains to a "file" (e.g., a regular file or a directory) in an [inode](https://en.wikipedia.org/wiki/Inode). This metadata includes information such as:
- the physical location of the file's contents on the storage medium (e.g., hard disk)
- the user that owns the file
- the access rights to the file
- the file's timestamps, etc.

We will **_not_** simulate all of this.

Of note, the metadata for an inode does not contain the name of the file. Instead, a directory maps names to inodes. But a directory is also a "file". This is what we will simulate.


### **Some Specifics**

Your program will store a fixed number of "inodes" (which is not unreasonable when simulating a physical device). The metadata for these "inodes", stored in a file named inodes_list will contain only a number (an index) and a "type" to indicate a directory ('d') or a file ('f'). The contents of the "file" associated with each "inode" are to be stored in a regular file with name matching the index number (i.e., if your program is meant to access the contents of the "file" associated with "inode" 3721, then it will open the file named "3721"; this is a simplification of tracking blocks on a physical medium).

**Limit**: Your program must support up to 1024 inodes (ranging from 0 to 1023). This is an entirely artificial limit and we will work to remove such limits as the quarter progresses.

The content of an example `inodes_list` file is listed below. Note that this is a binary file, not ASCII-text as we have been working with thus far, and therefore cannot be viewed or edited with a text editor such as `vim`. The example below uses the [`xxd`](https://www.man7.org/linux/man-pages/man1/hexdump.1.html) utility to display binary data, using `-c 5` to show each record in the file on its own line (each inode record is 5 bytes: a 4-byte integer followed by a single ASCII character: d or f) In the `xxd` output, the first column (before the `:` character) represents the hexadecimal byte offset within the file. The middle columns show the binary file data in hexadecimal. The final output column displays ASCII characters where possible, or the period character for non-ASCII bytes.

![Screen Shot 2023-04-14 at 5 06 14 PM](https://github.com/jerhuan205/CSC357-SystemsProgramming/assets/122332863/f640cf1d-3950-4df5-9dbb-bd56b136c465)


### **Directory Representation**

The contents of a directory will be a sequence of file entries. Each file entry will specify, in this order, the inode number for the entry (use the `uint32_t` type defined in `stdint.h` to represent an unsigned 32-bit integer) and the name of the entry, up to 32 characters. File names less than 32 characters will include a null character within the first 32 characters (but a full 32 charactes must be stored in the file to keep each entry the same size); it is valid to use the full 32 characters for the file name, so your program must account for the lack of a null character in that case. The entries within a directory are stored as binary representations of each piece of data.

The following is an example of a directory corresponding to inode number 3, containing the files: `.`, `..`, `final.tex` and `code.c`

![Screen Shot 2023-04-14 at 5 07 39 PM](https://github.com/jerhuan205/CSC357-SystemsProgramming/assets/122332863/e7831222-0675-4976-9277-33a511bd8252)


#### **Initial Directory**

Your program will begin in the (assumed) directory at inode 0 (this is meant to be the root of the file system, typically referenced as /). Your program will then take commands from the user to traverse and manipulate the file system (see below).


### **File Representation**

The inode file corresponding to a plain file (type f) in your simulated filesystem should contain the full file name stored as ASCII text, followed by a single newline character. The following is an example of the content of inode file `4`, representing a file with name `file.tex`:

![Screen Shot 2023-04-20 at 12 18 23 PM](https://github.com/jerhuan205/CSC357-SystemsProgramming/assets/122332863/25c47515-1def-41e4-ad09-296b7e32a950)



## Requirement 1

Your program must take one command-line argument (a string) specifying the name of a directory that holds the simulated file system (referenced as the file system directory below). Assuming that `fs` is such a directory, your program may be run as follows:

```
./fs_simulator fs
```

Your program must verify that the specified directory actually exists.



## Requirement 2

Your program must load, from the file system directory (consider using `chdir` once for this), the `inodes_list` file. This file includes an entry for each inode currently in use. Each such entry will include the inode number (of type `uint32_t`, as within a directory) and an indicator (a single character, d or f) of the file type. These entries are stored as the binary representations of each piece of data, so you will want to use `read`/`fread` to extract the data from the file. Verify that these indicators are valid. Any inode numbers outside of the range from 0 to the maximum (exclusive) or with invalid indicators should be reported as invalid and ignored.



## Requirement 3

With the simulation initialized, the user will start in the directory at inode 0. If this inode is not a directory (or is not in use), then the program should report an error and terminate. The program will need to track the current working directory (initially 0).



## Requirement 4

Your program must prompt the user (using the character `>` as command prompt) for commands, reading commands as text from stdin. The required commands and their behavior are listed below. If the user attempts to run any other command, or if a command is provided with the wrong number of arguments, then report an appropriate error and wait for the next command.

**Limit**: The use of file names in the commands discussed below must be restricted in length to properly interact with the represented directories. In particular, if your program allows a user to enter an arbitrarily long name (e.g., when executing `mkdir`), then it will need to truncate the name to the maximum length of 32 (specified earlier).

- **exit** - (or EOF; ^D in Unix). Save the state of the inode list (see the Requirement 5) and exit the program. 
- **cd &#60;name&#62;** - Change to the specified directory (by name) from the current directory. The name must be given. Your program does not need to support paths (i.e., `cd lab2` is valid, but `cd lab2/task1` is not). If the specified name is not a directory (does not exist or is a regular file), then report the error and do not change the current working directory.
- **ls** - List the contents (inode number and name) of the current working directory.  You may list contents in "natural" order (ie. the same order in which the entries appear in the directory inode file, beginning with `.` and `..`)
- **mkdir &#60;name&#62;`` - Create a new directory (in the current directory) with the given name. Doing so will require that there is an available inode and that the contents of this directory are updated. Moreover, the file corresponding to this new inode will need to be created with the default entries of `.` and `..`. Report an error and continue if there is already an entry in the current directory with this name.
- **touch &#60;name&#62;** - Create a regular file (in the current directory) with the given name. Doing so will require that there is an available inode and that the contents of this directory are updated. The real file corresponding to this new inode will need to be created; for simulation/debugging purposes the _name_ of the file should be written to the real file (corresponding to the inode number). If there is already an entry with this name, then this command will do nothing.****



## Requirement 5

On exit, your program must update the `inodes_list` file in the file system directory, as well as all numbered inode files, to reflect any modifications to the inodes.



# Utility Function

It is likely that your program will need to convert an integer into a string. There are a few ways to do this (especially given that the maximum number of digits that will be printed for this program is known to be small), but the following is a common approach.

```
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

char *uint32_to_str(uint32_t i)
{
   int length = snprintf(NULL, 0, "%lu", (unsigned long)i);   // pretend to print to a string to determine length
   char* str = malloc(length + 1);                            // allocate space for the actual string
   snprintf(str, length + 1, "%lu", (unsigned long)i);        // print to string

   return str;
}
```



# Deliverables

- Source Code - Push all relevant source code and an appropriate `Makefile` to your repository.
- During testing, the autograder creates several files & directories named with the prefix `ag_`. Please make sure your repository does not include directories with this prefix.
- Please do not commit compiled/executable files to git (ie. do not add `a.out`, `fs_simulator` (the executable file, without an extension), `main.o`, etc.)
- For full credit, your git commit history must be sufficiently detailed to illustrate how you approached the assignment, addressed bugs, etc. Commit often, with descriptive commit labels. For example: "initial data structures and function prototypes", "implemented the -a switch to display hidden files", "solved resource leak in function xyz()"  There is no fixed minimum number of commits. As one example guideline, you will not be eligible for full credit if your submitted git repository includes just 2-3 commits where large blocks of code are added in each commit.
