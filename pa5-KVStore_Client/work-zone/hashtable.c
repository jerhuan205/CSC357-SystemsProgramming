#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LEN 33		// 32 chars + 1 null term
#define MAX_VAL_LEN 1025	// 1024 chars + 1 null term
#define DEFAULT_HT_CAP 5

// A KeyValuePair struct holds 3 pieces of information:
typedef struct KeyValuePair {
	char key[MAX_KEY_LEN];		// 1. a string representing the key
	int value;			// 2. a byte offset representing the position of the value in the database
	struct KeyValuePair* next;	// 3. another key-value pair to handle separate chaining
} KeyValuePair;

// A HashTable struct will hold a table of constant-defined pointers to KeyValuePairs
typedef struct HashTable {
	struct KeyValuePair* table[DEFAULT_HT_CAP];
	int cap;
	int size;
} HashTable;

int hash_function(const char* key)
{
	int hash = 0;
	for (int i = 0; key[i] != '\0'; i++)
	{
		hash += key[i];
	}
	return hash % DEFAULT_HT_CAP;
}

// TODO: NEED TO COMMIT ALL THESE CHANGES
void set_item(HashTable* hash_table, char* key, int value)
{
	// Create a hash index using the inputted key
	int index = hash_function(key);

	// Loop through the separate chains in the HashTable
	KeyValuePair* cur = hash_table->table[index];
	printf("%s\n", cur->key);
	while (cur != NULL)
	{
		// If we find the exact key, replace it with a new value
		if (strcmp(cur->key, key) == 0)
		{
			cur->value = value;
			return;
		}
	}

	// If the key doesn't exist, create an instance of the pair
	KeyValuePair* new_pair = (KeyValuePair*) malloc(sizeof(KeyValuePair));
	if (new_pair != NULL)
	{
		strncpy(new_pair->key, key, MAX_KEY_LEN - 1);
		new_pair->key[MAX_KEY_LEN - 1] = '\0';
		new_pair->value = value;
		new_pair->next = NULL;
	}
	else
	{
		printf("Allocation for new_pair failed.\n");
		return;
	}

	// Add the new KeyPairValue to HashTable at the front
	new_pair->next = hash_table->table[index];
	hash_table->table[index] = new_pair;
	hash_table->size += 1;

	// 
	if ((hash_table->size / hash_table->cap) > 1)
	{
		hash_table->cap *= 2;
	}
//	if ((hash_table.size / hash_table.capacity) > 1.0):
//		hash_table.capacity *= 2
//		new_table = [[] for _ in range(hash_table.capacity)]
//		for chain in hash_table.table:
//			for pair in chain:
//				pair_key = pair[0]
//				index = hash_table.hash_function(pair_key) % hash_table.capacity
//				new_table[index].append(pair)
//		hash_table.table = new_table
}
