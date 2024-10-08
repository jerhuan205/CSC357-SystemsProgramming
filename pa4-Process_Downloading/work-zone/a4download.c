#include "a4download.h"



// Global variables to help with signal catching and freeing
FileEntry* file_info;

int n_lines = 0;



// Function free's the array holding file information, clearing all heap data
// Returns nothing
void free_file_info()
{
	// Free mallocs: array holding file information before closing
	for (int i = 0; i < n_lines; i++)
	{
		free(file_info[i].file_name);
		free(file_info[i].url);
	}
	free(file_info);
}



// Function catches the specified program terminations and clears the heap
void sig_handler(int signum)
{
	if 	(signum == SIGINT ) 	{ printf("Received SIGINT.\n"); }
	else if (signum == SIGQUIT) 	{ printf("Received SIGQUIT.\n");}
	free_file_info();
	exit(signum);
}



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
			fprintf(stderr, "Invalid download cap: %s\n", argv[2]);
			exit(EXIT_FAILURE);
		}
		return max_downloads;
	}
}



// Function parses the user specified input_file and populates an array holding download information
// Returns a pointer to the array
// Note: It is not possible to instantiate file_info in main and pass it into a function as a pointer
//	 to mutate it with mallocs and reallocs so we just return the pointer to the data.
FileEntry* copy_file_contents(char* input_file, int* n_lines)
{
	// Open the file with the download links
	FILE *fp = fopen(input_file, "r");
	if (fp == NULL)
	{
		printf("Error: failed to read file %s.\n", input_file);
		exit(EXIT_FAILURE);
	}

	// Initialize an array and necessary buffers to hold all of the line entries in the file
	FileEntry* file_info = NULL;
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

	return file_info;
}



// Function executes "curl" shell command per child request
// Returns nothing and exits once shell command is completed
void child_process(int timeout, char* file_name, char* url)
{
	char seconds_str[10];
	sprintf(seconds_str, "-m %d", timeout);

	execl("/usr/bin/curl", "curl", seconds_str, "-o", file_name, "-s", url, NULL);
	exit(0);
}



// Function operates the "waiting" portion of the program each child request
// Returns nothing
void parent_process(pid_t process_ids[], int n_lines)
{
	// waitpid with -1 to wait for any child process to finish and its pid is stored
	int status;
	pid_t finished_child = waitpid(-1, &status, 0);

	// Search for finished child in our array of process ids
	for (int i = 0; i < n_lines; i++)
	{
		if (finished_child == process_ids[i])
		{
			// Check child exit status, if successful:
			int exit_status = WEXITSTATUS(status);
			if (exit_status == 0)
				{ printf("process %d processing line %d exited normally\n", finished_child, i + 1); }
			// If not successful, then display appropriate message
			else	{ printf("process %d processing line %d terminated with exit status: %d\n", finished_child, i + 1, exit_status); }
			// adding this break removes a valgrind error
			// "Conditional jump or move depends on uninitialised value(s)"
			break;
		}
	}
}



/* ------------------------------------------------------------ MAIN PROGRAM RUNS HERE ------------------------------------------------------------ */
int main(int argc, char* argv[])
{
	// Check if the number of arguments are valid first
	char* input_file = argv[1];
	int max_downloads = parse_args(argc, argv);

	// Signals to catch any program terminations.
	if (signal(SIGQUIT, sig_handler) == SIG_ERR) // "CTRL + \"
		{ printf("unable to register handler for SIGQUIT\n"); 	return 1; }
	if (signal(SIGINT, sig_handler) == SIG_ERR) // "CTRL + C"
		{ printf("unable to register handler for SIGINT\n"); 	return 1; }

	// Instantiate the array and populate it with information from the input_file
	file_info = copy_file_contents(input_file, &n_lines);

	/* - - - - - The downloading - - - - - - */

	// Set the max_downloads to the number of FileEntries in the file to prevent over-downloading
	if (max_downloads > n_lines)
	{
		// These prints aren't necessary, but are good for debugging or clarity for user.
		// printf("Number of max processes, %d, specified by user exceeds number of file entries, %d.\n", max_downloads, n_lines);
		max_downloads = n_lines;
		// printf("Set to cap\n");
	}

	// Array holding pid information
	pid_t process_ids[n_lines];
	int process_count = 0;
	int total_downloaded = 0;
	int cur_spot = 0;

	// Begin process downloading until we have downloaded all of the files.
	while (total_downloaded < n_lines)
	{
		// There are 2 issues occur that this check solves:
		// 1. cur_spot will exceed n_lines and will download past the number of valid entries
		//	"curl: option -m -1: expected a positive numerical parameter"
		// 2. entering a max_downloads greater than 1 will speed up the a4download program and
		//    the program finishes way before the curl downloads ever finish
		if (cur_spot >= n_lines)
		{
			parent_process(process_ids, n_lines);
			process_count--;
			total_downloaded++;
			continue;
		}

		// Begin child processes
		pid_t pid = fork();
		process_count++;

		// fork() error handler
		if (pid == -1)
		{
			printf("Fork Error\n");
			return -1;
		}
		// Child process: fork() returns 0
		else if (pid == 0)
		{
			// On fork, begin the downloading
			printf("process %d processing line %d\n", getpid(), file_info[cur_spot].line_number);

			// Helper function for the downloading: curl -m <seconds> -o <filename> -s <url>
			child_process(file_info[cur_spot].timeout, file_info[cur_spot].file_name, file_info[cur_spot].url);
		}
		// Parent process: fork() returns the child's process ID (pid)
		else
		{
			// Copy the child's pid into our array holding all of the process ids
			process_ids[cur_spot] = pid;

			// This check fixes the problem of downloads only occuring one at a time because
			//	as long as we haven't gone through all entries yet, cur_spot will keep
			//	incrementing. This ensures that with each fork a process_count will increase
			//	and with it an associated download at the correct spot in file_info
			if (process_count < max_downloads)
			{
				cur_spot++;
				continue;
			}

			// Helper function for parent, waiting for any child to be done with their download
			parent_process(process_ids, n_lines);

			// On completed download, allow for another processes by decrementing and incrementing our totals
			process_count--;
			total_downloaded++;
			cur_spot++;
		}
	}
	free_file_info();
}
