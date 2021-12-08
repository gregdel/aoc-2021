#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT_LENGTH 12
#define MAX_LINES 12

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

	// Create a counter to store for each line the number of 0 and 1
	int counter[MAX_INPUT_LENGTH][2] = {0};

	char input[MAX_INPUT_LENGTH];
	int input_length = MAX_INPUT_LENGTH;
	while(fscanf(file, "%s\n", input) != EOF) {
		for (int i = 0; i < input_length; i++) {
			switch (input[i]) {
			case '0':
				counter[i][0]++;
				break;
			case '1':
				counter[i][1]++;
				break;
			case '\0':
				input_length=i;
			default:
				break;
			}
		}
	}

	uint16_t gamma_rate = 0;
	uint16_t epsilon_rate = 0;
	for (int i=0; i < input_length; i++) {
		int shift = (input_length - 1) - i;
		if (counter[i][1] > counter[i][0]) {
			gamma_rate |= (1 << shift);
		} else {
			epsilon_rate |= (1 << shift);
		}
	}

	printf("%d\n", gamma_rate * epsilon_rate);

	fclose(file);
	return 0;
}
