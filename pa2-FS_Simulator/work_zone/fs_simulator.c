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

// Function parses and verifies program arguments for correct startup
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

// Function reads the "inodes_list" file and returns the number of possible entries left
int read_inodes_list(Inode* inodes_list)
{
	// Load the inodes_list file
	FILE* inodes_list_file = fopen("inodes_list", "r");
	char buff[5];
	int num_inodes = MIN_INODES;
	int inode_size = sizeof(uint32_t) + sizeof(char);
	int one_inode = 1;

	// Read 1 inode (5 bytes) from the inodes_list and store this in buff until we are at the end of "inodes_list"
		// fread(*buffer, size of n, n elements to read, *file_stream);
	while (num_inodes < MAX_INODES && fread(buff, inode_size, one_inode, inodes_list_file) != 0)
	{
		inodes_list[num_inodes].index = buff[0];
		inodes_list[num_inodes].type  = buff[4];
		num_inodes++;
	}
	fclose(inodes_list_file);

	// Note: num_inodes is 1 behind, but if it is bigger or equal to the max number of possible inodes there's an issue
	if (num_inodes >= MAX_INODES - 1)
	{
		return -1;
	}

	// Initialize the remaining list for reminder that:
	// 	-1 is nonexistent inode,
	//	'\0' is nonexistent type
	for (int i = num_inodes; i < MAX_INODES; i++)
	{
		inodes_list[i].index = -1;
		inodes_list[i].type  = '\0';
	}

	// Return the current size of our array / number of current inodes
	return num_inodes;
}

int main(int argc, char* argv[])
{
	// Check if the arguments are valid first
	parse_args(argc, argv[1]);

	// Array holding 1024 possible contents from the file "inodes_list"
	Inode inodes_list_entries[MAX_INODES];

	// Read the "inodes_list" file and get the current number of inodes
	int cur_inodes = read_inodes_list(inodes_list_entries);

	int starting_inodes = cur_inodes;
	int rem_inodes = MAX_INODES - cur_inodes;

	printf("cur_inodes-%d\n", cur_inodes);
	printf("rem_inodes-%d\n", rem_inodes);
}
