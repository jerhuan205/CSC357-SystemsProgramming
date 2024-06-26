#include "fs_simulator.h"

// Helper function provided by program assignment instructions
char *uint32_to_str(uint32_t i)
{
	int length = snprintf(NULL, 0, "%lu", (unsigned long)i);   // pretend to print to a string to determine length
	char* str = malloc(length + 1);                            // allocate space for the actual string
	snprintf(str, length + 1, "%lu", (unsigned long)i);        // print to string

	return str;
}



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
int load_inodes_list(Inode* inodes_list)
{
	// Open the inodes_list file
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



// Function initializes the remaining entries of a directory with nonexistent blocks
void init_rem_entries(Entry* dir_list, int start, int end)
{
	// Start and End represent the number of entries to loop through
	for (int i = start; i < end; i++)
	{
		// Indices of '-1' ints represent our "NULL"
		dir_list[i].inode = -1;

		// Names of 32 '\0' chars represent our "NULL"
		for (int j = 0; j < FNAME_SIZE + NULL_TERM; j++)
		{
			dir_list[i].name[j] = '\0';
		}
	}
}



// Function reads the directory file and populates the dir_list contents in memory
// Returns the number of entries in the list
int load_directory(Entry* dir_list, char* dir_name, int rem_inodes)
{
	// Open the specified file according to directory name
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
	// fread(*buffer, size of n, n elements to read, *file_stream);
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
	init_rem_entries(dir_list, num_inodes, rem_inodes);

	// Return the current size / number of entries in the directory
	return num_inodes;
}



// Function compares the user input string with program key-strings
// Returns a constant associated to a specified string
int get_command(const char *cmd)
{
	// If it is a Program Simulation command:
	if (strcmp(cmd, "ls") == 0) 	{return CMD_LS;}
	if (strcmp(cmd, "cd") == 0) 	{return CMD_CD;}
	if (strcmp(cmd, "mkdir") == 0) 	{return CMD_MKDIR;}
	if (strcmp(cmd, "touch") == 0) 	{return CMD_TOUCH;}
	if (strcmp(cmd, "exit") == 0) 	{return CMD_EXIT;}

	// If it is a Debugging command:
	if (strcmp(cmd, "e_ilist") == 0)   {return DEV_INODES_LIST;}
	if (strcmp(cmd, "e_ninodes") == 0) {return DEV_N_INODES;}
	if (strcmp(cmd, "e_dir") == 0)     {return DEV_DIRECTORY;}
	if (strcmp(cmd, "e_nitems") == 0)  {return DEV_N_ENTRIES;}

	return CMD_UNKNOWN;
}



/* ------------------------------------------------------------ SIMULATOR FUNCTIONS ------------------------------------------------------------ */
// Function displays the entries of the current working directory of Simulator Program
void fs_ls(char* dir_name)
{
	// Open the file name of the directory
	FILE* dir_file = fopen(dir_name, "r");
	char buff[36];
	int line_size = sizeof(buff);
	int one_line = 1;

	// Read all 36-bytes of characters: 4-byte inode, 32-byte name
	// The inode index from the directory's file is stored in the first 4 spots of buff
	// The next 32 spots are characters that make up the inode's name.
	// fread(*buffer, size of n, n elements to read, *file_stream);
	while (fread(buff, line_size, one_line, dir_file) != 0)
	{
		// We first want to display the inode number.
		printf("%d ", buff[0]);

		// Next are the individual characters making up the inode name.
		for (int i = 4; i < line_size; i++)
		{
			// Stop displaying characters for ASCII values of '0'
			if (buff[i] == 0) { break; }
			printf("%c", buff[i]);	// for ASCII-values, %c instead of %s
		}
		printf("\n");
	}
	fclose(dir_file);
}

// Function changes the current working directory information and repopulates the array
void fs_cd(Inode* inodes_list, Entry* dir_list, Entry* current_directory, char* args, int* free_spot, int rem_inodes)
{
	// Loop through the entries in our current directory for a match
	int new_dir = -1;
	for (int i = 0; i < *free_spot; i++)
	{
		if (strcmp(args, dir_list[i].name) == 0)
		{
			new_dir = i;
			break;
		}
	}

	// If no match was found, print a message similar to shell
	if (new_dir == -1)
	{
		printf("cd: %s: No such file or directory\n", args);
		return;
	}

	// If the match is not a directory, print a message similar to shell
	if (inodes_list[(dir_list[new_dir].inode)].type == 'f')
	{
		printf("cd: %s: Not a directory\n", dir_list[new_dir].name);
		return;
	}

	// Update our current working directory, first the inode
	current_directory->inode = dir_list[new_dir].inode;

	// Next, the new name needs to be formatted into a string and copied
	char* new_dir_name = uint32_to_str(dir_list[new_dir].inode);
	strcpy(current_directory->name, new_dir_name);
	free(new_dir_name);	// release the pointed memory

	// Load the new directory and populate our new directory with its entries
	*free_spot = load_directory(dir_list, current_directory->name, rem_inodes);
}

// Function creates a new Entry instance in memory and creates a new directory in the shell
void fs_mkdir(Inode* inodes_list, Entry* dir_list, Entry* current_directory, char* args, int* free_spot, int* rem_inodes, int* cur_inodes)
{
	// If we don't have enough space for more inodes, then report an error
	if (*cur_inodes >= *rem_inodes)
	{
		printf("Error, not enough space for another directory.\n");
		return;
	}

	// Loop through the entries in our current directory for a match
	int new_dir_index = -1;
	for (int i = 0; i < *free_spot; i++)
	{
		if (strcmp(args, dir_list[i].name) == 0)
		{
			new_dir_index = -1;
			break;
		}
		new_dir_index = i;
	}

	// If a file or directory exists with that particular name, print a message similar to shell
	if (new_dir_index == -1)
	{
		printf("mkdir: cannot create directory '%s': File exists\n", args);
		return;
	}

	// Add the new entry to our inodes list, with its inode as the current number of inodes and file type of 'd'
	inodes_list[*cur_inodes].index = *cur_inodes;
	inodes_list[*cur_inodes].type = 'd';

	// Add the new entry to our 'current working directory' at the free spot in our array
	dir_list[*free_spot].inode = *cur_inodes;
	strcpy(dir_list[*free_spot].name, args);

	// Create a new instance for the Directory
	Entry new_dir_list[2];

	// Initialize the Directory with nonexistent blocks.
	init_rem_entries(new_dir_list, 0, 2);

	// Set the first inode to itself followed by the '.'
	new_dir_list[0].inode = (uint32_t)*cur_inodes;
	new_dir_list[0].name[0] = '.';

	// Set the second inode to current followed by the '..'
	new_dir_list[1].inode = current_directory->inode;
	new_dir_list[1].name[0] = '.';
	new_dir_list[1].name[1] = '.';

	// Create the new inode file, containing itself and the current directory
	char* new_dir_name = uint32_to_str(inodes_list[*cur_inodes].index);
	FILE* new_dir_file = fopen(new_dir_name, "wb");

	// Write 1 4-byte inode and 32 1-byte chars: '.' / ASCII-value 46 (0x2e)
	fwrite(&(new_dir_list[0].inode), sizeof(uint32_t), 1, new_dir_file);
	fwrite(&(new_dir_list[0].name), sizeof(char), FNAME_SIZE, new_dir_file);

	// Write the 4-byte current inode + 32-byte chars: '..' / 46-46 (0x2e-0x2e)
	fwrite(&(new_dir_list[1].inode), sizeof(uint32_t), 1, new_dir_file);
	fwrite(&(new_dir_list[1].name), sizeof(char), 32, new_dir_file);

	fclose(new_dir_file);
	free(new_dir_name);

	// Open the current directory's inode file and append the new directory to its data
	FILE* dir_file = fopen(current_directory->name, "ab");
	fwrite(&(dir_list[*free_spot].inode), sizeof(uint32_t), 1, dir_file);
	fwrite(&(dir_list[*free_spot].name), sizeof(char), 32, dir_file);
	fclose(dir_file);

	// Increment the current number of inodes and size of our current directory
	(*cur_inodes)++;
	(*free_spot)++;

	// Decrement the number of remaining inodes we have left to create
	(*rem_inodes)--;
}

// Function adds the changes made to our inodes_list in Memory to the 'inodes_list' file
void fs_exit(Inode* inodes_list, int starting_inodes)
{
	// Open the "inodes_list" file
	FILE* inodes_list_file = fopen("inodes_list", "ab");
	// TODO: ^ - opening in wb may not be necessary because we are just adding the new entries as a result of mkdir and touch
		// - opening in ab might be better, starting at 'cur_inodes' up until the end of inodes_list to append the entrie
	// Interesting note:
		// - wb may make sense for when we have to do a 'remove' function

	// Loop over all entries of our Array holding our current "inodes_list", writing each entry to the "inodes_list" file
	for (int i = starting_inodes; i < MAX_INODES; i++)
	{
		if (inodes_list[i].type == 'd' || inodes_list[i].type == 'f')
		{
			fwrite(&(inodes_list[i].index), sizeof(uint32_t), 1, inodes_list_file);
			fwrite(&(inodes_list[i].type), sizeof(char), 1, inodes_list_file);
		}
	}
	fclose(inodes_list_file);
	exit(0);
}



/* ------------------------------------------------------------ DEBUGGING FUNCTIONS ------------------------------------------------------------ */
// "e_ilist"
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

// "e_ninodes"
void echo_n_inodes(Inode* inodes_list, int n_inodes)
{
	printf("DBUG: inodes_list contents up to %d inodes\n", n_inodes);
	for (int i = MIN_INODES; (i < n_inodes) ; i++)
	{
		printf("\t%d:%d %c\n", i, inodes_list[i].index, inodes_list[i].type);
	}
}

// "e_dir"
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

// "e_nitems"
void echo_n_entries(Entry* dir_list, int n_entries)
{
	printf("DBUG: dir_list contents up to %d entries\n", n_entries);
	for (int i = MIN_INODES; (i < n_entries) ; i++)
	{
		printf("\t%d:%d %s\n", i, dir_list[i].inode, dir_list[i].name);
	}
}



/* ------------------------------------------------------------ MAIN PROGRAM RUNS HERE ------------------------------------------------------------ */
int main(int argc, char* argv[])
{
	// Check if the arguments are valid first
	parse_args(argc, argv[1]);

	// Array holding 1024 possible entries of type Inode from the file "inodes_list"
	Inode inodes_list[MAX_INODES];

	// Load the "inodes_list" file and get the current number of inodes
	// At the same time, populate our inodes_list array
	int cur_inodes = load_inodes_list(inodes_list);
	int starting_inodes = cur_inodes;
	int rem_inodes = MAX_INODES - cur_inodes;

	// Array holding the entries of our 'current working' directory
	Entry dir_list[rem_inodes];

	// For simulation, user starts in directory inode 0.
	Entry current_directory = {0, "0"};
	//	uint32_t dir_index = 0;
	//	char dir_name[] = "0";

	// Load the current_directory file and get the current number of entries
	// At the same time, populate our array of entries (i.e. a Directory)
	int num_entries = load_directory(dir_list, current_directory.name, rem_inodes);

	// Buffers to store User input
	char input[9 + SPACE + FNAME_SIZE + SPACE + NULL_TERM];
	char cmd[9 + NULL_TERM] = "";
	char args[FNAME_SIZE + NULL_TERM] = "";
	while (1)
	{
		// Flush the input each time
		for (int i = 0; i < sizeof(input); i++)
		{
			input[i] = 0;
		}
		for (int i = 0; i < sizeof(cmd); i++)
		{
			cmd[i] = 0;
		}
		for (int i = 0; i < sizeof(args); i++)
		{
			args[i] = 0;
		}

		// Prompt user for commands to read as text from stdin
		printf("> ");

		// Read the user input until a new-line character or EOF
		// fgets(*string, n chars to read, *file_stream);
		if (fgets(input, sizeof(input), stdin) != NULL)
		{
			// Scan the input string and replace the new-line inputted by the user with a null terminator
			input[strcspn(input, "\n")] = '\0';

			// If the user inputs more than 42 chars, then reprompt user for input
			if (strlen(input) == sizeof(input) - 1 && input[sizeof(input) - 2] != '\n')
			{
				printf("Input exceeded maximum length. Please enter a shorter input.\n");

				// Flush out remaining characters when there is an overflow
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
				continue;
			}
		}
		else
		{
			printf("Error reading input.\n");
			continue;
		}

		// Parse the input for the command and arguments
		sscanf(input, "%s %s", cmd, args);

		// Unecessary for project requirements, but will make user interface cleaner to interact with
		if (strlen(cmd) >= sizeof(cmd))
		{
			printf("Input exceeded maximum length for command. Please enter an input under less than 9 characters.\n");
			continue;
		}

		// If the arg size is more than 32 chars, then truncate down to 32
		int len_args = strlen(args);
		if (len_args > FNAME_SIZE)
		{
			args[32] = '\0';
			len_args = 32;
			printf("Argument truncated down to 32 bytes %s\n", args);
		}

		// Get the command constant from user input string
		int fs_cmd = get_command(cmd);

		// File Simulator commands
		switch (fs_cmd)
		{
			case CMD_LS:
				// Display the contents of our 'current working directory'
				fs_ls(current_directory.name);
				break;
			case CMD_CD:
				// Change our 'current working directory': inode, name, contents, and size
				fs_cd(inodes_list, dir_list, &current_directory, args, &num_entries, rem_inodes);
				break;
			case CMD_MKDIR:
				// Create a new directory and add to our 'current working directory' at the size of our array
				fs_mkdir(inodes_list, dir_list, &current_directory, args, &num_entries, &rem_inodes, &cur_inodes);
				break;
			case CMD_TOUCH:
				printf("matches touch\n");
				break;
			case CMD_EXIT:
				// Exit program, writing to the "inodes_list" file any changes since our starting number of inodes
				fs_exit(inodes_list, starting_inodes);
				break;

			// The rest are not necessary, but just easier than hardcoding in the inode # and name
			// when debugging the in-memory contents.
			case DEV_INODES_LIST:
				echo_present_inodes(inodes_list); // This should yield the same displayed contents as "xxd -c 5 fs/inodes_list"
				break;
			case DEV_N_INODES:
				echo_n_inodes(inodes_list, 10);
				break;
			case DEV_DIRECTORY:
				echo_cur_dir(dir_list); // This should yield the same displayed contents as "xxd -c 36 fs/0"
				break;
			case DEV_N_ENTRIES:
				echo_n_entries(dir_list, 10);
				break;
			default:
				printf("nothing \n");
		}
		// End of Switch
	}
	// End of program
}
