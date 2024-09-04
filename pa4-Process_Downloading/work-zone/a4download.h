/* IMPORTS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

/* CONSTANTS */
#define DEFAULT_DOWNLOADS 2
#define MAX_LINE_CHARS 256
#define MAX_NAME_CHARS 64
#define MAX_URL_CHARS  124
#define MAX_DOWNLOAD_TIME 20


/* STRUCTS */
// Information regarding a file will follow in the particular order:
typedef struct {
	char* file_name;	// 1. Output file name,
	char* url;		// 2. URL,
	int timeout;		// 3. optional max-number of seconds for download
	int line_number;	// 4. the line number to assist in display and access
} FileEntry ;
// a variable called file_info will be the array holding a FileEntry
// and each FileEntry is a line extracted from the file, holding these 4 key pieces

/* PROGRAM FUNCTIONS */
