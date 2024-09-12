#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_LEN 3
#define KEY_LEN 32
#define VAL_LEN 1024
#define SPACE 1
#define NULL_TERM 1



int main(int argc, char* argv[])
{
	// Verify number of parameters from input
	if (argc < 4 || argc > 5) {
		fprintf(stderr, "Error, usage: ./kvclient <input-socket-name> <get or set> <key> \"<value>\"\n");
		exit(EXIT_FAILURE);
	}

	// Functionality Handling
	char* control = argv[2];
	char* client_key = argv[3];
	char* client_value = NULL;
	char client_request[CMD_LEN + SPACE + KEY_LEN + SPACE + VAL_LEN + NULL_TERM] = "";

	if (strcmp(control, "get") == 0)
	{
		// Error handling "get" with improper number of arguments:
		// "./kvclient <socket_name> <get> <key> <...>"
		if (argc == 5)
		{
			printf("Error, incorrect number of parameters for \"get\"\n");
			return 1;
		}

		// "get <key32>\0"
		sprintf(client_request, "%s %s", control, client_key);
	}
	else if (strcmp(control, "set") == 0)
	{
		// Error handling "set" with improper number of arguments:
		// "./kvclient <socket_name> <set> <key>"
		if (argc == 4)
		{
			printf("Error, cannot set without a given value\n");
			return 1;
		}

		// "/.kvclient <socket_name> <set> <key> <"">"
		client_value = argv[4];
		if (strcmp(client_value, "") == 0)
		{
			printf("Error, empty value\n");
			return 1;
		}

		// "set <key32> <value1024>\0"
		sprintf(client_request, "%s %s %s", control, client_key, client_value);
	}
	else
	{
		printf("Error, invalid request:\"%s\"\n", control);
		exit(EXIT_FAILURE);
	}

	// TESTING
	printf("client_request: %s\n", client_request);

	// TODO: SOCKETWORK LATER
}

