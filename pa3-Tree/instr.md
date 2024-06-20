# Overview

In the previous assignment you were asked to write a program that interacted with an simulated filesystem. One of the goals was to emphasize that directories provide structure, but are otherwise just files with a particular format (and role).

This assignment requires that you write a program that interacts with the system's actual filesystem (via system calls), that traverses a portion of the directory hierarchy, and that manages scarce resources. For this assignment you will write a program that recursively prints directory contents in a tree-like format.



### Setup

Clone the repository.



## Functional Overview

Your program, named `tree`, must take as command-line arguments optional switches and a single directory name. For each directory name, your program must print the contents of that directory (and, recursively, each subdirectory) in a tree-like format. If no directory names are given, list the contents of the current directory. Your output should match the Linux command: `tree -n --charset=ascii`  (assuming the use of the standard `tree` utility available within an Ubuntu environment)  Note that you do not need to implement text color.

As an example, your program should produce output as follows:

```
% ./tree tmp
tmp
|-- alpha
|-- bob
|-- tmp
|   |-- alpha
|   |-- bob
|   |-- burrito
|   |-- tmp
|   |   `-- bob
|   `-- zebra
`-- zebra

4 directories, 6 files
    
```

This tree shows the contents of the `tmp` directory and each subdirectory below it. The contents of each directory must be sorted alphabetically.

**Note**: When processing a directory's contents, do not recursively follow the two special files `.` and `..` (consider why not). Also, do not follow symbolic links (but do print the link name itself).



## Command-line Arguments

Your program must support the following two command-line switches, either individually or in combination. If given, these switches will immediately follow the program name (and precede the specified directory). Your program must allow the switches to be provided in any order.

- Hidden Files &#91;`-a`&#93;: By default, your program should exclude "hidden" files (those beginning with a `.`) in its listing. The `-a` switch causes "hidden" files to be displayed (except for the two special files `.` and `..`).
- Size &#91;`-s`&#93;: The `-s` switch causes the size (in bytes) of each file and directory. Your output should match the Linux command: `tree -n -s --charset=ascii tmp` (assuming the use of the standard tree utility available within an Ubuntu environment; note that you **do not** need to manually sum sizes within subtrees) For example, running your program with `-s`:

```
./tree -s tmp
tmp
|-- [         40]  alpha
|-- [         21]  bob
|-- [       4096]  tmp
|   |-- [          0]  alpha
|   |-- [          0]  bob
|   |-- [       4096]  burrito
|   |-- [       4096]  tmp
|   |   `-- [         25]  bob
|   `-- [         10]  zebra
`-- [       4096]  zebra

4 directories, 6 files
```



## Resource Management

All programs must properly manage the resources they use. In this respect, systems programs differ only in that the resources they must manage are often more scarce than those used by general applications. To address this issue, one will often use more readily available resources (e.g., memory) to avoid exhausting other resources.

If you are not careful in your implementation, then your `tree` program will likely run out of file descriptors and be unable to continue. Do not allow this to happen. Your program will be tested with a very deep, very contrived directory structure (i.e., it will be much deeper than the number of files that a process may open) or with the limit for the number of open files set artificially low (you should do this for your testing, see this [link](https://www.linuxtechi.com/set-ulimit-file-descriptors-limit-linux-servers/)).



## Useful Functions

You might find the following functions to be of use (this is not to say that you will need them all or that you will not use others)

```chdir, fchdir, opendir, qsort, readdir, stat, lstat```



# Deliverables

- Source Code - Push all relevant source code and an appropriate Makefile to your repository.
- During testing, the autograder creates several files & directories named with the prefix `ag_`. Please make sure your repository does not include directories with this prefix.
- Please do not commit compiled/executable files to git (ie. do not add `a.out`, `fs_simulator` (the executable file, without an extension), `main.o`, etc.)
- For full credit, your git commit history must be sufficiently detailed to illustrate how you approached the assignment, addressed bugs, etc. Commit often, with descriptive commit labels. For example: "initial data structures and function prototypes", "implemented the -a switch to display hidden files", "solved resource leak in function xyz()"  There is no fixed minimum number of commits. As one example guideline, you will not be eligible for full credit if your submitted git repository includes just 2-3 commits where large blocks of code are added in each commit.
