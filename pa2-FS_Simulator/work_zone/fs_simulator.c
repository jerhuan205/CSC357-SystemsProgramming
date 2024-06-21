#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_INODES 1024
#define MIN_INODES 0
#define FNAME_SIZE 32

typedef struct {
	uint32_t index;
	char 	type;
} Inode;

typedef struct {
	uint32_t index;
	char name[FNAME_SIZE + 1];
} Directory;

Inode inodes_list[MAX_INODES];

void parse_args(int argc, char* argv1)
{
	// Verify number of parameters from input
	if (argc != 2)
	{
		fprintf(stderr, "Usage: ./fs_simulator <file-name>\n");
		exit(EXIT_FAILURE);
	}

	// Try to change the current working directory to the specified directory
	// chdir returns -1 if the directory is invalid
	if (chdir(argv1) == -1)
	{
		fprintf(stderr, "Invalid input. '%s' is not a directory\n", argv1);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
	// Check if the arguments are valid first
	parse_args(argc, argv[1]);
}
