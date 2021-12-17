#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT 100

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Missing input file");
		return 1;
	}

	FILE *file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	char *line = NULL;
	size_t n;

	int score_count = 0;
	uint64_t scores[MAX_INPUT] = {0};
	while(getline(&line, &n, file) != -1) {
		char parser[MAX_INPUT] = {0};
		int pi = 0;

		for (int i = 0; i < MAX_INPUT; i++) {
			char c = line[i];
			if (c == '\n') {
				uint64_t score = 0;
				for (int j = pi-1; j >= 0; j--) {
					c = parser[j];
					if (c == '(') score = (score * 5) + 1;
					if (c == '[') score = (score * 5) + 2;
					if (c == '{') score = (score * 5) + 3;
					if (c == '<') score = (score * 5) + 4;
				}
				scores[score_count++] = score;
				break;
			}

			if ((c == '{') || (c == '(')
			    || (c == '[') || (c == '<')) {
				parser[pi++] = c;
				parser[pi] = '\0';
				continue;
			}

			char expected;
			if (c == '}') expected='{';
			if (c == ')') expected='(';
			if (c == ']') expected='[';
			if (c == '>') expected='<';
			if (parser[pi-1] != expected) {
				break;
			}

			pi--;
		}
	}
	free(line);
	fclose(file);

	uint64_t tmp;
	for (int i = 0; i < score_count; i++) {
		for (int j = i+1; j < score_count; j++) {
			if(scores[i] > scores[j]) {
				tmp = scores[i];
				scores[i] = scores[j];
				scores[j] = tmp;
			   }
		}
	}

	printf("%ld\n", scores[score_count/2]);
	return 0;
}
