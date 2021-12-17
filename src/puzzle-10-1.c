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

	int ret = 0;
	while(getline(&line, &n, file) != -1) {
		char parser[MAX_INPUT] = {0};
		int pi = 0;

		for (int i = 0; i < MAX_INPUT; i++) {
			char c = line[i];
			if (c == '\n') {
				break;
			}

			if ((c == '{') || (c == '(')
			    || (c == '[') || (c == '<')) {
				parser[pi++] = c;
				parser[pi] = '\0';
				continue;
			}

			char expected;
			int score = 0;
			if (c == '}') { expected='{'; score = 1197;  }
			if (c == ')') { expected='('; score = 3;     }
			if (c == ']') { expected='['; score = 57;    }
			if (c == '>') { expected='<'; score = 25137; }

			if (parser[pi-1] != expected) {
				ret += score;
				break;
			}

			pi--;
		}
	}
	free(line);
	fclose(file);

	printf("%d\n", ret);
	return 0;
}
