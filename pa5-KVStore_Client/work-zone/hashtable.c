#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LEN 33		// 32 characters + 1 null terminator
#define MAX_VAL_LEN 1025	// 1024 characters + 1 null terminator


// A KeyValuePair struct holds 3 pieces of information:
// Information regarding a file will follow in the particular order:
// 1. 


typedef struct KeyValuePair {
	char key[MAX_KEY_LEN];
	int value;
	struct KeyValuePair* next;	// separate chaining
} KeyValuePair;
