#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT_LENGTH 12
#define MAX_LINES 12

int main() {
	FILE *file = fopen("inputs/input-3", "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	// Create a counter to store for each line the number of 0 and 1
	int counter[MAX_INPUT_LENGTH][2] = {0};

	char input[MAX_INPUT_LENGTH];
	while(fscanf(file, "%s\n", input) != EOF) {
		for (int i = 0; i < MAX_INPUT_LENGTH; i++) {
			char c = input[i];

			switch (input[i]) {
			case '0':
				counter[i][0]++;
				break;
			case '1':
				counter[i][1]++;
				break;
			default:
				printf("Invalid char: %c\n", c);
				return 1;
			}
		}
	}

	uint16_t gamma_rate = 0;
	uint16_t epsilon_rate = 0;
	for (int i=0; i < MAX_INPUT_LENGTH; i++) {
		int shift = (MAX_INPUT_LENGTH - 1) - i;
		if (counter[i][1] > counter[i][0]) {
			gamma_rate |= (1 << shift);
		} else {
			epsilon_rate |= (1 << shift);
		}
	}

	printf("Gamma rate: %d\n", gamma_rate);
	printf("Epsilon rate: %d\n", epsilon_rate);
	printf("Result: %d\n", gamma_rate * epsilon_rate);

	fclose(file);
	return 0;
}
