#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_INODES 1024
#define MIN_INODES 0
#define FNAME_SIZE 32
#define NULL_TERM 1
#define SPACE 1

// Each inode has an associated index (a number) and type ('d' or 'f')
typedef struct {
	uint32_t index;
	char 	 type;
} Inode;

// Each directory will always display entries that have an inode (or similar to Inode type: index) and a name of up to 32 chars.
typedef struct {
	uint32_t inode;
	char 	 name[FNAME_SIZE + NULL_TERM];
} Entry;

// In memory...
// The inodes_list will be represented as an Array, holding Inode types.

// A directory will be represented as an Array, holding a sequence of Entry types.
// This is an inode that can point to other inodes through type Inode index / Entry inode

// A file will be represented as just a single instance of a name that can be up to 32 chars.



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



// Function reads the "inodes_list" file and populates the inodes_list contents in memory
// Returns the number of current inodes present in simulation
int read_inodes_list(Inode* inodes_list)
{
	// Load the inodes_list file
	FILE* inodes_list_fp = fopen("inodes_list", "r");
	char buff[5];
	int num_inodes = MIN_INODES;
	int inode_size = sizeof(uint32_t) + sizeof(char);
	int one_inode = 1;

	// Read 1 inode (5 bytes) from the inodes_list and store this in buff until we are at the end of "inodes_list"
		// fread(*buffer, size of n, n elements to read, *file_stream);
	while (num_inodes < MAX_INODES && fread(buff, inode_size, one_inode, inodes_list_fp) != 0)
	{
		inodes_list[num_inodes].index = buff[0];
		inodes_list[num_inodes].type  = buff[4];
		num_inodes++;
	}
	fclose(inodes_list_fp);

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



// Function reads the directory file and populates the dir_list contents in memory
// Returns the number of entries in the list
// This number is also the index of the next entry to add in this directory.
int read_dir_list(Entry* dir_list, char* dir_name, int rem_inodes)
{
	// Load the specified file according to directory name
	FILE* dir_file = fopen(dir_name, "r");
	if (dir_file == NULL)
	{
		return -2;
	}
	uint32_t buff_inode;
	char buff_name[FNAME_SIZE + NULL_TERM];
	int num_inodes = 0;
	int inode_index_size = sizeof(uint32_t);
	int one_index = 1;

	// Read separately:
	// First 1 inode index (4 bytes) from the directory's file and store this in buff until we are at the end of dir_file
	while (fread(&buff_inode, inode_index_size, one_index, dir_file) == 1)
	{
		// Fill our 'current working directory' with information
		dir_list[num_inodes].inode = buff_inode;

		// Next 32 chars of the inode's name (1 byte) from the directory's file and store this in buff_name
		fread(buff_name, sizeof(char), FNAME_SIZE, dir_file);
		buff_name[FNAME_SIZE] = '\0';

		// Copy the inode's name for the entry into our 'current working directory'
		strcpy(dir_list[num_inodes].name, buff_name);
		num_inodes++;
	}
	fclose(dir_file);

	// Initialize the remaining list for reminder that:
	// 	-1 is nonexistent inode,
	//	A string of '\0' is nonexistent inode
	for (int i = num_inodes; i < rem_inodes; i++)
	{
		dir_list[i].inode = -1;
		for (int j = 0; j < FNAME_SIZE + 1; j++)
		{
			dir_list[i].name[j] = '\0';
		}
	}

	// Return the current size / number of entries in the directory
	return num_inodes;
}



// DEBUGGING FUNCTIONS
void echo_present_inodes(Inode* inodes_list)
{
	printf("DBUG: inodes_list contents\n");
	for (int i = MIN_INODES; inodes_list[i].index != -1 || inodes_list[i].type != '\0'; i++)
	{
		printf("\t%d:%d %c\n", i, inodes_list[i].index, inodes_list[i].type);
		// If somehow the condition breaks from other things happening in the program,
		//  then this will print what the inodes_list holds and lets us know that the condition failed.
		if (i == 69) {printf("\t...breaking...\n"); break;}
	}
}

void echo_n_inodes(Inode* inodes_list, int n_inodes)
{
	printf("DBUG: inodes_list contents up to %d inodes\n", n_inodes);
	for (int i = MIN_INODES; (i < n_inodes) ; i++)
	{
		printf("\t%d:%d %c\n", i, inodes_list[i].index, inodes_list[i].type);
	}
}

void echo_cur_dir(Entry* dir_list)
{
	printf("DBUG: dir_list contents\n");
	for (int i = MIN_INODES; dir_list[i].inode != -1 || dir_list[i].name[0] != '\0'; i++)
	{
		printf("\t%d:%d %s\n", i, dir_list[i].inode, dir_list[i].name);
		// If somehow the condition breaks from other things happening in the program,
		//  then this will print what the inodes_list holds and lets us know that the condition failed.
		if (i == 69) {printf("\t...breaking...\n"); break;}
	}
}

void echo_n_entries(Entry* dir_list, int n_entries)
{
	printf("DBUG: dir_list contents up to %d entries\n", n_entries);
	for (int i = MIN_INODES; (i < n_entries) ; i++)
	{
		printf("\t%d:%d %s\n", i, dir_list[i].inode, dir_list[i].name);
	}
}



// MAIN PROGRAM RUNS HERE
int main(int argc, char* argv[])
{
	// Check if the arguments are valid first
	parse_args(argc, argv[1]);

	// Array holding 1024 possible entries of type Inode from the file "inodes_list"
	Inode inodes_list[MAX_INODES];

	// Read the "inodes_list" file and get the current number of inodes
	int cur_inodes = read_inodes_list(inodes_list);
	int starting_inodes = cur_inodes;
	int rem_inodes = MAX_INODES - cur_inodes;

	/* T E S T I N G */
	printf("cur_inodes-%d\n", cur_inodes);
	printf("rem_inodes-%d\n", rem_inodes);
	echo_present_inodes(inodes_list);	// This should yield the same displayed contents as "xxd -c 5 fs/inodes_list"
	printf("\n-s-p-a-c-e-\n");
	echo_n_inodes(inodes_list, 10);

	// Array holding the entries of our 'current working' directory
	Entry dir_list[rem_inodes];

	// For simulation, user starts in directory inode 0.
	Entry current_directory = {0, "0"};
//	uint32_t dir_index = 0;
//	char dir_name[] = "0";

	// Read the current_directory file and get the current number of entries
	int num_entries = read_dir_list(dir_list, current_directory.name, rem_inodes);

	/* T E S T I N G */
	printf("cur_dir inode:%d, cur_dir name:%s\n", current_directory.inode, current_directory.name);
	echo_cur_dir(dir_list); // This should yield the same displayed contents as "xxd -c 36 fs/0"
	printf("\n-s-p-a-c-e-\n");
	echo_n_entries(dir_list, 10);
}
