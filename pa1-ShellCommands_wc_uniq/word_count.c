#include<stdio.h>
#include<ctype.h>

int main(int argc, char *argv[]) {

	FILE *fp = fopen(argv[1], "r");

	if (fp == NULL) {
		printf("> ");
		fp = stdin;
	}

	int line_count = 0;
	int word_count = 0;
	int byte_count = 0;

	char s[100];

	int is_a_word;

	while (fgets(s, 100, fp) != NULL) {
		for (int i = 0; i < 100; i++) {
			if (isspace(s[i])) {
				if (is_a_word == 1) {
					word_count++;
					is_a_word = 0;
				}
				if (s[i] == '\n') {
					line_count++;
					byte_count++;
					break;
				}
			} else {
				is_a_word = 1;
			}
			byte_count++;
		}
	}

	printf("%d %d %d\n", line_count, word_count, byte_count);
}
