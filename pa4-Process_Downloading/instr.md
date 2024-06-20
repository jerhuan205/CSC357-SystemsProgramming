# Overview

The ability for one process to spawn additional processes is a powerful mechanism that can be used to accomplish a variety of tasks. This includes processes that work together and communicate to accomplish one larger task. It also includes using multiple processes to accomplish many smaller tasks in parallel.

For this assignment you will write a program that will use multiple processes to download a collection of files. These processes do not need to communicate to accomplish this task; the goal is to increase efficiency. In particular, if a single process attempts to download a collection of files in serial, then downloading from a slow server will delay all remaining downloads. Instead, when done in parallel, a process connected to a slow server can wait without affecting the other processes.


### Setup

Clone the repository.

- Accept the [GitHub Classroom Assignment 4](https://github.com/amigler-cp/assignment-4-jerhuan205) invitation.



# Functional Overview

Your program will process a file containing the [URLs](https://en.wikipedia.org/wiki/URL) for the files to be downloaded (an example file is provided in the assignment repository). The actual downloading will be handled by the _curl_ program which be installed in your development environment with the command: `sudo apt-get install curl`


### curl

The _curl_ program is a command-line tool that can be used to download files specified by URLs. This program allows for configuration using a truly incredible number of command-line options, but we will not consider the vast majority of these options. Instead, your program will use _curl_ in the following manner (with the options described below).

`curl -m <seconds> -o <filename> -s <url>`

- **`-m <seconds>`** — This option limits the download time to the specified number of seconds.
- **`-o <filename>`** — This option specifies the name of the output file to which the contents of the download file will be written.
- **`-s`** — This option silences the progress meter typically printed by _curl_.
- **`<url>`** — This is the URL of the file to download.



# Requirement 1

Your program must be named `a4download` take two command-line arguments, in this order:

1. The name of the file (discussed below) containing the downloads to be processed.
2. An integer indicating the _maximum_ number of downloading processes that may be executing at one time (this is so that the user can control the load from this program). Your program must verify that the arguments represent a valid file and a valid positive integer.

For example, your program should run as follows:

`./a4download test_urls.txt 3`



# Requirement 2

Each line of the provided file will contain the information relevant to a file to download, in this order:

1. Output file name,
2. URL
3. An optional maximum number of seconds for the download.

These values will be separated by a space character. You may assume that the output file name is valid and does not include any directory separators. You may also assume that the URL is a valid HTTP(s) URL. The maximum number of seconds, if provided, will be an integer value > 0.

For example, the line:

`file1 https://csc.calpoly.edu/wp-content/uploads/2020/11/cal-poly-logo-reverse.svg 2`

Should cause your program to download the content at URL https://csc.calpoly.edu/wp-content/uploads/2020/11/cal-poly-logo-reverse.svg, into a local file named `file1`, waiting a maximum of 2 seconds



# Requirement 3

Your program must spawn a single process per download request, but only up to the (command-line argument) specified maximum number of (child) processes that may exist at one time. Each of these processes will execute the appropriate _curl_ command with the specified output file, url, and, if appropriate, maximum time.

If there are more download requests than the maximum number of allowed processes, then additional requests must be satisfied as each process completes. It is not sufficient to spawn only a single process at a time; multiple processes (up to the specified limit) must be used.



# Requirement 4

The primary (parent) process should print when a download request is starting and when it ends. The output (when starting and when ending) must refer to each download request by the line number in the provided file and the child process id (e.g., `process 3287 processing line #1`). If one of the _curl_ processes terminates abnormally, then the primary process must print an error message indicating such.



# Requirement 5

Your program must properly manage resources to avoid running out of processes.



# Deliverables

- Source Code (and `Makefile`) - Push all relevant source code and an appropriate `Makefile` to your repository.
- During testing, the autograder creates files/directories with the prefix: `ag_` Please make sure your repository does not include files or directories named with this prefix with these names.
- Please do not commit compiled/executable files to git (ie. do not add: `a.out`, `a4download` (the executable file, without an extension), `main.o`, etc.)
- For full credit, your git commit history must be sufficiently detailed to illustrate how you approached the assignment, addressed bugs, etc. Commit often, with descriptive commit labels. For example: "initial data structures and function prototypes", "implemented the -a switch to display hidden files", "solved resource leak in function xyz()"  There is no fixed minimum number of commits. As one example guideline, you will not be eligible for full credit if your submitted git repository includes just 2-3 commits where large blocks of code are added in each commit.



# Testing / Expected Output Notes

The GitHub autograder performs an acceptance test confirming that your program successfully downloads a sequence of files; as a final step, the autograder performs a diff check to confirm output file names / sizes. To further verify functionality of your program, the following steps may be used.


### Sample Output with Provided test_urls.txt File

Exact program output may vary (no diff checking is performed). Important aspects in this example output include: indication that a new download process starts immediately (subject to specified maximum -- 3 in this sample run), and possible out-of-order completion: for example, the long-running download (line #7) completes last. At the start and end of each download process, you should print the (child) process ID and line number processed. Any abnormal termination conditions or return codes from the `curl` process other than 0 should also be reported (see below for examples.)

```
$ ./a4download test_urls.txt 3
process 29386 processing line 1
process 29387 processing line 2
process 29388 processing line 3
process 29388 processing line 3 exited normally
process 29392 processing line 4
process 29387 processing line 2 exited normally
process 29394 processing line 5
process 29386 processing line 1 exited normally
process 29396 processing line 6
process 29394 processing line 5 exited normally
process 29398 processing line 7
process 29392 processing line 4 exited normally
process 29400 processing line 8
process 29400 processing line 8 exited normally
process 29396 processing line 6 exited normally
process 29398 processing line 7 exited normally
```

### Test Concurrent Downloads

Create an input file with several time-consuming downloads, for example (`delays.txt`):

```
file1 https://httpbin.org/delay/10
file2 https://httpbin.org/delay/10
file3 https://httpbin.org/delay/10
file4 https://httpbin.org/delay/10
```

Run your download process using the `time` command to confirm that your program downloads files in parallel. The reported "real" time should, in this case, be under 11 seconds.

```
$ time ./a4download delays.txt 4
process 28885 processing line 1
process 28886 processing line 2
process 28887 processing line 3
process 28888 processing line 4
process 28885 processing line 1 exited normally
process 28888 processing line 4 exited normally
process 28886 processing line 2 exited normally
process 28887 processing line 3 exited normally

real	0m10.597s
user	0m0.196s
sys	0m0.048s

```

### Test Failed Downloads

Create an input file with an invalid URL and/or an expected timeout (delay longer than max allowed time), for example (`invalid_urls.txt`):

```
file1 https://not-a-real-domain.not-a-tld/not_valid
file2 https://httpbin.org/delay/3 1
```

```
./a4download invalid_urls.txt 1
process 28944 processing line 1
process 28944 processing line 1 terminated with exit status: 6
process 28946 processing line 2
process 28946 processing line 2 terminated with exit status: 28
```




