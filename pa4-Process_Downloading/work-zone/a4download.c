#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_TIME 5



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
		printf("Warning: default maximum number for downloading processes of 5 will be used.\n");
		return DEFAULT_TIME;
	}
	else
	{
		// Convert the second argument to a long integer with end_ptr pointing to '\0'
		char* endptr;
		int max_time = strtol(argv[2], &endptr, 10 /*base*/);

		// Check for conversion errors or for an invalid time value
		if (*endptr != '\0' || max_time <= 0)
		{
			fprintf(stderr, "Invalid time: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		return max_time;
	}
}

int main(int argc, char* argv[])
{
	// Check if the number of arguments are valid first
	char* input_file = argv[1];
	int max_process_time = parse_args(argc, argv);

	// Open the file with the download links
	FILE *fp = fopen(input_file, "r");
	if (fp == NULL)
	{
		printf("Error: failed to read file %s.\n", input_file);
		return -1;
	}

	// need some kind of struct to hold the information from the file when reading
	// an int representing seconds
	// some string representing file name
	// some string representing url
}
