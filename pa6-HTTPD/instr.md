# Overview

For this assignment you will implement a minimal web server that will support a subset of the Hypertext Transfer Protocol (HTTP). This webserver will service multiple client requests by creating threads to handle the actual request logic. In addition, your server will support data storage and retrieval, relying on your kvstore implementation from the previous assignment.


### Setup

- Accept the [GitHub Classroom Assignment 6 invitation](https://github.com/amigler-cp/357-assignment-6-jerhuan205).
- Clone the repository.



# Requirement 1

Your program, named `httpd`, will take two command-line arguments specifying (a) the name of a kvstore socket file file, and (b) the TCP port on which to listen for connections. The port is an integer value given to the operating system when setting up the network communications. A web server usually listens for connections on port 80. Since this is a low-numbered port, elevated access is required. Since this program will be run without superuser access, you will need to choose a different port. When testing, you may use any number between 1024 and 65535. Example command line:

`./httpd ../project5/server_socket 8080`

In this example, the `server_socket` file is located in a different directory (recall that `..` represents the parent directory)



# Requirement 2

To handle multiple requests your server will create threads. Each request is to be handled by a single thread. Your httpd server should ensure that threads are gracefully terminated and should report an error if a thread terminates abnormally. If your threads share any data, access to this data should be synchrobnized appropriately using mutexes.



# Requirement 3

HTTP is a plaintext line-based protocol. This assignment requires that you support the HTTP request types:

[`HEAD`](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/HEAD), [`GET`](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/GET), and [`PUT`](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/PUT)


### Request

Each request will begin with a text line of the form: `TYPE endpoint HTTP/version`.

For example,

```
GET /index.html HTTP/1.1
```

The server will attempt to locate the requested file and, if found, will send a reply with information pertaining to the file and, if a `GET` request, the contents of the file. A `HEAD` request will send information about the file; the contents are not sent. For this assignment, you can ignore the `HTTP` version information on a request.

Your server should handle only a single request/response per thread.


### Reply

Each response begins with a header. The header contains information identifying the type of response, the type of any attached content, and the length of that content. HTTP supports additional fields in the header, but we are only interested in a subset of HTTP. The header ends with a blank line. Each line of the header must end with \r\n. The following is an example of a reply to a valid request for a file of length 5686 bytes (with the carriage-return, line-feed explicitly shown):


```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 5686\r\n
\r\n
<---- contents here ---->
```

The contents come directly from the file without any interpretation. For this assignment, your program may always specify text/html as the content type, regardless of file. The content-length can be found using the `stat` system call.

The response to a HEAD request provides the header only (no contents).

A HEAD request can be sent with the following curl command:

```
curl -v -I http://localhost:10000/file.txt
```

`-I` for informational/HEAD only, the `-v` argument provides "verbose" output detail.



# Requirement 4

Erroneous requests will be responded to with an appropriate error response. Such a response should have an error type (see below), a content type of `text/html`, and an appropriate snippet of HTML to be presented to the user (this is often just the response type). The following are some standard responses that you may find useful:

- HTTP/1.1 400 Bad Request  (malformed or unparseable HTTP request)
- HTTP/1.1 403 Permission Denied (PUT request for a regular file)
- HTTP/1.1 404 Not Found  (GET request for non-existent file or key)
- HTTP/1.1 500 Internal Error  (critical error that prevents a response from being generated; malloc() failure, pthread_create() error, etc.)
- HTTP/1.1 501 Not Implemented  (request that uses an action other than GET, PUT, or HEAD)

You are free to customize the HTML message (which becomes the contents of the response).



# Requirement 5

Your server will also provide support for data storage and retrieval (using your kvstore implementation from assignment 5) via HTTP [PUT](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/PUT) and [GET](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/GET) requests of the following form:

`PUT /kv/<key> HTTP/1.1`

`GET /kv/<key> HTTP/1.1`

A `PUT` request should create or update the value associated with the provided key.


### Example PUT Request/Response

An HTTP `PUT` request to set the value of key `a` to `"new value"` can be sent using the curl command:

```
curl -X PUT -d "new value" http://localhost:10000/kv/a
```

The HTTP request below will be sent to port 10000:

```
PUT /kv/a HTTP/1.1
Host: localhost:10000
User-Agent: curl/7.47.0
Accept: */*
Content-Length: 9
Content-Type: application/x-www-form-urlencoded

new value
```

Your `httpd` server should respond as follows:

```
HTTP/1.1 200 OK
Content-Length: 0


```

A `GET /kv/<key>` (note that the key _should not_ be surrounded with <> brackets) request should return the value corresponding to the requested key using a "200 OK" response where the response body contains just the value as a plain text string. If the key does not exist, return an HTTP 404 response.

You _do not_ need to support HEAD requests for /kv/* endpoints.



# Requirement 6

As with all programs, you must be careful to properly manage any resources used. Since a web server is supposed to run continuously, you will need to be especially careful about your resource usage. Be sure to free memory and close unused file descriptors. You can use the top program to monitor memory usage while your program executes.



# Additional Details

- The client may close the connection while the server is processing the file. Be sure to handle this case without the server crashing.
- We will require the user to always specify a filename. No special action will be taken if no file name is provided (i.e., the request has /; many servers will attempt to open a default file such as `index.html.`).
- We will not support the notion of users, so a URL with a ~ will not attempt to search a home directory.
- All file searches will be done from the directory in which the server is executed. Your server should prevent any attempt to access files in directories above its working directory (e.g., using `..`). If such a request is received, reply with an error. Note that some browsers will automatically remove any `..` in a filename.



# Testing

You can certainly use a web browser to test your program. An alternative is to use `curl` or another HTTP client (or `telnet` which you might install on your own machine. When you execute `telnet` you can provide the machine name (or IP address) and a port. `telnet` will then provide a prompt at which you can input data to be sent to the server.).



# Deliverables

- Source Code (and `Makefile`) - Push all relevant source code and an appropriate `Makefile` to your repository.
- During testing, the autograder creates files/directories with the prefix: `ag_` Please make sure your repository does not include files or directories named with this prefix.
- Please do not commit compiled/executable files to git (ie. do not add: `a.out`, `httpd` (the executable file, without an extension), `main.o`, etc.)
- For full credit, your git commit history must be sufficiently detailed to illustrate how you approached the assignment, addressed bugs, etc. Commit often, with descriptive commit labels. For example: "initial data structures and function prototypes", "implemented the -a switch to display hidden files", "solved resource leak in function xyz()"  There is no fixed minimum number of commits. As one example guideline, you will not be eligible for full credit if your submitted git repository includes just 2-3 commits where large blocks of code are added in each commit.



# Rubric
![httpd_rubric](https://github.com/jerhuan205/CSC357-SystemsProgramming/assets/122332863/a444ab80-1add-4cc8-bbcc-90c82342643a)














