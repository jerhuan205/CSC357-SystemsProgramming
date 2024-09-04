#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define DEFAULT_DOWNLOADS 2
#define MAX_LINE_CHARS 256
#define MAX_NAME_CHARS 64
#define MAX_URL_CHARS  124
#define MAX_DOWNLOAD_TIME 20

// Information regarding a file will follow in the particular order:
// 1. Output file name,
// 2. URL,
// 3. optional max-number of seconds for download
// file_info will be the array with each entry being a line holding these 3 pieces

typedef struct {
	char* file_name;
	char* url;
	int timeout;
	int line_number;
} FileEntry ;

// Function parses and verifies program arguments for correct startup
// Returns the max-processing time as inputted by user, a default value if unspecified
int parse_args(int argc, char* argv[])
{
	// Verify number of parameters from input
	if (argc < 2 || 3 < argc)
	{
		fprintf(stderr, "Usage: ./a4download <file-name> <(opt) max-processing-time>\n");
		exit(EXIT_FAILURE);
	}
	else if (argc == 2)
	{
		printf("Note: default number for concurrent process downloads of 2 will be used.\n");
		return DEFAULT_DOWNLOADS;
	}
	else
	{
		// Convert the second argument to a long integer with end_ptr pointing to '\0'
		char* endptr;
		int max_downloads = strtol(argv[2], &endptr, 10 /*base*/);

		// Check for conversion errors or for an invalid time value
		if (*endptr != '\0' || max_downloads <= 0)
		{
			fprintf(stderr, "Invalid download cap: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		return max_downloads;
	}
}

void copy_file_contents(char* input_file, int* n_lines, FileEntry* file_info)
{
	// Open the file with the download links
	FILE *fp = fopen(input_file, "r");
	if (fp == NULL)
	{
		printf("Error: failed to read file %s.\n", input_file);
		exit(EXIT_FAILURE);
	}

	// Initialize an array and necessary buffers to hold all of the line entries in the file
//	FileEntry* file_info = NULL;
	char line[MAX_LINE_CHARS];

	// Read the lines of the file until an EOF is reached.
	// fgets(*string, n chars to read, *file_stream);
	while (fgets(line, MAX_LINE_CHARS, fp) != NULL)
	{
		// Parse the line for the local_name, url, and max_seconds. If max_seconds not specified, set to MAX_DOWNLOAD_TIME;
		char local_name[MAX_NAME_CHARS];
		char url[MAX_URL_CHARS];
		int max_seconds;
		if (sscanf(line, "%63s %123s %d", local_name, url, &max_seconds) != 3)
		{
			max_seconds = MAX_DOWNLOAD_TIME;
		}

		// TODO: might be able to just remove this one
		if (max_seconds <= 0) { max_seconds = MAX_DOWNLOAD_TIME; }

		// Create an array to hold the information of the current line
		FileEntry line_info = {
			.file_name = malloc(strlen(local_name) + 1),
			.url = malloc(strlen(url) + 1),
			.timeout = max_seconds,
			.line_number = (*n_lines) + 1
		} ;

		// If the malloc failed, end the program
		if (!line_info.file_name || !line_info.url)
		{
			if (!line_info.file_name) { free(line_info.file_name); }
			if (!line_info.url) { free(line_info.url); }
			fprintf(stderr, "Memory allocation failed\n");
			fclose(fp);
			exit(EXIT_FAILURE); // or exit, depending on the program's needs
		}

		// Copy the strings held in the buffers to temporary memory, line_info
		strcpy(line_info.file_name, local_name);
		strcpy(line_info.url, url);

		// Reallocate memory for file_info to hold another entry of a line and its contents
		file_info = realloc(file_info, (sizeof(FileEntry) * ((*n_lines) + 1)) /* * 2*/);
		if (file_info == NULL)
		{
			printf("Could not allocate memory for <FileEntry* file_info>.\n");
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		// Now file_info is a dynamic array holding number n_lines of entries
		file_info[(*n_lines)] = line_info;
		(*n_lines)++;
	} // End of while

	// After storing file contents, null terminate the last elements of our array
	// TODO: the following line creates write errors with valgrind, but no memory leaks
	// file_info[n_lines] = (FileEntry) {NULL, NULL, -1, -1};

	// Close the file after we have gathered its information
	fclose(fp);

	for (int i = 0; i < (*n_lines); i++)
	{
		printf("%s \n", file_info[i].file_name);
	}

	return;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
int main(int argc, char* argv[])
{
	// Check if the number of arguments are valid first
	char* input_file = argv[1];
	int max_downloads = parse_args(argc, argv);

	int n_lines = 0;
	FileEntry* file_info = NULL;
	copy_file_contents(input_file, &n_lines, file_info);
	// Free all mallocs
	for (int i = 0; i < n_lines; i++)
	{
		printf("%s \n", file_info[i].file_name);
		free(file_info[i].file_name);
		free(file_info[i].url);
	}
	free(file_info);

}
