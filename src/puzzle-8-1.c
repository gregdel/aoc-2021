#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_INPUT 1024
#define DISPLAY_SEGMENTS 7

uint8_t number_from_input(char *input) {
	if (input == NULL)
		return 0;

	uint8_t ret = 0;
	for (int i = 0; i < DISPLAY_SEGMENTS; i++) {
		char c = input[i];
		if (c == '\0') break;
		ret |= 1 << (c - 'a');
	}

	return ret;
}

struct board {
	uint8_t numbers[10];
	uint8_t segments[256];
};

void init_board(struct board *b) {
	for (int i = 0; i < 255; i++) {
		b->segments[i] = 16;
	}

	for (int i = 0; i < 10; i++) {
		b->numbers[i] = 255;
	}
}

void add_segment(struct board *b, char *input) {
	size_t len = strlen(input);
	uint8_t n = number_from_input(input);
	uint8_t value = 16;
	switch (len) {
	case 2:
		value = 1;
		break;
	case 4:
		value = 4;
		break;
	case 3:
		value = 7;
		break;
	case 7:
		value = 8;
		break;
	}

	if (value != 16) {
		b->numbers[value] = n;
		b->segments[n] = value;
	}
}

uint8_t decode_segment(struct board *b, char *input) {
	return b->segments[number_from_input(input)];
}

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

	struct board boards[MAX_INPUT];

	char *line = NULL;
	size_t n;
	char delim[] = " |";
	char *token;
	int i = 0;
	int j = 0;
	int ret = 0;
	while(getline(&line, &n, file) != -1) {
		token = strtok(line, delim);
		j = 0;
		init_board(&boards[i]);
		while (token != NULL) {
			if (j > 13)
				break;

			if (j < 10) {
				add_segment(&boards[i], token);
			} else {
				if (decode_segment(&boards[i], token) != 16)
					ret++;
			}

			j++;
			token = strtok(NULL, delim);
		}

		i++;
	}
	free(line);
	fclose(file);

	printf("%d\n", ret);
	return 0;
}
