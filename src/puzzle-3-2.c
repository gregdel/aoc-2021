#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 20
#define MAX_LINES 1000

void print_number(uint16_t n, int max) {
	char result[17] = {};
	result[max] = '\0';

	for (int i = 0; i < max; i++) {
		int pos = (max - 1) - i;
		result[pos] = (((n & (1<<i)) >> i) == 1) ? '1' : '0';
	}

	printf("%d -> %s\n", n, result);
}

int count_number_of_ones(uint16_t numbers[], int lines, int pos) {
	int count = 0;
	for (int i = 0; i < lines; i++) {
		if (numbers[i] == 0)
			continue;

		if (((numbers[i] & (1 << pos)) >> pos) == 1)
			count++;
	}

	return count;
}

int filter_candidates(uint16_t numbers[], int lines, int input_length, bool reverse) {
	uint16_t candidates[MAX_LINES] = {0};
	int candidates_count = lines;
	memcpy(candidates, numbers, MAX_LINES*sizeof(uint16_t));

	for (int i = 0; i < lines; i++) {
		if (candidates[i] == 0)
			candidates_count--;
	}

	for (int col = 0; col < input_length; col++) {
		int pos = input_length - 1 - col;
		int count = count_number_of_ones(candidates, lines, pos);

		int bit = 0;
		int others = candidates_count - count;
		if (count >= others)
			bit = 1;

		if (reverse)
			bit = !bit;

		if (candidates_count == 1)
			break;

		for (int i = 0; i < lines; i++) {
			if (candidates_count == 1)
				break;

			if (candidates[i] == 0)
				continue;

			if (((candidates[i] & (1 << pos)) >> pos) == bit)
				continue;

			candidates[i] = 0;
			candidates_count--;
		}
	}

	if (candidates_count != 1) {
		printf("Invalid number of results: %d\n", candidates_count);
		return -1;
	}

	for (int i = 0; i < lines; i++) {
		if (candidates[i] != 0)
			return candidates[i];
	}

	return -1;
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

	char input[MAX_INPUT_LENGTH];
	int line = 0;
	int input_length = 0;
	uint16_t numbers[MAX_LINES] = {0};

	while(fscanf(file, "%s\n", input) != EOF) {
		// All inputs have the same size
		if (input_length == 0)
			input_length = strlen(input);

		for (int i = 0; i < input_length; i++) {
			int pos = input_length - 1 - i;
			if (input[pos] == '1')
				numbers[line] |= (1 << i);
		}

		line++;
	}

	int o2_generator = filter_candidates(numbers, line, input_length, false);
	int co2_scrubber = filter_candidates(numbers, line, input_length, true);

	printf("%d\n", o2_generator * co2_scrubber);

	fclose(file);
	return 0;
}
