#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>



/* CONSTANTS */
#define S_FLAG_ON 1
#define A_FLAG_ON 1

#define S_FLAG_OFF 0
#define A_FLAG_OFF 0



/* PROGRAM FUNCTIONS */
int compare(const struct dirent** a, const struct dirent** b);

void echo_args(int a_flag, int s_flag, char* path);

void tree_recurse(int s_flag, int a_flag, int level, int* dir_count, int* file_count, char* path);
