/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>



/* CONSTANTS */
// Inode count constants
#define MAX_INODES 1024
#define MIN_INODES 0

// Input string constants
#define FNAME_SIZE 32
#define NULL_TERM 1
#define SPACE 1

// FS command constants
#define CMD_LS 1
#define CMD_CD 2
#define CMD_MKDIR 3
#define CMD_TOUCH 4
#define CMD_EXIT 5
#define CMD_UNKNOWN 0

// DEV command constants
#define DEV_INODES_LIST 6
#define DEV_N_INODES 7
#define DEV_DIRECTORY 8
#define DEV_N_ENTRIES 9



/* STRUCTS */
// Each inode has an associated index (a number) and type ('d' or 'f')
typedef struct {
	uint32_t index;
        char     type;
} Inode;

// Each directory will always display entries that have an inode (or similar to Inode type: index) and a name of up to 32 chars.
typedef struct {
	uint32_t inode;
	char     name[FNAME_SIZE + NULL_TERM];
} Entry;



/* DEBRIEFS */
// In memory...
// The inodes_list will be represented as an Array, holding Inode types.

// A directory will be represented as an Array, holding a sequence of Entry types.
// This is an inode that can point to other inodes through type Inode index / Entry inode

// A file will be represented as just a single instance of a name that can be up to 32 chars.

// 'Loading' actions will inolve 'open'ing and 'read'ing files, as well as populating memory blocks.


/* PROGRAM FUNCTIONS */
// Function parses and verifies program arguments for correct startup
void parse_args(int argc, char* argv1);

// Function reads the "inodes_list" file and populates the inodes_list contents in memory
// Returns the number of current inodes present in simulation
// NOTE: This number is also the next inode that can be added to the inodes_list
int load_inodes_list(Inode* inodes_list);

// Function reads the directory file and populates the dir_list contents in memory
// Returns the number of entries in the list
// NOTE: This number is also the index of the next entry to add in this directory.
int load_directory(Entry* dir_list, char* dir_name, int rem_inodes);

// Function compares the user input string with program key-strings
// Returns a constant associated to a specified string
int get_command(const char *cmd);



/* SIMULATOR FUNCTIONS */
// Function lists the files of the current directory indicated by dir_name
void fs_ls(char* dir_name);

// TODO
//void fs_cd(Inode* inodes_list, Entry* dir_list, uint32_t* dir_inode, char* dir_name, char* args, int* free_spot, int rem_inodes);
void fs_cd(Inode* inodes_list, Entry* dir_list, Entry* current_directory, char* args, int* free_spot, int rem_inodes);



/* DEBUGGING FUNCTIONS */
// "e_ilist"
void echo_present_inodes(Inode* inodes_list);

// "e_ninodes"
void echo_n_inodes(Inode* inodes_list, int n_inodes);

// "e_dir"
void echo_cur_dir(Entry* dir_list);

// "e_nitems"
void echo_n_entries(Entry* dir_list, int n_entries);
