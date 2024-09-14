#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LEN 33		// 32 chars + 1 null term
#define MAX_VAL_LEN 1025	// 1024 chars + 1 null term
#define DEFAULT_HT_CAP 20

// A KeyValuePair struct holds 3 pieces of information:
typedef struct KeyValuePair {
	char key[MAX_KEY_LEN];		// 1. a string representing the key
	int value;			// 2. a byte offset representing the position of the value in the database
	struct KeyValuePair* next;	// 3. another key-value pair to handle separate chaining
} KeyValuePair;

// A HashTable struct will hold a table of constant-defined pointers to KeyValuePairs
typedef struct HashTable {
	struct KeyValuePair** table;	// Pointer to a dynamically allocated array of KeyValuePairs
	int cap;
	int size;
} HashTable;

// Simple hash function by DJB2
int hash_function(const char* key, int capacity)
{
	unsigned long hash = 5381;	// Start with a large prime
	int c;

	while ((c = *key++))
	{
		// hash * 33 + c
		hash = ((hash << 5) + hash) + c;
	}

	return (int)(hash % capacity);
}

// Function to create a new HashTable
HashTable* create_hash_table()
{
	HashTable* ht = (HashTable*) malloc(sizeof(HashTable));
	if (ht != NULL)
	{
		ht->cap = DEFAULT_HT_CAP;
		ht->size = 0;
		ht->table = (KeyValuePair**) calloc(DEFAULT_HT_CAP, sizeof(KeyValuePair*));
		if (ht->table == NULL)
		{
			free(ht); // Clean up if calloc fails
			return NULL;
		}
	}
	return ht;
}

void rehash(HashTable* ht)
{
	int old_cap = ht->cap;
	int new_cap = old_cap * 2;

	// Create a new table with double the capacity
	KeyValuePair** new_table = (KeyValuePair**) calloc(new_cap, sizeof(KeyValuePair*));
	if (new_table == NULL)
	{
		printf("Failed to allocate memory for rehashing.\n");
		return;
	}

	// Rehash all items from the old table into the new table
	for (int i = 0; i < old_cap; i++)
	{
		KeyValuePair* cur = ht->table[i];
		while (cur != NULL)
		{
			KeyValuePair* next = cur->next;
			int new_index = hash_function(cur->key, new_cap);

			// Insert into the new table
			cur->next = new_table[new_index];
			new_table[new_index] = cur;

			cur = next;
		}
	}

	// Free the old table and update the hash_table to point to the new table
	free(ht->table);
	ht->table = new_table;
	ht->cap = new_cap;

	printf("Rehashing complete. New capacity: %d\n", new_cap);
}

void set_item(HashTable* ht, char* key, int value)
{
	// Create a hash index using the inputted key and the current capacity of the hashtable
	int index = hash_function(key, ht->cap);

	// Loop through the separate chains in the HashTable
	KeyValuePair* cur = ht->table[index];
	while (cur != NULL)
	{
		// If we find the exact key, replace it with a new value
		if (strcmp(cur->key, key) == 0)
		{
			cur->value = value;
			return;
		}
		cur = cur->next;
	}

	// If the key doesn't exist, create an instance of the pair
	KeyValuePair* new_pair = (KeyValuePair*) malloc(sizeof(KeyValuePair));
	if (new_pair == NULL)
	{
		printf("Allocation for new_pair failed.\n");
		return;
	}
	strncpy(new_pair->key, key, MAX_KEY_LEN - 1);
	new_pair->key[MAX_KEY_LEN - 1] = '\0';
	new_pair->value = value;
	new_pair->next = NULL;

	// Add the new KeyPairValue to HashTable at the front and set the links appropriately
	new_pair->next = ht->table[index];
	ht->table[index] = new_pair;
	ht->size += 1;

	// Resize the hashtable if the load factor exceeds 0.75
	if ( ((float)ht->size / (float)ht->cap) > 0.75)
	{
		rehash(ht);
	}
}

int get_item(HashTable* ht, char* key)
{
	int index = hash_function(key, ht->cap);

	KeyValuePair* cur = ht->table[index];
	while (cur != NULL)
	{
		// If we find a key in the HashTable, return the value
		if ( strcmp(cur->key, key) == 0 ) {
			return cur->value;
		}
		cur = cur->next;
	}
	return -1;	// Key not found
}

void free_ht(HashTable *ht)
{
	for (int i = 0; i < ht->cap; i++)
	{
		KeyValuePair* cur = ht->table[i];
		while (cur != NULL)
		{
			KeyValuePair* temp = cur;
			cur = cur->next;
			free(temp);
		}
	}
	free(ht->table);
	free(ht);
}
