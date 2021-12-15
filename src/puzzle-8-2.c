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
	for (int i = 0; i < 128; i++) {
		b->segments[i] = 255;
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

	b->segments[n] = value;
	if (value != 16)
		b->numbers[value] = n;
}

uint8_t segment_length(uint8_t n) {
	uint8_t ret = 0;
	for (int i = 0; i < DISPLAY_SEGMENTS; i++)
		if (((n >> i) & 1) == 1) ret++;

	return ret;
}

// There's only 3 segments with a length of 5 but only the number two matches
// 2|4 == 8
void find_two(struct board *b) {
	for (int i = 0; i < 128; i++) {
		if (b->segments[i] != 16) continue;
		if (segment_length(i) != 5) continue;
		if ((i|b->numbers[4]) != b->numbers[8]) continue;
		b->segments[i] = 2;
		b->numbers[2] = i;
		return;
	}
}

// There's only 3 segments with a length of 6 but only the number nine matches
// 9|4 == 9
void find_nine(struct board *b) {
	for (int i = 0; i < 128; i++) {
		if (b->segments[i] != 16) continue;
		if (segment_length(i) != 6) continue;
		if ((i|b->numbers[4]) != i) continue;
		b->segments[i] = 9;
		b->numbers[9] = i;
		return;
	}
}

// Once 2 is found, there's only 2 segments with a length of 5 but only the 5
// nine matches 5|2 == 8
void find_five(struct board *b) {
	for (int i = 0; i < 128; i++) {
		if (b->segments[i] != 16) continue;
		if (segment_length(i) != 5) continue;
		if ((i|b->numbers[2]) != b->numbers[8]) continue;
		b->segments[i] = 5;
		b->numbers[5] = i;
		return;
	}
}

// Once 2 and 5 are found, there's only 1 segments with a length of 5
void find_three(struct board *b) {
	for (int i = 0; i < 128; i++) {
		if (b->segments[i] != 16) continue;
		if (segment_length(i) != 5) continue;
		b->segments[i] = 3;
		b->numbers[3] = i;
		return;
	}
}

void find_six(struct board *b) {
	for (int i = 0; i < 128; i++) {
		if (b->segments[i] != 16) continue;
		if (segment_length(i) != 6) continue;
		if ((i|b->numbers[1]) != b->numbers[8]) continue;
		b->segments[i] = 6;
		b->numbers[6] = i;
		return;
	}
}

void find_zero(struct board *b) {
	for (int i = 0; i < 128; i++) {
		if (b->segments[i] != 16) continue;
		if (segment_length(i) != 6) continue;
		b->segments[i] = 0;
		b->numbers[0] = i;
		return;
	}
}

uint8_t decode_segment(struct board *b, char *input) {
	uint8_t ret = b->segments[number_from_input(input)];
	if (ret != 16)
		return ret;

	find_two(b);
	find_nine(b);
	find_five(b);
	find_three(b);
	find_six(b);
	find_zero(b);

	return decode_segment(b, input);
}

void print_board(struct board *b) {
	for (int i = 0; i < 10; i++) {
		printf("%d - %d\n", i, b->numbers[i]);
	}
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
	int pow10[] = {1000, 100, 10, 1};
	while(getline(&line, &n, file) != -1) {
		token = strtok(line, delim);
		j = 0;
		int k = 0;
		int value = 0;
		init_board(&boards[i]);
		while (token != NULL) {
			if (j > 13)
				break;

			if (j < 10) {
				add_segment(&boards[i], token);
			} else {
				value += pow10[k++] * decode_segment(&boards[i], token);
			}

			j++;
			token = strtok(NULL, delim);
		}

		ret += value;
		i++;
	}
	free(line);
	fclose(file);

	printf("%d\n", ret);
	return 0;
}
