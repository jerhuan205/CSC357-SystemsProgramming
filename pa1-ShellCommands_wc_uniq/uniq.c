#include<stdio.h>
#include<string.h>

int main(int charc, char *argv[]) {
	FILE *fp = fopen(argv[1], "r");

	if (fp == NULL) {
		fp = stdin;
	}

	char s[512];
	int line_count = 0;
	char r[512];

	while (fgets(s, 512, fp) != NULL) {
		if (line_count == 0) {
			printf("%s", s);
			line_count++;
			strcpy(r, s);
			continue;
		}
		if (strcmp(r,s) != 0) {
			printf("%s", s);
			strcpy(r, s);
		}
		line_count++;
        }
}
