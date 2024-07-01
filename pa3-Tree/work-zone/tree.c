#include "tree.h"

// Function compares two strings from the dirent struct
// Returns 0 if the strings are the same
int compare(const struct dirent** a, const struct dirent** b)
{
	return strcasecmp( (*a)->d_name, (*b)->d_name );
}



// Debug function that just echoes the passed args in program
void echo_args(int a_flag, int s_flag, char* path)
{
	printf("s_flag|%d|\n", s_flag);
	printf("a_flag|%d|\n", a_flag);
	printf("path  |%s|\n", path);
	return;
}



// Function that recursively goes through the directories, sorts them in alphabetical order, then prints the names in a tree-like structure
void tree_recurse(int s_flag, int a_flag, int level, int* dir_count, int* file_count, char* path)
{
	DIR* p_dir;
	struct dirent* entry;
	struct stat statbuf;
	struct dirent** name_list;
	int num_entries;

	// If the opened directory from "path" is NULL, then print an error.
	// The opened directory should never yield an error due to main.
	if ((p_dir = opendir(path)) == NULL)
	{
		// Prints if opendir is successful or not
		perror("opendir");
		return;
	}

	// Change to the current directory to prevent the need of absolute paths
	// chdir returns 0 if successful, else -1
	if (chdir(path) != 0)
	{
		perror("chdir");
		closedir(p_dir);
		return;
	}

	// Read all entries into a list of names
	num_entries = scandir(".", &name_list, NULL, compare);
	// scandir scans the directory ".", sorted using qsort, and collected in &namelist.
	// returns # of directory entries, -1 on error
	if (num_entries < 0)
	{
		perror("scandir");
		closedir(p_dir);
		return;
	}

	// Iterated over the sorted entries
	for (int i = 0; i < num_entries; i++)
	{
		entry = name_list[i];

		// Skip "." and ".." to prevent infinite loop
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
		{
			free(entry);
			continue;
		}

		// Skip any hidden files unless a_flag is set
		// The inodes "." and ".." count as hidden, so they must be passed over to avoid infinite looping
			// "-a" IS on & hidden file IS present, display its information accordingly
		if (a_flag == A_FLAG_OFF && entry->d_name[0] == '.')
		{
			free(entry);
			continue;
		}

		// Get the stats of the entry with no Symbolic links for name, mode, and size
		if (lstat(entry->d_name, &statbuf) == -1)
		{
			perror("lstat");
			free(entry);
			continue;
		}

		// Display the entry in appropriate format
		for (int i = 0; i < level; i++)
			{ printf("|   "); }

		// If the current entry is the last in the list, print an ending character
		if (i == num_entries - 1)
			{ printf("`-- "); }
		// Otherwise, print with an entry character
		else	{ printf("|-- "); }

		// Check the s_flag for displaying the size of the file
		if (s_flag == S_FLAG_ON)
			{ printf("[%11ld]  %s\n", statbuf.st_size, entry->d_name); }
		else 	{ printf("%s\n", entry->d_name); }

		// If it is a directory
		if (S_ISDIR(statbuf.st_mode))
		{
			// Increment directory number count
			(*dir_count)++;

			// Recurse into the directory
			tree_recurse(s_flag, a_flag, level + 1, dir_count, file_count, entry->d_name);

			// Change back to the parent directory
			if (chdir("..") != 0)
			{
				perror("chdir");
				closedir(p_dir);
				return;
			}
		}
		// Otherwise it is a file
		else
		{
			// Increment file number count
			(*file_count)++;
		}

		// Free the entry when we're done with the recursion and printing
		free(entry);
	}
	// Free our malloc'd namelist array
	free(name_list);

	// Close the directory
	closedir(p_dir);
	return;
}



/* ------------------------------------------------------------ MAIN PROGRAM RUNS HERE ------------------------------------------------------------ */
int main(int argc, char *argv[])
{
	// Check if 2 < number of args < 4
	if (argc < 2 || argc > 4)
	{
		printf("Incorrect number of arguments\n");
		return 1;
	}
	else
	{
		// There are 3 cases for user input:
		// 1 ./<"tree"> <directory>
		// 2 ./<"tree"> <switch> <directory>
		// 3 ./<"tree"> <switch_1> <switch_2> <directory>

		// In any of these cases, loop through the arguments starting at the 2nd and ending before directory
		int a_flag = A_FLAG_OFF;
		int s_flag = S_FLAG_OFF;
		char* path;
		struct stat dir_info;
		for (int i = 1; i < (argc - 1); i++)
		{
			// If the optional argument is a flag, strcmp will return 0 as True
			if (!strcmp(argv[i], "-s"))
			{
				s_flag = S_FLAG_ON;
			}
			else if (!strcmp(argv[i], "-a"))
			{
				a_flag = S_FLAG_ON;
			}
		}
		// The last argument is the path
		path = argv[argc - 1];

		// TODO: This function just echoes the parameters to shell as a test
//		echo_args(a_flag, s_flag, path);

		// Store path information into variable: dir_info
		stat(path, &dir_info);

		// Check if the path is valid,
		// S_ISDIR returns 0 if the st_mode of a stat struct is not a directory
		if (!S_ISDIR(dir_info.st_mode))
		{
			printf("Error, '%s' not a valid directory.\n", path);
			return 1;
		}

		// With a valid path, recurse through tree with the level and current counts of dirs & files
		int dir_count = 0;
		int file_count = 0;
		int level = 0;
		printf("%s\n", path);
		tree_recurse(s_flag, a_flag, level, &dir_count, &file_count, path);

		// Display the count of dirs & files.
		printf("\n%d directories, %d files\n", dir_count, file_count);
		return 0;
	}
}
