#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// Verify number of parameters from input
	if (argc != 3)
	{
		fprintf(stderr, "usage: ./kvstore <database-file_name> <input-socket-domain>\n");
		exit(EXIT_FAILURE);
	}

	// Check if the current database exists
	char* database = argv[1];
	FILE *database_fp = fopen(database, "r");
	if (database_fp == NULL)
	{
		fprintf(stderr, "Specified database file \"%s\" does not exist.\n", database);
		exit(EXIT_FAILURE);
	}

	// TODO: NEED PART 2 OF REQUIREMENT 1, CREATING THE UNIX DOMAIN SOCKET
}
